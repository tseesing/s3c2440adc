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
#include <s3cadc.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "nqcommon.h"

#define S3CADCDEVPRENAME "/dev/fl2440adc"


/*
 * s3c_adc_init(): initial a s3cadc structure, for control s3c adc
 * @channel:     the adc channel
 * Return a struct s3cadc, otherwise NULL and the errno indicates 
 *  the error.
 */

struct s3cadc * s3cadc_alloc(int32_t channel)
{
    struct s3cadc * ret_s3cadc;
    char * devname;
    int fnlen;
    int fd;

   /* 
     * Note: Since S3C2440 just at most 8 ADC channels, the channel 
     *  number that to construct its dev name only need one byte, 
     *  if one has a chip with 10 (or more) channels, then the "2"
     *  (including the '\0') here has to change to fit the chip.
     */

    fnlen = sizeof(S3CADCDEVPRENAME) + 2; 

    ret_s3cadc = (struct s3cadc *)malloc(sizeof(*ret_s3cadc) + fnlen);
    if(!ret_s3cadc) 
    {
        nq_debug("malloc failed\n");
        errno = ENOMEM;
        return NULL;
    }
    memset(ret_s3cadc, 0, sizeof(*ret_s3cadc));

    devname = (char *)(ret_s3cadc + 1);
    snprintf(devname, fnlen, "%s%d", S3CADCDEVPRENAME, channel);

    ret_s3cadc->devname = devname;

    fd = open(ret_s3cadc->devname, O_RDONLY);
    if (fd < 0)
    {
        nq_debug("open failed");
        free(ret_s3cadc);
        return NULL;
    }
    ret_s3cadc->devfd = fd;
    ret_s3cadc->channel = channel;
    return ret_s3cadc;
}

void s3cadc_destroy(struct s3cadc * s3c_adc)
{
    close(s3c_adc->devfd);
    free(s3c_adc);
}

