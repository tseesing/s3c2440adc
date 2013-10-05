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



#ifndef DATA_ITEM_H__
#define DATA_ITEM_H__   1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>

#define TYPE_UNTYPE   0
#define TYPE_GETTIME  1
#define TYPE_GETDATA  2

struct DataGram {
        uint8_t   valid; // 数据(指服务端采样的实时数据)是否有效
        uint32_t  type; // 命令类型
        time_t    sample_time;  // 采样时间
        uint32_t  data; // 采样数据
        uint8_t   channel; // 采样通道
};

#ifdef __cplusplus
}
#endif

#endif   // DATA_ITEM_H__  
