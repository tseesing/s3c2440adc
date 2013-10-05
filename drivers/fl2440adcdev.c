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


#include <linux/platform_device.h>
#include <linux/err.h>
#include "fl2440adc.h"

//  extern struct platform_device s3c_device_adc; // Fuck s3c-adc

static struct platform_device * fl2440adcdev[ADCNUM];



struct platform_device ** fl2440adc_register_device(void)
{
    int loopidx;

    for (loopidx = 0; loopidx < ADCNUM; loopidx++)
    {
        fl2440adcdev[loopidx] = platform_device_register_simple(ADCDEVNAME, loopidx, NULL, 0);
        if (IS_ERR_OR_NULL(fl2440adcdev[loopidx]))
        {
            for (--loopidx; loopidx >=0; loopidx--)
            {
                platform_device_unregister(fl2440adcdev[loopidx]);
            }
            return NULL;
        }
        fl2440adcdev[loopidx]->dev.parent = NULL;
    }

    return fl2440adcdev;

}
EXPORT_SYMBOL_GPL(fl2440adc_register_device);

void fl2440adc_unregister_device(struct platform_device **fl2440adcdev)
{
    int loopidx; 

    for (loopidx = 0; loopidx < ADCNUM; loopidx++)
    {
        platform_device_unregister(fl2440adcdev[loopidx]);
    }

}
EXPORT_SYMBOL_GPL(fl2440adc_unregister_device);
