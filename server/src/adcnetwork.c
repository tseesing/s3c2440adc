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


#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <syslog.h>

#include <sys/epoll.h>
#include "nqcommon.h"
#include "dataitem.h"

#include <s3cadc.h>



void get_time(struct DataGram *);

#define get_tcp_listen_socket_nonblock(serv) \
    get_tcp_listen_socket_t((serv), 0)

#define get_tcp_listen_socket(serv) \
    get_tcp_listen_socket_t((serv), 1)

static int get_tcp_listen_socket_t(char *serv, int is_block)
{
    int listenfd;
    int ret_val;
    struct addrinfo *addr, * addrs;

    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    nq_debug("getaddr");
    ret_val = getaddrinfo(NULL, serv, &hints, &addrs);

    if (ret_val)
    {
        syslog(LOG_DEBUG, "%s", gai_strerror(ret_val));
        return -1;
    }
    addr = addrs;
    while(addr)
    {
        addr = addr->ai_next;
        nq_debug("socket");
        listenfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (listenfd < 0)
            continue;
        nq_debug("bind");
        if(!(bind (listenfd, addr->ai_addr, addr->ai_addrlen)))
            break;
        close(listenfd);
    }

    nq_debug("got addr");
    if (!addr)
    {
        syslog(LOG_ERR, "%s", strerror(errno));
        ret_val = -1;
        goto RTN_FREE_ADDRS;
    }

    nq_debug("set fd non-block");
    if ((!is_block) && (nq_set_fd_nonblocking(listenfd) < 0))
    {
        syslog(LOG_ERR, "%s", strerror(errno));
        close(listenfd);
        ret_val = -1;
        goto RTN_FREE_ADDRS;
    }

    nq_debug("listen");
    ret_val = listen(listenfd, 8);

    if (ret_val)
    {
        syslog(LOG_DEBUG, "listen:%s", strerror(errno));
        close(listenfd);
        ret_val = -1;
        goto RTN_FREE_ADDRS;
    }
    ret_val = listenfd;

RTN_FREE_ADDRS:        
    freeaddrinfo(addrs);
    return ret_val;
}

#ifndef MAXEPOLLEVENTS
#define MAXEPOLLEVENTS 10
#endif 

int adc_server_conn(char *serv, struct s3cadc **adcdev, uint32_t nadcs)
{
    uint32_t adc_val;
    uint32_t adc_ch;
    int ret_val;
    int listen_fd, conn_fd;
    int epoll_fd;
    int32_t nfds, i;
    struct DataGram datagram;
    struct epoll_event epl_ev, epl_evs[MAXEPOLLEVENTS];

    listen_fd = get_tcp_listen_socket_nonblock(serv);
    if (listen_fd < 0)
        return -1;

    nq_debug("listen [%d] socket return", listen_fd);
    epoll_fd = epoll_create(1); 
    if (epoll_fd < 0)
    {
        syslog(LOG_DEBUG, "epoll_create:%s", strerror(errno));
        ret_val = -1;
        goto RTN_CLOSE_LFD;
    }

    nq_debug("epoll_create");
    epl_ev.events = EPOLLIN | EPOLLET;
    epl_ev.data.fd = listen_fd,
    ret_val = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &epl_ev);
    if (ret_val < 0)
    {
        syslog(LOG_DEBUG,"epoll_ctl:%s", strerror(errno));
        goto RTN_CLOSE_EFD;
    }
    nq_debug("Starting to epoll wait");
    while(1)
    {
        nfds = epoll_wait(epoll_fd, epl_evs, MAXEPOLLEVENTS, -1);
        nq_debug("epoll_wait return with nfds = %d", nfds);
        if (nfds < 0)
        {
            syslog(LOG_DEBUG,"epoll_wait:%s", strerror(errno));
            ret_val = errno == EINTR ? 0 : -1;
            goto RTN_CLOSE_EFD;
        }
        for (i = 0; i < nfds; i++)
        {
            if (epl_evs[i].events & EPOLLERR || 
                epl_evs[i].events & EPOLLHUP ||
                !(epl_evs[i].events & EPOLLIN))
            {
                syslog(LOG_WARNING, "fd [%d] error occured", epl_evs[i].data.fd);
                close(epl_evs[i].data.fd);
                if (epl_evs[i].data.fd == listen_fd)
                {
                    break ; 
                }
                else
                {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, epl_evs[i].data.fd, NULL);
                    continue;
                }
            }
            else if (epl_evs[i].data.fd == listen_fd)
            {
                while(1)
                {
                    conn_fd = accept(listen_fd, NULL, NULL);
                    if (conn_fd < 0)
                    {
                        syslog(LOG_DEBUG,"accept:%s", strerror(errno));
                        break;
                    }
                    if (nq_set_fd_nonblocking(conn_fd) < 0)
                    {
                        close(conn_fd);
                        break;
                    }
                    epl_ev.events = EPOLLIN | EPOLLET;
                    epl_ev.data.fd = conn_fd;
                    ret_val = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &epl_ev);
                    if (ret_val < 0)
                    {
                        syslog(LOG_DEBUG,"epoll_ctl:%s", strerror(errno));
                        close(conn_fd);
                        break;
                    }
                }
            }
            else if (epl_evs[i].events & EPOLLIN)
            {
                memset(&datagram, 0, sizeof(struct DataGram));
                ret_val = read(epl_evs[i].data.fd, &datagram, sizeof(struct DataGram));
                if (ret_val == 0) // Read the end, means the client have close this connect
                {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, epl_evs[i].data.fd, NULL);
                    close(epl_evs[i].data.fd);
                    continue ;
                }

                switch(datagram.type)
                {
                    default:
                    case TYPE_UNTYPE:
                        nq_debug("Unknown datagram type, %d", datagram.type);
                        syslog(LOG_ERR, "Unknown datagram type");
                        datagram.valid = 0;
                        break;

                    case TYPE_GETDATA:
                        adc_ch = datagram.channel;
                        if (adc_ch >= nadcs || !adcdev[adc_ch])
                        {
                            nq_debug("Invalid channel [%d]", adc_ch);
                            syslog(LOG_ERR, "Invalid channel [%d]", adc_ch);
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, epl_evs[i].data.fd, NULL);
                            close(epl_evs[i].data.fd);
                            continue;
                        }
                        ret_val = s3cadc_get_adc(adcdev[adc_ch], &adc_val);
                        // adc_val &= 0x3ff;

                        nq_debug("Read ADC: Ch[%d], ret_val = [%d], adc_val = [%d]", adc_ch, ret_val, adc_val);
                        syslog(LOG_ERR, "Read ADC: Ch[%d], ret_val = [%d], adc_val = [%d]", adc_ch, ret_val, adc_val);
                        if (ret_val != sizeof(datagram.data))
                        {
                            datagram.valid = 0;
                        }
                        else
                        {
                            datagram.valid = 1;
                            get_time(&datagram);
                            datagram.data = adc_val;
                        }

                        break;

                    case TYPE_GETTIME: 
                        get_time(&datagram);
                        break;
                }

                do {
                    ret_val = write(epl_evs[i].data.fd, &datagram, sizeof(struct DataGram));
                } while ((ret_val == -1) && ((errno == EWOULDBLOCK) || (errno == EAGAIN)));
            }
        }
    }

    ret_val = 0;

RTN_CLOSE_EFD:
    close(epoll_fd);

RTN_CLOSE_LFD:
    close(listen_fd);

    return ret_val;
}

void get_time(struct DataGram * datagram)
{       
    time_t ret_time;

    if (!datagram)
        return;

    ret_time = time(NULL);
    if (ret_time == ((time_t)-1))
    {
        datagram->valid = 0;
    }
    else
    {
        datagram->valid = 1;
        datagram->sample_time = ret_time;
    }
}

