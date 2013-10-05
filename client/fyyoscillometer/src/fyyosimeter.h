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


#ifndef FYY_OSCI_H__
#define FYY_OSCI_H__    1

#include <QWidget>
#include <QDateTime>

#include "dataitem.h"
#include "fyych.h"
#include "fyystateboard.h"
#include "fyymon.h"


class FyyOscimeter: public QWidget
{
        private:
                FyyCH * ch[4];
                FyyStateBoard * chState[4];
                FyyMon * mon;

        private:
                void bindChAndState(FyyCH *, FyyStateBoard *);
                void bindChAndChart(FyyCH *, qint32);
                void init();

        public:
                FyyOscimeter(QWidget * parent = 0);
                virtual QSize sizeHint() const { return QSize(800, 680); }
};


#endif // FYY_OSCI_H__    
