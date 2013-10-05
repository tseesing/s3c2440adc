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



#ifndef FL2440_S3C_ADC_H__
#define FL2440_S3C_ADC_H__

#include <sys/types.h>
#include <unistd.h>


struct s3cadc {
    int32_t channel;
    int32_t devfd;
    char * devname;
};



struct s3cadc * s3cadc_alloc(int32_t);
void s3cadc_destroy(struct s3cadc *);

#define s3cadc_get_adc(s3c_adc, buf) \
    ({ \
        const struct s3cadc * s3c_adc__ = (s3c_adc); \
        int32_t * adcbuf = (int32_t *)(buf); \
        read(s3c_adc__->devfd, adcbuf, sizeof(int32_t));})


#endif // FL2440_S3C_ADC_H__

