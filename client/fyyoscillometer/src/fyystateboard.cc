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



#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QPointer>
#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "fyystateboard.h"



// -----------------------


FyyLogViewer::FyyLogViewer(const QString & str, const QString & rTitle, QWidget * parent)
    : QDialog(parent)
{
    dataSrc = &str;
    title = rTitle;
    init ();
}

void FyyLogViewer::init()
{
    textEdit = new QTextEdit;
    fileName = new QLineEdit(QDir::homePath() + "/fyy_" + title + "_" + (QDateTime::currentDateTime()).toString("yyMMddhhmm") + ".log");
    fileName->setAttribute(Qt::WA_InputMethodEnabled);
    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    QHBoxLayout * hbox = new QHBoxLayout;
    QPushButton * returnBt = new QPushButton(tr("&Return"));
    QPushButton * saveBt = new QPushButton(tr("E&xport to:"));
    QPushButton * syncBt = new QPushButton(tr("S&ync"));

    textEdit->setReadOnly(true);
    textEdit->setPlainText(*dataSrc);
    connect(returnBt, SIGNAL(clicked()), this, SLOT(reject()));
    connect(saveBt, SIGNAL(clicked()), this, SLOT(saveLog()));
    connect(syncBt, SIGNAL(clicked()), this, SLOT(syncLog()));

    mainLayout->addWidget(textEdit);
    QLabel * noteLabel = new QLabel(tr("Note: Sampling date and time are from server.And may be not the same as local time."));
    noteLabel->setWordWrap(true);
    noteLabel->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(noteLabel);

    hbox->addWidget(saveBt);
    hbox->addWidget(fileName);
    mainLayout->addLayout(hbox);

    hbox = new QHBoxLayout;
    hbox->addWidget(syncBt);
    hbox->addWidget(returnBt);
    hbox->addStretch();
    mainLayout->addLayout(hbox);
    setWindowTitle(title + " " + tr("log"));
}

void FyyLogViewer::saveLog() 
{
    QFile outFile(fileName->text());
    QMessageBox::StandardButton resValue;
    bool ok;
    if (outFile.exists())
    {
        resValue = QMessageBox::warning(this, tr("Overwrite?"), tr("\"%1\" already exists !\nOverwrite it ?").arg(outFile.fileName()),
                                            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (resValue == QMessageBox::No)
            return ;
    }
    ok = outFile.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text);
    if (!ok)
    {
        QMessageBox::warning(this, tr("Open Error"), tr("\"%1\" Counld not be opened.\n").arg(outFile.fileName()));
        return ;
    }
    outFile.unsetError();
    outFile.write((textEdit->toPlainText()).toUtf8());
    if (outFile.error() != QFileDevice::NoError)
    {
        QMessageBox::warning(this, tr("Write Failed"), tr("\"%1\" Could not be written.\n%2").arg(outFile.fileName()).arg(outFile.errorString()));
        qDebug() << "Log file written failed.";
    }
    outFile.close();
}

void FyyLogViewer::syncLog()
{
    textEdit->setPlainText(*dataSrc);
}


// -----------------------

FyyStateBoard::FyyStateBoard(const QString & ch, QWidget * parent)
    : QWidget(parent)
{
    channelName = ch;
    init();
}

FyyStateBoard::FyyStateBoard(const char * ch, QWidget * parent)
    : QWidget(parent)
{
    channelName = QString(ch);
    init();
}
void FyyStateBoard::updateChannelName(const QString & ch)
{
    channelName = ch;
    channelNameLabel->setText("<span style='background-color: black; color: \"#cacaca\"; font-weight: bold'>" + channelName + "</span>");
}

void FyyStateBoard::init()
{
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::lightGray);
    setPalette(p);
    setFixedSize(FyyStateBoardGeoDefSize);

    channelNameLabel   = new QLabel("<span style='background-color: black; color: \"#cacaca\"; font-weight: bold'>" + channelName + "</span>");
    channelNameLabel->setTextFormat(Qt::RichText);
    channelNameLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    stateLabel = new QLabel("<span style='color: \"#ff0e24\";'>" + tr("Offline") + "</span>");
    stateLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    stateLabel->setTextFormat(Qt::RichText);

    sampleDateTimeLabel   = new QLabel;
    sampleDateTimeLabel->setTextFormat(Qt::RichText);
    sampleDateTimeLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    dataValueLabel     = new QLabel;
    dataValueLabel->setTextFormat(Qt::RichText);
    dataValueLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    QHBoxLayout * hbox = new QHBoxLayout;

    hbox->addWidget(channelNameLabel);
    hbox->addWidget(stateLabel);
    mainLayout->addLayout(hbox); // E.g. CH1 Sampling
    
    mainLayout->addWidget(sampleDateTimeLabel);

    dataLabel = new QLabel;
    hbox = new QHBoxLayout;
    hbox->addWidget(dataLabel);
    hbox->addWidget(dataValueLabel);
    mainLayout->addLayout(hbox);

    setToolTip(tr("Click to show log"));
}

void FyyStateBoard::updateState(const QString & str)
{
    stateLabel->setText("<span style='color: \"#ff0e24\";'>" + str + "</span>");
    stateLogAddItem(str);
}

void FyyStateBoard::updateSampleState(DataItem di)
{
    QString dateTimeRichText;
    QString dateText(di.dateTime.date().toString("yyyy-MM-dd")); 
    QString timeText(di.dateTime.time().toString("hh:mm:ss")); 
    QString valueText(QString("0x%1").arg(di.data, 0, 16));

    stateLabel->setText("<span style='color: green;'>" + tr("Sampling") + "</span>");
    dateTimeRichText.append(dateText);
    dateTimeRichText.append("  ");
    dateTimeRichText.append("<span style='font-weight: bold; color: blue;'>" + timeText + "</span>");
    sampleDateTimeLabel->setText(dateTimeRichText);
    dataLabel->setText(tr("Channel") + "[" + QString("%1").arg(di.ch) + "]  ");
    dataValueLabel->setText("<span style='font-weight: bold; background-color: \"#e2e2ff\"; color: \"#080800\";'>" + valueText + "</span>");
    stateLogAddItem(dateText + " " + timeText + " = " + valueText);
}

void FyyStateBoard::mouseReleaseEvent(QMouseEvent * menv)
{
    menv = menv;
    QPointer<FyyLogViewer> logViewer( new FyyLogViewer(stateLog, channelName, this));
    logViewer->exec();
}

void FyyStateBoard::stateLogAddItem(const QString & str)
{
    stateLog.append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh::mm::ss"));
    stateLog.append("\n\t[ ");
    stateLog.append(str);
    stateLog.append(" ]\n");
}
