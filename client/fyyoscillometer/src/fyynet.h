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



#ifndef FYY_NET_H__
#define FYY_NET_H__     1

#include <QTcpSocket>
#include <QDateTime>
#include <QHostAddress>


#include "dataitem.h"

#if !defined(SERVPORT)
#define SERVPORT 65000
#endif

class DataItem
{
    public:
        DataItem() {}
        DataItem(const DataItem & di) 
        { 
            this->ch = di.ch; 
            this->data = di.data; 
            this->dateTime = di.dateTime; 
        }
        DataItem(quint8 rCh, quint16 rData, QDateTime & dt) 
            : ch(rCh), data(rData), dateTime(dt) {}
        DataItem(struct DataGram & rDataGram)
        {
            ch   = rDataGram.channel;
            data = rDataGram.data;
            dateTime.setTime_t(rDataGram.sample_time);
        }
        DataItem & operator=(const DataItem &di) 
        {
            if (this != &di)
            {
                this->ch = di.ch;
                this->data = di.data;
                this->dateTime = di.dateTime;
            }
            return *this;
        }
        quint8 ch;
        quint32 data; 
        QDateTime dateTime;
};

class FyyNet : public QTcpSocket
{
    Q_OBJECT

    private:
        QHostAddress targetAddr;
        quint16      targetPort;
        quint8       sampleChannel;

        struct DataGram  rDataGram;
        struct DataGram  sDataGram;

    private:
        void init();
        bool sendData();
        void processData();

    public:
        FyyNet(QHostAddress addr = QHostAddress(QHostAddress::LocalHost), quint16 port = SERVPORT, quint8 ch = 0);
        ~FyyNet();
        void updateAddressPort(QHostAddress &, quint16);
        void updateSampleChannel(quint8);
        bool isReady();

    signals:
        void dataReady(DataItem);
        void timeReady(QDateTime);
        void netState(QString);

    public slots:
        bool connectNet();
        void reConnectNet();
        bool getData();
        bool getDateTime();
        void readData();
        void connectedState() { emit netState(tr("Connected")); }
        void disConnectedState() { emit netState(tr("Disconnected")); }
        void errorState() { emit netState(QString(errorString())); }
};


#endif  // FYY_NET_H__     

/* vim:set ts=4 sw=4 expandtab: */
