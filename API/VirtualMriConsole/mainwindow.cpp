#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QHostAddress>
#include <QMessageBox>
#include <QByteArray>
#include "SampleDataProtocol.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _reconHost(nullptr),
    _tcpSocket(nullptr),
    _connected(false)
{


    _index1 = 0;

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
    _connected = true;
    ui->buttonStart->setText("Stop");

    StartSendData();



    /*QString message = "Hello!";

    if (tcpSocket->write(message.toLatin1(), message.length()) != message.length())
    {
        return;
    }*/



}

void MainWindow::slotDisconnected()
{
}

void MainWindow::slotDataReceived()
{
}

void MainWindow::on_buttonStart_clicked()
{


    if (!_connected)
    {
        QString ip_address = ui->editReconHost->text();
        _reconHost = std::make_shared<QHostAddress>();

        if (!_reconHost->setAddress(ip_address))
        {
            QMessageBox::information(this, tr("Error"),
                                     tr("Server ip address error."));
            return;
        }

        _tcpSocket = std::make_shared<QTcpSocket>(this);
        connect(_tcpSocket.get(), &QTcpSocket::connected, this, &MainWindow::slotConnected);
        connect(_tcpSocket.get(), &QTcpSocket::disconnected, this, &MainWindow::slotDisconnected);
        connect(_tcpSocket.get(), &QTcpSocket::readyRead, this, &MainWindow::slotDataReceived);

        _tcpSocket->connectToHost(*_reconHost, ui->editReconPort->text().toInt());


    }
    else
    {
        _tcpSocket->disconnectFromHost();
        _connected = false;
    }
}


void MainWindow::StartSendData()
{
    int TRms = ui->editTR->text().toInt();

     killTimer(_timeId1);
     _timeId1=startTimer(TRms);
     QTimer *timer=new QTimer(this);
     timer->start();

}


void MainWindow::timerEvent(QTimerEvent *e)
{
    if(e->timerId()==_timeId1)
    {
        //
        _index1 ++;

        const double PI = 3.1415926;

        float k = _index1 % 16;
        IntAndFloatArray sendStruct;

        //sendStruct.CreateDemoStruct();

        int dataCount = 100;
        double ACQ = 2;
        double dw = ACQ / dataCount;

        //
        double T = 0.5; //
        double freq = 1 / T;//s

        double phi0 = _index1 * 5 * PI / 180;

        sendStruct.CreateDemoSinStruct(phi0 , freq,  dw, dataCount);
        for(unsigned int i = 0; i < sendStruct.data.size(); i ++)
        {
           // sendStruct.data[i] *= k;
        }


        QByteArray sendArray;
        sendStruct.Pack(sendArray);

        if(_connected)
            _tcpSocket->write(sendArray);

        //
         qDebug()<<"time1"<<_index1;
         //ui->editInfo->appendPlainText("id1");
         if(_index1 >= 1000)
         {
             this->killTimer(_timeId1);
             TimerDestroyed();
         }

    }
    else
    {
        qApp->quit();
    }


}

void MainWindow::TimerDestroyed()
{
    qDebug()<<"End timeUpdate()";
}
