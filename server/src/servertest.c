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


#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>

#include "adcnetwork.h"
#include "nqcommon.h"
#include "dataitem.h"


int main(int argc, char *argv[])
{
    int fd;
    int ret_val; 
    struct addrinfo addrhint;
    struct addrinfo *resaddr, *addr;
    struct DataGram datagram;

    if (argc != 3)
    {
        nq_errmsg_exit("./a.out ip port");
    }

    memset(&addrhint, 0, sizeof(struct addrinfo));
    addrhint.ai_family = AF_UNSPEC;
    addrhint.ai_socktype = SOCK_STREAM;

    ret_val = getaddrinfo(argv[1], argv[2], &addrhint, &resaddr); 
    if (ret_val)
    {
        nq_errmsg_exit("getaddrinfo: %s", gai_strerror(ret_val));
    }
    addr = resaddr;
    do
    {
        fd = socket(addr->ai_family, SOCK_STREAM, addr->ai_protocol);
        if (fd < 0)
        {
            continue ;
        }
        ret_val = connect(fd, addr->ai_addr, addr->ai_addrlen);
        if (ret_val == 0)
            break ;
        close(fd);
    } while((addr = addr->ai_next));

    if (!addr)
    {
        freeaddrinfo(resaddr);
        nq_errmsg_exit("no appropriate address");
    }
    freeaddrinfo(resaddr);

    memset(&datagram, 0, sizeof(struct DataGram));
    datagram.type = TYPE_GETTIME;
    datagram.channel = 0;
    datagram.valid = 1;

    ret_val = write(fd, &datagram, sizeof(struct DataGram));
    if (ret_val < 0)
    {
        close(fd);
        nq_errmsg_exit("write");
    }
    nq_debug("1. Written bytes: %d", ret_val);

    read(fd, &datagram, sizeof(struct DataGram));
    if (datagram.valid)
        printf("Recv time: %s", ctime(&datagram.sample_time));

    memset(&datagram, 0, sizeof(struct DataGram));
    datagram.type = TYPE_GETDATA;
    ret_val = write(fd, &datagram, sizeof(struct DataGram));
    if (ret_val < 0)
    {
        close(fd);
        nq_errmsg_exit("write");
    }
    nq_debug("2. Written bytes: %d", ret_val);
    read(fd, &datagram, sizeof(struct DataGram));
    if (datagram.valid)
        nq_normsg("Recv time: %s; Recv data: %d", ctime(&datagram.sample_time), datagram.data);

    close(fd);

    return 0;
}
