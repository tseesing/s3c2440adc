
#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "fyyosimeter.h"
#include "fyystateboard.h"


int main(int argc, char **argv)
{
        QApplication app(argc, argv);
        QTranslator translator;
        translator.load("fyyoscillometer_" + QLocale::system().name(), ":/translations");
        app.installTranslator(&translator);

        QWidget * fyyosci = new FyyOscimeter;

        fyyosci->show();

        return app.exec();
}

