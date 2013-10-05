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



#include "fyynet.h"

FyyNet::FyyNet(QHostAddress addr, quint16 port, quint8 ch)
    : targetAddr(addr), targetPort(port), sampleChannel(ch)
{
    sDataGram.type    = TYPE_UNTYPE;
    sDataGram.valid   = 0;
    sDataGram.channel = sampleChannel;
    rDataGram.valid   = 0;

    connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(this, SIGNAL(connected()), this, SLOT(connectedState()));
    connect(this, SIGNAL(disconnected()), this, SLOT(disConnectedState()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorState()));
}

FyyNet::~FyyNet()
{
    abort();
    disconnect();
}

bool FyyNet::isReady()
{
    return state() == QAbstractSocket::ConnectedState;
}

void FyyNet::updateSampleChannel(quint8 ch)
{
    sDataGram.channel = ch;
}

bool FyyNet::getData()
{
    sDataGram.type = TYPE_GETDATA;
    return sendData();
}

bool FyyNet::getDateTime()
{
    sDataGram.type = TYPE_GETTIME;
    return sendData();
}

bool FyyNet::sendData()
{
    if (state() != QAbstractSocket::ConnectedState)
    {
        return false;
    }
    return  (sizeof(struct DataGram) == write((const char *)(&sDataGram), static_cast<qint64> (sizeof(struct DataGram))));
}

void FyyNet::updateAddressPort(QHostAddress & newAddr, quint16 newPort)
{
    targetAddr = newAddr;
    targetPort = newPort;
    /*
       QAbstractSocket::SocketState curState = state();
       if ((curState == QAbstractSocket::UnconnectedState) ||
       (curState == QAbstractSocket::ClosingState))
       {
       return;
       } 
     */
    disconnectFromHost();
}

void FyyNet::reConnectNet()
{
    abort(); 
    connectNet();
}

/* 
 * Return "true" just represents that connectToHost() 
 * had been successfully called. Have nothing about the
 * subsequent connected state.
 */
bool FyyNet::connectNet() 
{
    try
    {
        if (targetAddr.isNull())
        {
            throw QString(tr("Null address"));
        }
        connectToHost(targetAddr, targetPort);
    }
    catch (QString & expMsg)
    {
        qDebug() << expMsg.toUtf8().constData();
        emit netState(expMsg);
        return false;
    }
    return true;
}

void FyyNet::readData()
{
    qint64 rSize;
    rSize = read((char *)(&rDataGram), static_cast<qint64>(sizeof(struct DataGram)));

    if (rSize != sizeof(struct DataGram))
    {
        QString errStr(tr("Received invalid datagram and discard it"));
        qDebug(errStr.toUtf8().constData());
        emit netState(errStr);
        return ;
    }
    processData();
}

void FyyNet::processData()
{
    DataItem dataItem;

    if ((rDataGram.valid) && (rDataGram.channel == sDataGram.channel))
    {
        dataItem = DataItem(rDataGram);
    }
    else
    {
        emit netState(QString(tr("Reveived invalid datagram")));
        return ;
    }
    switch (rDataGram.type)
    {
        case TYPE_GETDATA:
            emit dataReady(dataItem);
            break;

        case TYPE_GETTIME:
            emit timeReady(dataItem.dateTime);
            break;

        default:
            rDataGram.valid = 0;
            emit netState(QString(tr("Unknown datagram type")));
            break;
    }
}


/* vim:set ts=4 sw=4 expandtab: */
