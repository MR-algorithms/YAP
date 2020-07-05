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


namespace Ui {
class MainWindow;

}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void customEvent(QEvent *event);
private slots:
    void on_actionOpen_triggered();
    void slotDataReceived(int length);
    void slotDisconnected(int);    
    void on_buttonStart_clicked();

    void on_buttonBrowsePipeline_clicked();
    void on_buttonFinish_clicked();

private:
    Ui::MainWindow *ui;
    void SetImportedProcessors();
    void Threadfunc_reactor();
    void OnSampleDataEvent(QEvent *event);
    std::shared_ptr<std::thread> _reactor_thread;

};

#endif // MAINWINDOW_H
