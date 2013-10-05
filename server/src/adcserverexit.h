/****************************************************************************
* This file is part of s3c2440adc, a project for S3C2440A ADC.
* Copyright (C) 2013 Tseesing
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*
* Contact: chen-qx@live.cn
*
****************************************************************************/



#ifndef ADC_SERVER_EXIT_H__
#define ADC_SERVER_EXIT_H__


#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include <nqcommon.h>

static inline void adc_server_exit(const char *pidpath)
{
    pid_t pid;
    int fd;
    int ret_val;
    char strpid[20];

    if (access(pidpath, F_OK) < 0)
        return ;
    if (access(pidpath, R_OK) < 0)
    {
        nq_errmsg("%s:%s", pidpath, strerror(errno));
        return ;
    }
    fd = open(pidpath, O_RDONLY);
    if (fd < 0 && errno != EEXIST)
    {
        nq_errmsg("%s:%s", pidpath, strerror(errno));
        return ;
    }
    ret_val = read(fd, strpid, sizeof(strpid));
    if (ret_val < 0)
    {
        nq_errmsg("%s:%s", pidpath, strerror(errno));
        return ;
    }
    pid = atoi(strpid);
    close(fd);
    ret_val = kill(pid, SIGUSR1);
    if (ret_val < 0)
    {
        if (errno == ESRCH)
            unlink(pidpath);
        else
            nq_errmsg("Can't exit the running one!%s", strerror(errno));
    }
}




#endif  // ADC_SERVER_EXIT_H__
