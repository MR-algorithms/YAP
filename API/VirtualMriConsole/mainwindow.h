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

    void on_testButton_clicked();

private:
    Ui::MainWindow *ui;

    bool _connected;

    int _timeId1;
    int _index1;


    void SendTestSample(int index, bool &finished);
    void ScanFinished();
    void Reset();

};

#endif // MAINWINDOW_H
