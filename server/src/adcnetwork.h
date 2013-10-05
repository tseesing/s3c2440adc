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



#ifndef ADC_NETWORK_H__
#define ADC_NETWORK_H__

#include <stdint.h> // Declaration of uint32_t, And why not <sys/types.h> ?
#include <s3cadc.h>

int adc_server_conn(char *serv, struct s3cadc *adcdev[], uint32_t nadcs);

#endif //  ADC_NETWORK_H__
