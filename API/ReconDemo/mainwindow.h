#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Interface/Interfaces.h"
#include <memory>

#include <future>
#include<thread>
#include<QDebug>
#include<utility>
#include<functional>
#include<chrono>

class ReconClientSocket;
class ReconServer;

namespace Ui {
class MainWindow;

}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static void buttonStart_thread(MainWindow* mw);//创建的线程函数，必须为static


private slots:
    //void on_loadpushButton_clicked();

    void on_actionOpen_triggered();
    //void slotDataReceived(QByteArray dataArray);
    void slotDataReceived(int length);
    void slotDisconnected(int);    
    void on_buttonStart_clicked();

    void on_buttonBrowsePipeline_clicked();

    void on_buttonFinish_clicked();

private:
    Ui::MainWindow *ui;

    virtual bool event(QEvent *event);
    void SetImportedProcessors();
    std::shared_ptr<ReconServer> reconServer;
    //ReconServer* reconServer;
    std::thread _mythread;
};

#endif // MAINWINDOW_H
