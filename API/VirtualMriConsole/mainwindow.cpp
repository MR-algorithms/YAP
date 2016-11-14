#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QHostAddress>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    reconHost(nullptr),
    tcpSocket(nullptr),
    connected(false)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotConnected()
{
    QString message = "Hello!";

    if (tcpSocket->write(message.toLatin1(), message.length()) != message.length())
    {
        return;
    }
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
        connected = true;
    }
    else
    {
        tcpSocket->disconnectFromHost();
        connected = false;
    }
}
