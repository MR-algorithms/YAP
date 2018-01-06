#include "mainwindow.h"
#include <QApplication>


#include <log4cplus/initializer.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    log4cplus::Initializer initializer;

    MainWindow w;
    w.show();
    // w.setWindowState(Qt::WindowFullScreen);

    return a.exec();
}
