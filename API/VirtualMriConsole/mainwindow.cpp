#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QHostAddress>
#include <QMessageBox>
#include <QByteArray>
#include "SampleDataProtocol.h"
#include <QDebug>
#include <vector>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _reconHost(nullptr),
    _tcpSocket(nullptr),
    _connected(false)
{

    std::vector<int> a;

    a.reserve(100);
    a.resize(50);
    qDebug()<<a.size()<<"  "<<a.capacity()<<endl;
    a.resize(150);
    qDebug()<<a.size()<<"  "<<a.capacity()<<endl;
    a.reserve(50);
    qDebug()<<a.size()<<"  "<<a.capacity()<<endl;
    a.resize(50);
    qDebug()<<a.size()<<"  "<<a.capacity()<<endl;

    //QString str("127.0.11.1");
    //ui->editReconHost->setText(str);
    //ui->editReconPort->setText("10");
    ui->setupUi(this);

    Reset();

    //TestSampleStruct();

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::slotDisconnected()
{
}

void MainWindow::slotDataReceived()
{
}

void MainWindow::on_connectButton_clicked()
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

        Reset();
    }
}

void MainWindow::Reset()
{
    ui->connectButton->setText("Connect");
    _index1 = 0;
    ui->scanButton->setEnabled(false);
    ui->stopButton->setEnabled(false);

    _connected = false;

}

void MainWindow::slotConnected()
{
    _connected = true;
    ui->connectButton->setText("Disconnect");
    ui->scanButton->setEnabled(true);



    /*QString message = "Hello!";

    if (tcpSocket->write(message.toLatin1(), message.length()) != message.length())
    {
        return;
    }*/



}


void MainWindow::StartTimer()
{
    bool temp;

    //SendDemoStruct(_index1);
    //this->SendTestSample(0, temp);

    int TRms = ui->editTR->text().toInt();

    if(_timeId1 != -1)
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
        ui->editInfo->appendPlainText(QString("%1").arg(_index1));

        //

        bool finished = false;

        SendTestSample(_index1, finished);

        if(finished)
        {
            ScanFinished();
        }
        //
        qDebug()<<"time1"<<_index1;



    }
    else
    {
        qApp->quit();
    }


}

void MainWindow::SendDemoStruct(int index)
{
    const double PI = 3.1415926;

    //
    FloatArray sendStruct;

    int dataCount = 100;
    double ACQ = 2;
    double dw = ACQ / dataCount;
    double T = 0.5; //
    double freq = 1 / T;//s
    double phi0 = index * 5 * PI / 180;
    sendStruct.CreateDemoSinStruct(phi0 , freq,  dw, dataCount);
    //

    QByteArray sendArray;
    sendStruct.Pack(sendArray);

    if(_connected)
        _tcpSocket->write(sendArray);

}


void MainWindow::SendTestSample(int index, bool &finished)
{
    //初始化三种数据结构
    SampleDataStart sampleStart;
    sampleStart.dim1_size = 128;
    sampleStart.dim2_size = 1;
    sampleStart.dim3_size = 1;
    sampleStart.dim4_size = 1;
    sampleStart.dim5_size = 1;
    sampleStart.dim6_size = 1;
    sampleStart.dim23456_size = sampleStart.dim2_size * sampleStart.dim3_size * sampleStart.dim4_size;


    //
    const double PI = 3.1415926;
    double ACQ = 2;
    double dw = ACQ / sampleStart.dim1_size;
    double T = 0.5; //
    double freq = 1 / T;//s
    double phi0 = index * 5 * PI / 180;
//
    SampleDataData  sampleData;
    sampleData.dim23456_index = 24;

    sampleData.CreatDemoData(phi0,freq, dw, sampleStart.dim1_size);

    SampleDataEnd   sampleEnd(999);

    //打包发送

    QByteArray dataArray1;
    QByteArray dataArray2;
    QByteArray dataArray3;

    sampleStart.Pack(dataArray1);
    sampleData.Pack(dataArray2);
    sampleEnd.Pack(dataArray3);

    uint32_t dataMark = 0xFFFFFFFF;
    QByteArray markArray = QByteArray::fromRawData((char*)(&dataMark), sizeof(uint32_t));

    //uint32_t temp = 0x3424;
    //QByteArray tempArray = QByteArray::fromRawData((char*)(&temp), sizeof(uint32_t));

    int sendedBytes = 0;
    if(_connected)
    {

        //_tcpSocket->write(tempArray);
        //sendedBytes += tempArray.length();
        //qDebug()<< "sended bytes" << sendedBytes;

        _tcpSocket->write(markArray);
        sendedBytes += markArray.length();
        qDebug()<< "sended bytes" << sendedBytes;

        _tcpSocket->write(dataArray1);
        sendedBytes += dataArray1.length();
        qDebug()<< "sended bytes" << sendedBytes;

        _tcpSocket->write(markArray);
        sendedBytes += markArray.length();
        qDebug()<< "sended bytes" << sendedBytes;

        _tcpSocket->write(dataArray2);
        sendedBytes += dataArray2.length();
        qDebug()<< "sended bytes" << sendedBytes;

        _tcpSocket->write(markArray);
        sendedBytes += markArray.length();
        qDebug()<< "sended bytes" << sendedBytes;

        _tcpSocket->write(dataArray3);
        sendedBytes += dataArray3.length();
        qDebug()<< "sended bytes" << sendedBytes;

    }


    if(index < 50)
    {
        finished = false;
    }
    else
    {
        finished = true;
    }

}
/*
void MainWindow::TestSampleStruct()
{
    SampleDataStart sampleStart;
    sampleStart.dim1_size = 128;

    SampleDataData  sampleData;

    const double PI = 3.1415926;
    double ACQ = 2;
    double dw = ACQ / sampleStart.dim1_size;
    double T = 0.5; //
    double freq = 1 / T;//s
    double phi0 = 0 * 5 * PI / 180;

    sampleData.CreatDemoData(phi0, freq, dw, sampleStart.dim1_size);

    SampleDataEnd   sampleEnd(0);
    //测试结构1，
    QByteArray dataArray;
    sampleStart.Pack(dataArray);
    SampleDataStart start;
    start.Unpack(dataArray);

    //测试结构2，
    sampleData.Pack(dataArray);
    SampleDataData data;
    data.Unpack(dataArray);

    //测试结构3，
    sampleEnd.Pack(dataArray);
    SampleDataEnd end(1);
    end.Unpack(dataArray);

}
*/


void MainWindow::on_scanButton_clicked()
{

    if(_connected)
    {
        ui->scanButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
        StartTimer();
    }
    else
        QMessageBox::warning(this,"Warning", "Not connected", QMessageBox::Yes);//, QMessageBox::No);


}

void MainWindow::on_stopButton_clicked()
{

    this->killTimer(_timeId1);

    ui->editInfo->appendPlainText(QString("Stoped!"));
    _index1 = 0;//
    ui->scanButton->setEnabled(true);
    ui->stopButton->setEnabled(false);

    qDebug()<<"Scan stopped!";
}


void MainWindow::ScanFinished()
{

    this->killTimer(_timeId1);

    ui->editInfo->appendPlainText(QString("Finished!"));
    _index1 = 0;//
    ui->scanButton->setEnabled(true);
    ui->stopButton->setEnabled(false);

    qDebug()<<"Scan finished!";
}

void MainWindow::on_testButton_clicked()
{

    uint16_t temp = 0x3424;
    QByteArray tempArray = QByteArray::fromRawData((char*)(&temp), sizeof(uint16_t));

    _tcpSocket->write(tempArray);

    qDebug()<< "send another "<< tempArray.length()<<" bytes";
}
