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

    QByteArray data;

    CreateDemoData(data);

    IntAndFloatArray test;
    ConvertToMystruct(data, test);


    //QString str("127.0.11.1");
    //ui->editReconHost->setText(str);
    //ui->editReconPort->setText("10");
    ui->setupUi(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::CreateDemoData(QByteArray &data)
{
    IntAndFloatArray mystruct;//
    for(unsigned int i = 0; i < mystruct.count; i ++)
    {
        mystruct.data[i] = float(i)* 1.0;
    }


    float data3[13];

    data= QByteArray::fromRawData((char*)(&mystruct), sizeof(IntAndFloatArray));

    //
    IntAndFloatArray test1;
    IntAndFloatArray *test2;
    IntAndFloatArray test3;

    memcpy(&test1, data.data(), sizeof(IntAndFloatArray));
    test2 = (IntAndFloatArray*)data.data();

    ConvertToMystruct(data, test3);
    int xx = 13;


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


    QByteArray data;

    CreateDemoData(data);

    IntAndFloatArray test;
    ConvertToMystruct(data, test);

    unsigned int x = sizeof(unsigned int);
    unsigned int y = sizeof(float);
    unsigned int z = x + 12* y;

    unsigned int stuctlen = sizeof(IntAndFloatArray);
    unsigned int buflen = data.length();

    tcpSocket->write(data);
    //tcpSocket->writeData(data.data(), data.length());

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
