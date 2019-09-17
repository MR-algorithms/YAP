#include "mainwindow.h"
#include <QApplication>
#include<thread>

//#include <log4cplus/initializer.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   // log4cplus::Initializer initializer;

    MainWindow w;
    w.show();
//    std::thread t(w.on_buttonStart_clicked);
//    t.join();

    w.setWindowState(Qt::WindowMaximized);

    return a.exec();
}
