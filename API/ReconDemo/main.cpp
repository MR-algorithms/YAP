#include "mainwindow.h"
#include <QApplication>
#include<thread>
#include "Implement/LogImpl.h"
#include "Implement/LogUserImpl.h"
//#include <log4cplus/initializer.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   // log4cplus::Initializer initializer;

    Yap::LogUserImpl::GetInstance().SetLog(&Yap::LogImpl::GetInstance());
    Yap::LogImpl::GetInstance().AddUser(&Yap::LogUserImpl::GetInstance());

    MainWindow w;
    w.show();


    w.setWindowState(Qt::WindowMaximized);

    return a.exec();
}
