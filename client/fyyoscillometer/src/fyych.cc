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



#include <QErrorMessage>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QColorDialog>
#include <QPalette>
#include <QPointer>
#include <QIntValidator>
#include "fyych.h"


// -------------------------- 


FyyChSetDialog::FyyChSetDialog(FyyCH * beSetCh, QWidget * parent) 
    : QDialog(parent), beSetCHWidget(beSetCh)
{
    sampleInterval = beSetCh->getSampleInterval();
    targetHostCh   = beSetCh->getTargetHostCh();
    color          = beSetCh->getColor();
    name           = beSetCh->getName();
    targetPort     = beSetCh->getTargetPort();
    targetHost     = beSetCh->getTargetHost();

    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    QHBoxLayout * itemLayout;
    QGroupBox   * dataSrcGrpBox = new QGroupBox(tr("Data Source"));
    QGroupBox   * propertyGrpBox = new QGroupBox(tr("Property"));
    QVBoxLayout * dataSrcLayout = new QVBoxLayout;
    QVBoxLayout * propertyLayout = new QVBoxLayout;


    QLabel * ipTable = new QLabel(tr("IP address"));
    QLabel * portTable = new QLabel(tr("Port"));
    targetPortText = new QLineEdit;
    targetAddrText = new QLineEdit;
    targetPortText->setValidator(new QIntValidator);
    if (!(targetHost.isNull()))
    {
        targetAddrText->setText(targetHost.toString());
        targetPortText->setText(QString("%0").arg(targetPort));
    }
    itemLayout = new QHBoxLayout;
    itemLayout->addWidget(ipTable);
    itemLayout->addWidget(targetAddrText);
    dataSrcLayout->addLayout(itemLayout); // ipaddr

    itemLayout = new QHBoxLayout;
    itemLayout->addWidget(portTable);
    itemLayout->addWidget(targetPortText); // port

    chLabel = new QLabel(tr("Channel"));
    chText = new QLineEdit((QString("%0").arg(targetHostCh)));
    chText->setValidator(new QIntValidator);
    itemLayout->addStretch(0);
    itemLayout->addWidget(chLabel);
    itemLayout->addWidget(chText);
    itemLayout->addStretch(0);      // sample channel

    dataSrcLayout->addLayout(itemLayout); // port, and sample channel

    itemLayout = new QHBoxLayout;
    inteLabel = new QLabel(tr("Sample Interval(s)"));
    inteText = new QLineEdit(QString("%0").arg(sampleInterval));
    inteText->setValidator(new QIntValidator);

    itemLayout->addWidget(inteLabel);
    itemLayout->addWidget(inteText);

    dataSrcLayout->addLayout(itemLayout); // sample interval(period)

    sampleSwitch = new QCheckBox(tr("Connection and sample"));
    sampleSwitch->setCheckState(Qt::Checked);
    dataSrcLayout->addWidget(sampleSwitch); 

    dataSrcGrpBox->setLayout(dataSrcLayout);
    mainLayout->addWidget(dataSrcGrpBox);

    // -------------------

    nameLabel = new QLabel(tr("Channel Name"));
    nameText  = new QLineEdit(name);
    nameText->setAttribute(Qt::WA_InputMethodEnabled);
    itemLayout = new QHBoxLayout;
    itemLayout->addWidget(nameLabel);
    itemLayout->addWidget(nameText); // Name of sample channel

    propertyLayout->addLayout(itemLayout);

    colorNameLabel = new QLabel(color.name());
    colorNameLabel->setAutoFillBackground(true);
    QPalette p = colorNameLabel->palette();
    p.setColor(QPalette::Window, color);
    colorNameLabel->setPalette(p);

    colorBt = new QPushButton(tr("Color"));
    colorLabel = new QLabel(tr("Color"));
    itemLayout = new QHBoxLayout;
    itemLayout->addWidget(colorLabel);
    itemLayout->addWidget(colorNameLabel);
    itemLayout->addWidget(colorBt);
    itemLayout->addStretch(0);
    connect(colorBt, SIGNAL(clicked()), this, SLOT(showColorDialog())); // color for chart

    propertyLayout->addLayout(itemLayout);
    propertyGrpBox->setLayout(propertyLayout);
    mainLayout->addWidget(propertyGrpBox);
    mainLayout->addStretch(0);

    okBt = new QPushButton(tr("&OK"));
    cancelBt = new QPushButton(tr("&Cancel"));
    cancelBt->setAutoDefault(true);
    cancelBt->setDefault(true);
    connect(okBt, SIGNAL(clicked()), this, SLOT(setConf()));
    connect(cancelBt, SIGNAL(clicked()), this, SLOT(reject()));
    itemLayout = new QHBoxLayout;
    itemLayout->addStretch(1);
    itemLayout->addWidget(okBt);
    itemLayout->addWidget(cancelBt);

    mainLayout->addLayout(itemLayout);
}

void FyyChSetDialog::showColorDialog()
{
    QColor tmpColor = QColorDialog::getColor(color, this, tr("Select Color"));
    if (tmpColor.isValid())
    {
        colorNameLabel->setText(tmpColor.name());
        QPalette p = colorNameLabel->palette();
        p.setColor(QPalette::Window, tmpColor);
        colorNameLabel->setPalette(p);
    }
}

class confValueException
{
    public:
        confValueException(QWidget * wid, const char *err) : widget(wid), errStr(err) {}
        QWidget * widget;
        const char * errStr;
};

void FyyChSetDialog::setConf()
{
    qint32 tmpSampleValue;
    quint8 tmpSampleCh;
    quint16 tmpPortValue;

    QString textValue;
    QHostAddress tmpAddrValue;

    bool ok;
    try 
    { 
        textValue = inteText->text();
        if (textValue.isEmpty()) 
        {
            throw confValueException(inteText, "No sample interval was provided.");
        }
        tmpSampleValue = textValue.toInt(&ok, 10);
        if (tmpSampleValue <= 0)
        {
            throw confValueException(inteText, "Invalid sample interval.");
        }

        textValue = chText->text();
        if (textValue.isEmpty())
        {
            throw confValueException(chText, "No Channel was provided.");
        }
        tmpSampleCh = textValue.toUInt(&ok, 10);

        textValue = targetAddrText->text();
        if (textValue.isEmpty())
        {
            throw confValueException(targetAddrText, "No target host address was provided.");
        }

        if (!(verifyIPAddr(textValue, tmpAddrValue)))
        {
            throw confValueException(targetAddrText, "Invalid target host address.");
        }

        textValue = targetPortText->text();
        if (textValue.isEmpty())
        {
            throw confValueException(targetPortText, "No target host port was provided.");
        }
        tmpPortValue = textValue.toUInt(&ok, 10);
    }
    catch (confValueException & exp)
    {
        exp.widget->setFocus(Qt::OtherFocusReason);
        qDebug(exp.errStr);
        return;
    }

    if (tmpSampleValue != sampleInterval)
        beSetCHWidget->setSampleInterval(tmpSampleValue);

    if (tmpSampleCh != targetHostCh)
        beSetCHWidget->setTargetHostCh(tmpSampleCh);

    if (tmpAddrValue != targetHost)
        beSetCHWidget->setTargetHost(tmpAddrValue);

    if (tmpPortValue != targetPort)
        beSetCHWidget->setTargetPort(tmpPortValue);

    textValue = nameText->text();
    if (!(textValue.isEmpty()) && (textValue != name))
        beSetCHWidget->setName(textValue);

    textValue = colorNameLabel->text();
    if (!(textValue.isEmpty()) && (textValue != color.name()))
    {
        QColor tmpColor(textValue);
        beSetCHWidget->setColor(tmpColor);
    }

    beSetCHWidget->setAllowConnection((sampleSwitch->checkState() == Qt::Checked));

    QDialog::accept();
}

bool FyyChSetDialog::verifyIPAddr(const QString & addr, QHostAddress & addrObject)
{
    return addrObject.setAddress(addr);
}

// -------------------------- 


FyyCH::FyyCH(quint32 ch, QWidget * parent) : QWidget(parent), targetHostCh(ch)
{
    name = QString(tr("Un-named"));
    color = chDefColor;
    sampleInterval = 1000;
    init();
}

FyyCH::FyyCH(quint32 ch, const QString title, qint32 interval, QColor lcolor, QWidget *parent)
    : QWidget(parent), sampleInterval(interval), targetHostCh(ch)
{
    name = title; 
    color = lcolor; 
    init();
}

FyyCH::FyyCH(quint32 ch, const char *title, qint32 interval, QColor lcolor, QWidget *parent)
    : QWidget(parent), sampleInterval(interval), targetHostCh(ch)
{
    name = QString(title); 
    color = lcolor; 
    init();
}

void FyyCH::init()
{
    setMaximumSize(chSize);
    targetPort = SERVPORT;
    targetHost = QHostAddress::LocalHost;
    reConnect = true;
    allowConnect = false;
    fyyNet = new FyyNet;

    sampleTimer = new QTimer(this);
    sampleTimer->setInterval(sampleInterval * 1000);
    fyyNet->updateSampleChannel(targetHostCh); 

    connect(this, SIGNAL(reConnectNet()), fyyNet, SLOT(reConnectNet()));
    connect(sampleTimer, SIGNAL(timeout()), fyyNet, SLOT(getData()));
    connect(fyyNet, SIGNAL(dataReady(DataItem)), this, SIGNAL(dataReady(DataItem)));
    // connect(fyyNet, SIGNAL(dataReady(DataItem)), this, SLOT(readData(DataItem))); // For debug, remove this line when Release
    connect(fyyNet, SIGNAL(netState(QString)), this, SIGNAL(channelState(QString)));
    connect(fyyNet, SIGNAL(netState(QString)), this, SLOT(readState(QString)));
    connect(fyyNet, SIGNAL(connected()), this, SLOT(netReady()));
    connect(fyyNet, SIGNAL(disconnected()), this, SLOT(netUnReady()));

    setToolTip(tr("Click to Configure Signal Source"));
}

FyyCH::~FyyCH ()
{ 
    delete fyyNet; 
    delete sampleTimer;
}

void FyyCH::paintEvent(QPaintEvent *evp)
{
    evp = evp;
    QPainter painter(this);
    painter.fillRect(rect(), color);
    painter.drawText(0, height() / 2.0 + 4, name + " " + tr("Src")); // 对于高度, 有坐标计算, 似乎有点问题.连标题栏也算上去了(parent = 0 的情况下)
}

void FyyCH::mouseReleaseEvent(QMouseEvent * evm)
{
    evm = evm;
    int resValue;

    qDebug("mouse Release");
    QPointer <FyyChSetDialog> confDialog = new FyyChSetDialog(this, this);
    resValue = confDialog->exec();
    if (resValue == QDialog::Rejected)
    {
        return ;
    }

    if (rePaint)
    {
        update(); 
        rePaint = false;
    }

    if (allowConnect)
    {
        if (reConnect)
        {
            // Re Connect
            fyyNet->updateAddressPort(targetHost, targetPort);
            emit reConnectNet();
        }
    }
    else
    {
        fyyNet->disconnectFromHost();
    }
}

void FyyCH::setSampleInterval(qint32 & interval)
{ 
    sampleInterval = interval; 
    sampleTimer->setInterval(sampleInterval * 1000);
    if (sampleTimer->isActive())
        sampleTimer->start();
}

void FyyCH::netReady()
{
    reConnect = false;
    sampleTimer->start();
    emit dataSrcReady();
}
void FyyCH::netUnReady()
{
    reConnect = true;
    emit dataSrcUnReady();
    emit channelState(QString(tr("Connection closed")));
}

void FyyCH::readData(DataItem dataItem)
{
    qDebug() << "Sample time: " << dataItem.dateTime.toString();
    qDebug() << "Sample data: " << dataItem.data;
}


void FyyCH::readState(QString str)
{
    qDebug() << "CH State: " << str.toUtf8().constData();
}

/* vim:set ts=4 sw=4 expandtab: */
