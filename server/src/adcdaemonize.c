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


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <syslog.h>

#include "nqcommon.h"
#include "adcdaemonize.h"

int adc_server_write_pid(const char *pidpath)
{
    int fd;
    int ret_val;
    char pidstr[20];

    fd = open(pidpath, O_EXCL | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        nq_debug("%s:%s", pidpath, strerror(errno));
        return errno * (-1);
    }

    snprintf(pidstr, 20, "%d", getpid());
    ret_val = write(fd, pidstr, strlen(pidstr));
    if (ret_val < 0)
    {
        nq_debug("%s:%s", pidpath, strerror(errno));
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

void  adc_server_remove_pid(const char *pidpath)
{
    FILE *fp;
    pid_t pid;

    fp = fopen(pidpath, "r");
    if (!fp && errno == EEXIST)
    {
        nq_debug("%s:%s", pidpath, strerror(errno));
        return;
    }
    fscanf(fp, "%d", &pid);
    if(!ferror(fp))
    {
        fclose(fp);
        if (pid == getpid())
            unlink(pidpath);
        return ;
    }
    fclose(fp);
}

int adc_server_daemonize(void)
{
    int i, fdin, fdout, fderr;
    pid_t ret_pid;
    struct rlimit rl;

    umask(0);
    if ((ret_pid = fork()) < 0)
    {
        nq_debug("%s", strerror(errno));
        return -1;
    }
    else if (ret_pid > 0)
    {
        exit(0);
    }
    setsid();

    signal(SIGHUP, SIG_IGN);
    
    if ((ret_pid = fork()) < 0)
    {
        nq_debug("%s", strerror(errno));
        return -1;
    }
    else if (ret_pid > 0)
    {
        exit(0);
    }

    if (chdir("/") < 0)
    {
        nq_debug("Can't change directory");
        return -1;
    }
    
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
    {
        nq_debug("Get rlimit:%s", strerror(errno));
        return -1;
    }
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 512;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);

    fdin = open("/dev/null", O_RDWR);
    fdout = dup(0);
    fderr = dup(0);

    if (fdin != 0 || fdout != 1 || fderr != 2)
    {
        syslog(LOG_ERR, "Redicet fd failed...");
        return -1;
    }

    return 0;
}
