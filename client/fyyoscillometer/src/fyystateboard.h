
#ifndef FYY_STATE_BOARD_H__
#define FYY_STATE_BOARD_H__     1

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QString>
#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>

#include "fyynet.h"

#define FyyStateBoardGeoDefSize (QSize(180, 80))

class FyyStateBoard;
class LogViewer;

class FyyLogViewer : public QDialog
{
    Q_OBJECT

    public:
        FyyLogViewer(const QString &, const QString &, QWidget * parent = 0);
        virtual QSize sizeHint() const { return QSize(380, 300); }
       
    private:
        QTextEdit * textEdit;
        QLineEdit * fileName;
        const QString   * dataSrc;
        QString     title;

    private:
        void init();

    private slots:
        void saveLog();
        void syncLog();
};



class FyyStateBoard : public QWidget
{
    Q_OBJECT 

    private:
        QString  channelName;
        QString  stateLog;
        QLabel * stateLabel;
        QLabel * channelNameLabel;
        QLabel * sampleDateTimeLabel;
        QLabel * dataValueLabel;
        QLabel * dataLabel;

    private:
        void init();
        void stateLogAddItem(const QString &);

    public:
        FyyStateBoard(const QString &, QWidget * parent = 0);
        FyyStateBoard(const char *, QWidget * parent = 0);
        virtual QSize sizeHint() const { return FyyStateBoardGeoDefSize; }

    protected:
        virtual void mouseReleaseEvent(QMouseEvent *);

    public slots:
        void updateState(const QString &);
        void updateSampleState(DataItem);
        void updateChannelName(const QString &);

};

#endif // FYY_STATE_BOARD_H__      
