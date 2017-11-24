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

public slots:
    void slotConnected();
    void slotDisconnected();
    void slotDataReceived();

private slots:
    void on_buttonStart_clicked();



private:
    Ui::MainWindow *ui;

    std::shared_ptr<QTcpSocket> _tcpSocket;
    std::shared_ptr<QHostAddress> _reconHost;
    bool _connected;
    int _timeId1;
    int _index1;

    void CreateDemoStruct(IntAndFloatArray &mystruct);

    void StartSendData();
    void TimerDestroyed();

    void timerEvent(QTimerEvent *e);

};

#endif // MAINWINDOW_H
