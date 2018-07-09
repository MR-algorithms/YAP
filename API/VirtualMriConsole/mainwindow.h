#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <memory>
#include "SampleDataProtocol.h"
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_connectButton_clicked();

    void on_scanButton_clicked();

    void on_stopButton_clicked();

    //void on_testButton_clicked();

    void on_browsedatabutton_clicked();

private:
    Ui::MainWindow *ui;

    virtual bool event(QEvent *event);

    unsigned int _scan_count=0;

};

#endif // MAINWINDOW_H
