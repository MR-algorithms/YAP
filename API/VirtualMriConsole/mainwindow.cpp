#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QHostAddress>
#include <QMessageBox>
#include <QByteArray>
#include "SampleDataProtocol.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    reconHost(nullptr),
    tcpSocket(nullptr),
    connected(false)
{
/*
    IntAndFloatArray testStruct;
    QByteArray dataArray;

    testStruct.CreateDemoStruct();

    testStruct.Pack(dataArray);

    IntAndFloatArray testStruct2;
    testStruct2.Unpack(dataArray);
*/

    //QString str("127.0.11.1");
    //ui->editReconHost->setText(str);
    //ui->editReconPort->setText("10");
    ui->setupUi(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::slotConnected()
{
    connected = true;
    ui->buttonStart->setText("Stop");

    /*QString message = "Hello!";

    if (tcpSocket->write(message.toLatin1(), message.length()) != message.length())
    {
        return;
    }*/


    IntAndFloatArray sendStruct;

    sendStruct.CreateDemoStruct();

    QByteArray sendArray;
    sendStruct.Pack(sendArray);

    tcpSocket->write(sendArray);

}

void MainWindow::slotDisconnected()
{
}

void MainWindow::slotDataReceived()
{
}

void MainWindow::on_buttonStart_clicked()
{


    if (!connected)
    {
        QString ip_address = ui->editReconHost->text();
        reconHost = std::make_shared<QHostAddress>();

        if (!reconHost->setAddress(ip_address))
        {
            QMessageBox::information(this, tr("Error"),
                                     tr("Server ip address error."));
            return;
        }

        tcpSocket = std::make_shared<QTcpSocket>(this);
        connect(tcpSocket.get(), &QTcpSocket::connected, this, &MainWindow::slotConnected);
        connect(tcpSocket.get(), &QTcpSocket::disconnected, this, &MainWindow::slotDisconnected);
        connect(tcpSocket.get(), &QTcpSocket::readyRead, this, &MainWindow::slotDataReceived);

        tcpSocket->connectToHost(*reconHost, ui->editReconPort->text().toInt());


    }
    else
    {
        tcpSocket->disconnectFromHost();
        connected = false;
    }
}
