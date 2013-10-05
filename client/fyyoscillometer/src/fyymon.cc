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


#include <QHBoxLayout>
#include <QGridLayout>
#include <QTime>
#include <QPainterPath>
#include <QPalette>

#include "fyymon.h"
#include "fyynet.h"

namespace FyyMonScreenGeometry {
    qint32 width = 560;
    qint32 height = 560;
    qint32 hSpace = 40;
    qint32 vSpace = 40;
    qint32 secondSpace = 20;
}

// -------------------------------- 

dataChart::dataChart(const QSize & rSize, qreal rSecondSpace, QWidget * parent)
    : QWidget(parent)
{
    init(rSize, rSecondSpace, Qt::white, dotType);
}

dataChart::dataChart(const QSize & rSize, qreal rSecondSpace, const QColor & rColor, enum ChartType rType, QWidget * parent)
    : QWidget(parent)
{
    init(rSize, rSecondSpace, rColor, rType);
}

        
void dataChart::init(const QSize & rSize, qreal rSecondSpace, const QColor & rColor, enum ChartType rType)
{
    setFixedSize(rSize);
    msecondSpace = rSecondSpace / 1000.0;
    color = rColor;
    type = rType;
    maxDots = width() / msecondSpace + 1;
    updateCycle = 1000; // 1s
    timer = new QTimer(this);
    timer->setInterval(updateCycle);
    connect(timer, SIGNAL(timeout()), this, SLOT(jiffies()));
    setIdle(true);

    myDraw();
}

dataChart::~dataChart()
{
    delete timer;
    sampleData.clear();
}

void dataChart::myDraw()
{
    QPainter p;
    p.begin(&paintPicture);
    switch(type)
    {
        default:
        case dotType:
            drawDot(p);
            break;

        case lineType:
            drawLine(p);
            break;

        case pillarType:
            drawPillar(p);
            break;

        case meterType:
            drawMeter(p);
            break;
    }
    p.end();
    update();
}

void dataChart::drawDot(QPainter & p)
{
    QVector<QPair<QTime, DataItem> >::iterator iter;
    qreal x, y;
    qint32 mseconds;

    QPainterPath pPath;

    for (iter = sampleData.begin(); iter != sampleData.end(); iter++)
    {
        mseconds = (iter->first).msecsTo(QTime::currentTime());
        x = mseconds * msecondSpace;
        if (x > static_cast<qreal>(width()))
        {
            break ;
        }
        y = CONVERT_TO_Y_POSITION(static_cast<qreal>((iter->second).data));
        x = width() - x;
        y = height() - y;
        // qDebug() << "x = " << x << "; " << "y = " << y;
        pPath.addEllipse(QPointF(x, y), 2.0, 2.0);
        p.fillPath(pPath, QBrush(color));
    }
    while (iter != sampleData.end())
    {
        dotRemoveOut(); // Remove the items that not within the time range
    }
}

void dataChart::drawLine(QPainter & p)
{
    QVector<QPair<QTime, DataItem> >::iterator iter;
    qreal x1, y1, x2, y2;
    qint32 mseconds;
    bool firstDot;

    firstDot = true;
    QPainterPath pPath;
    p.setPen(color);
    for (iter = sampleData.begin(); iter != sampleData.end(); iter++)
    {
        mseconds = (iter->first).msecsTo(QTime::currentTime());
        x2 = mseconds * msecondSpace;
        if (x2 > static_cast<qreal>(width()))
        {
            break ;
        }
        y2 = CONVERT_TO_Y_POSITION(static_cast<qreal>((iter->second).data));
        x2 = width() - x2;
        y2 = height() - y2;
        // qDebug() << "x = " << x << "; " << "y = " << y;

        if (firstDot)
        {
            firstDot = false;
        }
        else 
        {
            p.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
        x1 = x2; 
        y1 = y2;

        pPath.addEllipse(QPointF(x2, y2), 2.0, 2.0);
        p.fillPath(pPath, QBrush(color));
    }
    while (iter != sampleData.end())
    {
        dotRemoveOut(); // Remove the items that not within the time range
    }

}

void dataChart::drawPillar(QPainter & p)
{
    QVector<QPair<QTime, DataItem> >::iterator iter;
    qreal x, y, h;
    qint32 mseconds;
    QColor pillarColor = color;
    pillarColor.setAlpha(180);

    p.setPen(color);
    for (iter = sampleData.begin(); iter != sampleData.end(); iter++)
    {
        mseconds = (iter->first).msecsTo(QTime::currentTime());
        x = mseconds * msecondSpace;
        if (x > static_cast<qreal>(width()))
        {
            break ;
        }
        h = CONVERT_TO_Y_POSITION(static_cast<qreal>((iter->second).data));
        x = width() - x;
        y = height() - h;
        // qDebug() << "x = " << x << "; " << "y = " << y;
        p.drawRect(x, y, msecondSpace * 500, h);
        p.fillRect(x, y, msecondSpace * 500, h, QBrush(pillarColor));
    }
    while (iter != sampleData.end())
    {
        dotRemoveOut(); // Remove the items that not within the time range
    }

}

void dataChart::drawMeter(QPainter & p)
{
    QPainter * dummy = &p; 
    dummy = dummy;  // Un-supported now, constrains compiler warning
}

void dataChart::dotRemoveOut()
{
    if (sampleData.isEmpty())
    {
        timer->stop();
        return ;
    }
    sampleData.pop_back();
}

void dataChart::setUpdateCycle(qint32 rCycle)
{
    updateCycle = rCycle;
    timer->setInterval(updateCycle * 1000);
    timer->start();
}

void dataChart::paintEvent(QPaintEvent * penv)
{
    penv = penv;
    QPainter p(this);
    paintPicture.play(&p);
}

void dataChart::addData(DataItem di)
{
    qint32 dotNum = sampleData.size() - maxDots;
    while (dotNum-- >= 0)
    {
        dotRemoveOut(); // Ensuring that all data items are valid.
    }
    sampleData.push_front(QPair<QTime, DataItem>(QTime::currentTime(), di));
    if (isIdle && (type != freezeChart))
    {
        setIdle(false);
    }
}

void dataChart::jiffies()
{
    QVector<QPair<QTime, DataItem> >::iterator iter;
    for (iter = sampleData.begin(); iter != sampleData.end(); iter++)
    {
        (iter->first).addMSecs(updateCycle);
    }
    myDraw();
}

void dataChart::syncUpdateFrame()
{
    if (timer->isActive())
        timer->start();
}

void dataChart::setIdle(bool on)
{
    setVisible(!on);
    isIdle = on;
    if(on)
    {
        timer->stop();
    }
    else
    {
        timer->start();
        emit frameUpdateTimerStart();
    }
}

void dataChart::dataSrcOff()
{
    setIdle(true);
}
void dataChart::dataSrcOn()
{
    setIdle(false);
}

void dataChart::chartTypeChanged(dataChart::ChartType rType)
{
    type = rType;
    switch (type)
    {
        case dotType:
        case lineType:
        case pillarType:
        case meterType:
            myDraw();
            if (!isIdle)
            {
                setIdle(false);
            }
            break;

        case hideChart:
            setVisible(false);

        case freezeChart:
            timer->stop();
            break;
    }
}

// --------------------------------

FyyMon::FyyMon(QWidget * parent) 
    : QWidget(parent)
{
    init();
}

void FyyMon::init()
{
    int i;
    int monScrW = INT_ROUND_DOWN(FyyMonScreenGeometry::width, FyyMonScreenGeometry::hSpace); 
    int monScrH = INT_ROUND_DOWN(FyyMonScreenGeometry::height, FyyMonScreenGeometry::vSpace); 
    QSize monScrSize = QSize(monScrW, monScrH);
    monBack = new FyyMonBackground(monScrSize, FyyMonScreenGeometry::hSpace, FyyMonScreenGeometry::vSpace);

    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    QGridLayout * scrLayout = new QGridLayout;
    QHBoxLayout * idctLayout = new QHBoxLayout;
    QHBoxLayout * reFreshLayout = new QHBoxLayout;
    QLabel      * refreshLabel = new QLabel(tr("Refresh(s)")); // Too much CPU load to use millisecond
    refreshConf = new QLineEdit("1");
    refreshConf->setMaxLength(4);
    refreshConf->setFixedWidth(40);
    refreshConf->setValidator(new QIntValidator);

    initJitterTimer();
    connect(refreshConf, SIGNAL(textChanged(const QString &)), this, SLOT(refreshConfChanged(const QString &)));

    reFreshLayout->addStretch(0);
    reFreshLayout->addWidget(refreshLabel); 
    reFreshLayout->addWidget(refreshConf);
    reFreshLayout->addStretch(0);

    idctLayout->addStretch(0);

    scrLayout->addWidget(monBack, 0, 0);
    for (i = 0; i < 4; i++)
    {
        chChart[i] = new dataChart(monScrSize, FyyMonScreenGeometry::secondSpace);
        chIndicator[i] = new FyyMonChIndicator(QString("CH%0").arg(i));
        connect(chIndicator[i], SIGNAL(chartTypeChanged(dataChart::ChartType)), chChart[i], SLOT(chartTypeChanged(dataChart::ChartType)));
        connect(chChart[i], SIGNAL(syncColor(QColor)), chIndicator[i], SLOT(colorChanged(QColor))); 
        scrLayout->addWidget(chChart[i], 0, 0);
        idctLayout->addWidget(chIndicator[i]);
    }
    idctLayout->addStretch(0);
    mainLayout->addLayout(scrLayout);
    mainLayout->addLayout(reFreshLayout);
    mainLayout->addLayout(idctLayout);
    mainLayout->addStretch(0);
}

bool FyyMon::connectChart(FyyCH * ch, qint32 num)
{
    try 
    {
        if (!ch)
            throw "Invalid channel Object";
        if (num < 0 || num > 4)
            throw "Invalid channel number";
    }
    catch (const char *str)
    {
        qDebug() << str;
        return false;
    }
    chChart[num]->colorChanged(ch->getColor());
    connect(ch, SIGNAL(dataSrcReady()), chChart[num], SLOT(dataSrcOn()));
    connect(ch, SIGNAL(dataSrcUnReady()), chChart[num], SLOT(dataSrcOff()));
    connect(ch, SIGNAL(dataReady(DataItem)), chChart[num], SLOT(addData(DataItem)));
    connect(ch, SIGNAL(colorChanged(QColor)), chChart[num], SLOT(colorChanged(QColor)));

    connect(chChart[num], SIGNAL(frameUpdateTimerStart()), this, SLOT(syncUpdateFrame()));

    return true;
}

void FyyMon::disconnectChart(FyyCH * ch, qint32 num)
{
    try 
    {
        if (!ch)
            throw "Invalid channel Object";
        if (num < 0 || num > 4)
            throw "Invalid channel number";
    }
    catch (const char *str)
    {
        qDebug() << str;
        return ;
    }
    disconnect(ch, 0, chChart[num], 0);
    chChart[num]->setIdle(true);
}

void FyyMon::syncUpdateFrame()
{
    int i;
    for (i = 0; i < 4; i++)
        chChart[i]->syncUpdateFrame();
}

void FyyMon::setFramePeriod(qint32 p)
{
    int i;
    for (i = 0; i < 4; i++)
        chChart[i]->setUpdateCycle(p);
}


void FyyMon::refreshConfChanged(const QString &)
{
    jitterTimerStart();
}

void FyyMon::initJitterTimer()
{
    jitterTimer.setInterval(1500);
    jitterTimer.setSingleShot(true);
    connect(&jitterTimer, SIGNAL(timeout()), this, SLOT(updateRefreshPeriod()));
}

void FyyMon::jitterTimerStart()
{
    jitterTimer.start();
}

void FyyMon::updateRefreshPeriod()
{
    QString str = refreshConf->text();
    qint32 refValue = str.toInt();
    setFramePeriod(refValue);
}

// --------------------------------

FyyMonBackground::FyyMonBackground(const QSize & rSize, qint32 rHspace, qint32 rVspace)
{
    setFixedSize(rSize);
    hlineSpace = rHspace;
    vlineSpace = rVspace;
    color = QColor(0x00, 0x00, 0x00);
    myDraw();
}

void FyyMonBackground::myDraw()
{
    QPainter p;
    int i, j, w, h;
    w = width();
    h = height();
    p.begin(&paintPicture);

    p.fillRect(rect(), color);
    p.setPen(QColor(Qt::darkGray));
    for (i = 0, j = 0; i <= h && j <= w; i += vlineSpace, j += hlineSpace)
    {
        p.drawLine(0, i, w, i); // Horizontal lines
        p.drawLine(j, 0, j, h); // Vertical lines
    }
    while (i <= h)
    {
        p.drawLine(0, i, w, i); // Horizontal lines
        i += vlineSpace;
    }
    while (j <= w)
    {
        p.drawLine(j, 0, j, h); // Vertical lines
        j += hlineSpace;
    }
    p.end();
    update();
}

void FyyMonBackground::paintEvent(QPaintEvent * penv)
{
    penv = penv;
    QPainter p(this);
    p.drawPicture(0, 0, paintPicture);
}


// --------------------------------

FyyMonChIndicator::FyyMonChIndicator(const QString & str, QWidget * parent)
    : QWidget(parent)
{
    init(Qt::gray, str);
}

FyyMonChIndicator::FyyMonChIndicator(const char * str, QWidget * parent)
    : QWidget(parent)
{
    init(Qt::gray, QString(str));
}

FyyMonChIndicator::FyyMonChIndicator(const QColor & rColor, const QString & str, QWidget * parent)
    : QWidget(parent)
{
    init(rColor, str);
}

FyyMonChIndicator::FyyMonChIndicator(const QColor & rColor, const char * str, QWidget * parent)
    : QWidget(parent)
{
    init(rColor, QString(str));
}

void FyyMonChIndicator::init(const QColor & rColor, const QString & rTitle)
{
    color = rColor;
    title = rTitle;

    QHBoxLayout * mainLayout = new QHBoxLayout(this);

    nameLabel = new QLabel(title);
    chartFunc = new QComboBox;

    nameLabel->setAutoFillBackground(true); // Required for changing its background.
    setIndictatorBackgroud();

    chartFunc->addItem(QString(tr("Dot")));
    chartFunc->addItem(QString(tr("Line")));
    chartFunc->addItem(QString(tr("Pillar")));
    // chartFunc->addItem(QString(tr("Meter"))); // Un-supported now
    chartFunc->insertSeparator(5);
    chartFunc->addItem(QString(tr("Freeze")));
    chartFunc->addItem(QString(tr("Offline")));

    connect(chartFunc, SIGNAL(currentIndexChanged(qint32)), this, SLOT(chartSelectionChanged(qint32)));

    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(chartFunc);

}

void FyyMonChIndicator::colorChanged(QColor rColor)
{
    color = rColor;
    setIndictatorBackgroud();
}

void FyyMonChIndicator::setIndictatorBackgroud()
{
    QPalette p = nameLabel->palette();
    p.setColor(QPalette::Window, color);
    nameLabel->setPalette(p);
}

void FyyMonChIndicator::chartSelectionChanged(qint32 index)
{
    switch(index)
    {
        default:
        case 0:
            emit chartTypeChanged(dataChart::dotType);
            break;
        case 1:
            emit chartTypeChanged(dataChart::lineType);
            break;
        case 2:
            emit chartTypeChanged(dataChart::pillarType);
            break;
            /*
        case 3:
            emit chartTypeChanged(dataChart::meterType);
            break;
            // Don't forget the separator
            */
        case 4:
            emit chartTypeChanged(dataChart::freezeChart);
            break;
        case 5:
            emit chartTypeChanged(dataChart::hideChart);
            break;
    }
}
