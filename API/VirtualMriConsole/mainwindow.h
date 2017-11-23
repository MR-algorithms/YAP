#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <memory>
#include "SampleDataProtocol.h"

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

    std::shared_ptr<QTcpSocket> tcpSocket;
    std::shared_ptr<QHostAddress> reconHost;
    bool connected;

    void CreateDemoStruct(IntAndFloatArray &mystruct);
};

#endif // MAINWINDOW_H
