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


#ifndef FYY_CH_H__
#define FYY_CH_H__      1

#define chSize  QSize(80, 20)
#define chDefColor QColor(0x00, 0xff, 0x00)

#include <QWidget>
#include <QPainter>
#include <QDialog>
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHostInfo>
#include <QTimer>
#include <QCheckBox>

#include "fyynet.h"


class FyyCH;
class FyyChSetDialog;



class FyyChSetDialog : public QDialog
{
        Q_OBJECT

        private:
                qint32 sampleInterval;
                quint8 targetHostCh;
                QColor color;          
                QString name;          
                quint16 targetPort;
                QHostAddress targetHost;
                FyyCH * beSetCHWidget;

                QLabel    * inteLabel;
                QLineEdit * inteText;
                QLabel    * chLabel;
                QLineEdit * chText;
                QLabel    * colorNameLabel;
                QLabel    * colorLabel;
                QPushButton * colorBt;
                QLabel    * nameLabel;
                QLineEdit * nameText;
                QLineEdit * targetAddrText; 
                QLineEdit * targetPortText; 
                QCheckBox * sampleSwitch;
                QPushButton * okBt;
                QPushButton * cancelBt;

        public:
                FyyChSetDialog(FyyCH * beSetCh, QWidget * parent = 0);
                virtual QSize sizeHint() { return QSize(480, 480); }

        private slots:
                void showColorDialog();
                void setConf();
                bool verifyIPAddr(const QString &, QHostAddress &);
};



class FyyCH : public QWidget
{
        Q_OBJECT

        private:
                qint32 sampleInterval;
                quint8 targetHostCh;
                QColor color;
                QString name;
                QHostAddress targetHost;
                quint16 targetPort;
                FyyNet * fyyNet;
                QTimer * sampleTimer;
                bool reConnect;
                bool rePaint;
                bool allowConnect;

        protected:
                virtual void paintEvent(QPaintEvent *);
                virtual void mouseReleaseEvent(QMouseEvent *);

        public:
                FyyCH(quint32 ch, QWidget * parent = 0);
                FyyCH(quint32 ch, const QString title, qint32 interval = 1, QColor lcolor = chDefColor, QWidget *parent = 0);
                FyyCH(quint32 ch, const char *title, qint32 interval = 1, QColor lcolor = chDefColor, QWidget *parent = 0);
                ~FyyCH();

                void setSampleInterval(qint32 &);
                qint32 & getSampleInterval() { return sampleInterval; }

                void setColor(QColor & newColor) { color = newColor; rePaint = true; emit colorChanged(color); }
                QColor & getColor() { return color; }

                void setName(QString & newName) { name = newName; rePaint = true; emit nameChanged(name); }
                QString & getName() { return name; }

                void setTargetHostCh(quint8 & newChValue) { targetHostCh = newChValue; fyyNet->updateSampleChannel(targetHostCh); }
                quint8 & getTargetHostCh() { return targetHostCh; }

                void setTargetPort(quint16 & newPort) { targetPort = newPort; reConnect = true; }
                quint16 & getTargetPort() { return targetPort; }

                void setTargetHost(QHostAddress & newHost ) { targetHost = newHost; reConnect = true; }
                QHostAddress & getTargetHost() { return targetHost; }

                virtual QSize sizeHint() const { return chSize; }
                void init();

                void setAllowConnection(bool on) { allowConnect = on; }

        public slots:
                void readData(DataItem);
                void netReady();
                void netUnReady();
                void readState(QString);


        signals:
                void dataSrcReady();
                void dataSrcUnReady();
                void colorChanged(QColor);
                void nameChanged(QString);
                void reConnectNet();
                void getData();
                void dataReady(DataItem);
                void channelState(QString);

};


#endif // FYY_CH_H__

/* vim:set ts=4 sw=4 expandtab: */
