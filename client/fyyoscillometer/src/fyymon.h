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


#ifndef FYY_MON_H__
#define FYY_MON_H__     1

#include <QWidget>
#include <QColor>
#include <QVector>
#include <QPair>
#include <QPainter>
#include <QPicture>
#include <QComboBox>
#include <QTime>

#include "fyych.h"
#include "fyynet.h"

#ifndef DATA_RAW2VALUE
#define DATA_RAW2VALUE(maxValue, rawData, maxRaw) (((rawData) * (maxValue)) / (maxRaw))
#endif

#define INT_ROUND_DOWN(x, div) (((x) / (div)) * (div))

// Y position in the Coordinate
#define CONVERT_TO_Y_POSITION(rawValue) DATA_RAW2VALUE((height()), (rawValue), 0x3ff)

class dataChart : public  QWidget
{
    Q_OBJECT 

    public:
        enum ChartType {
            dotType,
            lineType,
            pillarType,
            meterType,
            hideChart,
            freezeChart
        };

    private:
        QColor color;
        QVector<QPair<QTime, DataItem> > sampleData;
        enum ChartType type;
        QPicture    paintPicture;
        QTimer   *  timer;
        qint32      updateCycle;
        bool        isIdle;

        qreal msecondSpace; // How much space one second occupies in the X axis.
        qint32 maxDots;

    private:
        void init(const QSize &, qreal, const QColor &, enum ChartType);
        void drawDot(QPainter &);
        void drawLine(QPainter &);
        void drawPillar(QPainter &);
        void drawMeter(QPainter &);
        void dotRemoveOut();

    protected:
        virtual void paintEvent(QPaintEvent *);

    public:
        dataChart(const QSize &, qreal, QWidget * parent = 0);
        dataChart(const QSize &, qreal, const QColor &, enum ChartType, QWidget * parent = 0);
        ~dataChart();
        void setIdle(bool);
        void syncUpdateFrame();

    signals:
        void frameUpdateTimerStart();
        void syncColor(QColor);

    public slots:
        void addData(DataItem);
        void colorChanged(QColor rColor) { color = rColor; myDraw(); emit syncColor(color); }
        void chartTypeChanged(dataChart::ChartType);
        void setUpdateCycle(qint32);
        void dataSrcOff();
        void dataSrcOn();

    private slots:
        void myDraw();
        void jiffies();  
};

class FyyMonBackground : public QWidget 
{
    private:
        qint32  hlineSpace, vlineSpace;
        QPicture paintPicture;
        QColor color;

    private:
        void myDraw();

    public:
        FyyMonBackground(const QSize &, qint32, qint32);
        virtual void paintEvent(QPaintEvent *);
};

class FyyMonChIndicator : public QWidget
{
    Q_OBJECT 

    private:
        QColor   color;
        QString  title;
        QLabel * nameLabel;
        QComboBox * chartFunc;

    private:
        void init(const QColor &, const QString &);
        void setIndictatorBackgroud();

    public:
        FyyMonChIndicator(const QString &, QWidget * parent = 0);
        FyyMonChIndicator(const char *, QWidget * parent = 0);
        FyyMonChIndicator(const QColor &, const QString &, QWidget * parent = 0);
        FyyMonChIndicator(const QColor &, const char *, QWidget * parent = 0);

    signals:
        void chartTypeChanged(dataChart::ChartType);

    public slots:
        void colorChanged(QColor);
        void chartSelectionChanged(qint32);
};

class FyyMon : public QWidget
{
    Q_OBJECT

    private:
        FyyMonBackground * monBack;
        dataChart * chChart[4];
        FyyMonChIndicator * chIndicator[4];
        QLineEdit * refreshConf;
        QTimer jitterTimer;
   
    private:
        void init();
        void initJitterTimer();
        void jitterTimerStart();

    private slots:
        void updateRefreshPeriod();

    public:
        FyyMon(QWidget * parent = 0);
        bool connectChart(FyyCH *, qint32);

    signals:
        void newData(DataItem);

    public slots:
        void disconnectChart(FyyCH *, qint32);
        void syncUpdateFrame();
        void setFramePeriod(qint32);
        void refreshConfChanged(const QString &);
};

#endif // FYY_MON_H__      
