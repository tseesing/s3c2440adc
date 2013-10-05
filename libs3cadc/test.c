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
#include <s3cadc.h>
#include <string.h>
#include <errno.h>
#include "nqcommon.h"

int main(int argc, char *argv[])
{
    int ret_val;
    int adc_result;


    struct s3cadc * adcdev;

    adcdev = s3cadc_alloc(0);
    if (!adcdev)
    {
        nq_errmsg_exit("%s:%s", "s3cadc_alloc",  strerror(errno));
    }

    ret_val = s3cadc_get_adc(adcdev, &adc_result);
    if (ret_val < 0)
    {
        nq_errmsg("%s:%s", "s3c_get_adc", strerror(errno));
        s3cadc_destroy(adcdev);
    }

    nq_normsg("ADC [%d] = %d\n", 0, adc_result);

    s3cadc_destroy(adcdev);

    return 0;
}

