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

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <syslog.h>

#include <s3cadc.h>

#include "nqcommon.h"
#include "adcserverexit.h"
#include "adcdaemonize.h"
#include "adcnetwork.h"

#ifndef PROGNAME
#define PROGNAME "adcserver"
#endif

#ifndef VERSION
#define VERSION "1.0"
#endif


#ifndef DEFPORT
#define DEFPORT "65000"
#endif 

#ifndef DEFPID
#define DEFPID  "/run/"PROGNAME".pid"
#endif 

#ifndef DEFCONF
#define DEFCONF "/etc/"PROGNAME"/"PROGNAME".conf"
#endif 

#ifndef MAXADC
#define MAXADC 4
#endif

/*
 * -f  前景运行
 * -c  配置文件路径(未实现)
 * -p  监听端口
 * -P  pid 
 * -x  退出目前在后台运行的副本
 * -h  帮助
 */

#define OPTSTRING   "fc:p:P:xh"

#define OPTFOREGROUND   (1<<0)
#define OPTCONFPATH     (1<<1)
#define OPTPORT         (1<<2)
#define OPTPIDPATH      (1<<3)
#define OPTQUIT         (1<<4)
#define OPTHELP         (1<<5)
#define OPTERROR        (1<<6)

void sig_hanler(int signo);

void sig_hanler(int signo)
{
    /* 
     * use SIGUSR1 to exit epoll_wait()
     *  at present 
     */
    signal(signo, SIG_IGN);
}

int main(int argc, char *argv[])
{
    int opt, ret_val;
    int32_t options = 0;
    // char *confpath; // Unimplemented
    char *pidpath;
    char *port;
    char *ch_c, *ch_tmp;
    int ch_num, i;
    struct s3cadc * adcdev[MAXADC];

    while ((opt = getopt(argc, argv, OPTSTRING)) > 0)
    {
        switch(opt)
        {
            case 'f':
                options |= OPTFOREGROUND;
                break;

            /*
            case 'c':
                options |= OPTCONFPATH;
                confpath = optarg;
                break;
            */

            case 'p':
                options |= OPTPORT;
                port = optarg;
                break;

            case 'P':
                options |= OPTPIDPATH;
                pidpath = optarg;
                break;

            case 'x':
                options = OPTQUIT;
                break;

            case 'h':
                options |= OPTHELP;
                break;

            default:
                options |= OPTERROR;
                break;
        }
    }
    for (i = 0; i < MAXADC; i++)
        adcdev[i] = NULL;
    if (optind == argc)
        adcdev[0] = (struct s3cadc *)1; // If not specifies, channel 0 is default; Now Mark it;
    while((optind < argc) && (!(options & (OPTERROR | OPTHELP))))
    {
        ch_c = argv[optind];
        ch_tmp = ch_c;
        while(*ch_tmp != '\0')
        {
            if (!isdigit(*ch_tmp))
            {
                options |= OPTERROR;
                nq_errmsg("Invalid channel [%s]", argv[optind]);
                break;
            }
            ++ch_tmp;
        }
        if (options & OPTERROR) // Possibility the loop above has changed it
            break;

        ch_num = atoi(ch_c);
        if (ch_num > MAXADC)
        {
            options |= OPTERROR;
            nq_errmsg("Invalid channel [%s]", argv[optind]);
            break;
        }
        adcdev[ch_num] = (struct s3cadc *)1; 
        ++optind;
    }

    if (options & OPTERROR)
    {
        nq_errmsg("%s specified -h to show usage", PROGNAME);
        return 1;
    }
    if (options & OPTHELP)
    {
        nq_normsg("\
"PROGNAME": The server for reading ADC data and echo to client\n\
VERSION: "PROGNAME"/"VERSION"\n\n\
Usage: "PROGNAME" [option] ... [adc channel0] [adc channel1] ...\n\
         Note: ADC channels count from 0.\n\
Example:\n\
        "PROGNAME" 0 \n\
       \n\
Options:\n\
    -f              :Running in foreground (background is default)\n\
    -p  <port>      :The port will be listened\n\
    -P  <pidfile>   :The pid file path\n\
    -x              :Exit the running one\n\
    -h              :This help message\n");

        return 0;
    }

    if (!(options & OPTPIDPATH))
        pidpath = DEFPID;

    if (options & OPTQUIT)
    {
        adc_server_exit(pidpath);
        return 0;
    }

    if (!(options & OPTFOREGROUND))
    {
        if(adc_server_daemonize())
            nq_warnmsg("Can't daemonize...");
    }

    for (i = 0; i < MAXADC; i++)
    {
        if (adcdev[i])
        {
            adcdev[i] = s3cadc_alloc(i);
            if (!adcdev[i])
            {
                nq_errmsg("Alloc ADC device [%d] failed.", i);
                goto RTN_FREE_ADC;
            }
        }
    }

        nq_debug("Writes pid");
    if(adc_server_write_pid(pidpath))
    {
        ret_val = 1;
        goto RTN_FREE_ADC;
    }

        nq_debug("Signal");
    signal(SIGUSR1, sig_hanler);

    if (!(options & OPTPORT))
    {
        port = DEFPORT;
    }

        nq_debug("starts server");
    if (adc_server_conn(port, adcdev, MAXADC))
    {
        nq_debug("failed to start");
        ret_val = 1;
        goto RTN_RM_PID_FILE;
    }

    ret_val = 0;
    nq_debug("back to main");
    syslog(LOG_DEBUG, "Back to main");

RTN_RM_PID_FILE:
    adc_server_remove_pid(pidpath);

RTN_FREE_ADC:
    for (i = 0; i < MAXADC; i++)
    {
        if (adcdev[i])
            s3cadc_destroy(adcdev[i]);
    }

    return ret_val;
}

