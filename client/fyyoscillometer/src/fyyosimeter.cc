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
#include <QVBoxLayout>
#include <QIcon>
#include <QPixmap>

#include "fyyosimeter.h"
#include "fyych.h"

FyyOscimeter::FyyOscimeter(QWidget * parent) 
    : QWidget(parent)
{
    init();
}

void FyyOscimeter::init()
{
    int i;
    QHBoxLayout * mainLayout = new QHBoxLayout(this);
    QVBoxLayout * leftLayout = new QVBoxLayout;
    QVBoxLayout * rightLayout = new QVBoxLayout;
    leftLayout->setSpacing(2);

    mon = new FyyMon;

    ch[0]= new FyyCH(0, "CH0", 1, QColor(Qt::green));
    ch[1]= new FyyCH(1, "CH1", 1, QColor(Qt::yellow));
    ch[2]= new FyyCH(2, "CH2", 1, QColor(Qt::blue));
    ch[3]= new FyyCH(3, "CH3", 1, QColor(Qt::white));

    chState[0] = new FyyStateBoard("CH0");
    chState[1] = new FyyStateBoard("CH1");
    chState[2] = new FyyStateBoard("CH2");
    chState[3] = new FyyStateBoard("CH3");

    for (i = 0; i < 4; i++)
    {
        bindChAndState(ch[i], chState[i]);
        bindChAndChart(ch[i], i);
    }

    leftLayout->addSpacing(12);
    QLabel * logo = new QLabel;
    logo->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    logo->setAttribute(Qt::WA_TransparentForMouseEvents);
    logo->setPixmap(QPixmap(":/logo.png"));
    leftLayout->addWidget(logo);

    QHBoxLayout * hbox = new QHBoxLayout;
    QVBoxLayout * vbox = new QVBoxLayout;
    vbox->setSpacing(2);
    hbox->setSpacing(2);
    hbox->addWidget(ch[0]);
    hbox->addWidget(ch[1]);
    vbox->addLayout(hbox);

    hbox = new QHBoxLayout;
    hbox->setSpacing(2);
    hbox->addWidget(ch[2]);
    hbox->addWidget(ch[3]);
    vbox->addLayout(hbox);

    leftLayout->addLayout(vbox); // Channel Source Control Widgets.
    leftLayout->addSpacing(20);

    vbox = new QVBoxLayout;
    vbox->setSpacing(2);
    for (i = 0; i < 4; i++)
    {
        vbox->addWidget(chState[i]);
    }
    leftLayout->addLayout(vbox);
    leftLayout->addStretch();

    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch(0);
    // ------------------ 

    rightLayout->addWidget(mon);
    mainLayout->addLayout(rightLayout);

    setWindowTitle("Fyy Oscillometer");
    setWindowIcon(QIcon(":/icon.png"));
}

void FyyOscimeter::bindChAndState(FyyCH * ch, FyyStateBoard * state)
{
    connect(ch, SIGNAL(channelState(QString)), state, SLOT(updateState(const QString &)));
    connect(ch, SIGNAL(dataReady(DataItem)),   state, SLOT(updateSampleState(DataItem)));
    connect(ch, SIGNAL(nameChanged(QString)),  state, SLOT(updateChannelName(const QString &)));
}
void FyyOscimeter::bindChAndChart(FyyCH * ch, qint32 num)
{
    mon->connectChart(ch, num);
}
