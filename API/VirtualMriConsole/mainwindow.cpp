#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QByteArray>
#include "SampleDataProtocol.h"
#include <QDebug>
#include <vector>
#include "Windows.h"
#include "databin.h"
#include "virtualconsole.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _connected(false)
{


    ui->setupUi(this);

    Reset();


}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_connectButton_clicked()
{

    if (!_connected)
    {
        QString ip_address = ui->editReconHost->text();
        std::wstring temp = ip_address.toStdWString();

        VirtualConsole::GetHandle().SetReconHost(temp.c_str(), ui->editReconPort->text().toInt());
        VirtualConsole::GetHandle().Connect();
        //--

        _connected = true;

        ui->connectButton->setText("Disconnect");
        ui->scanButton->setEnabled(true);

    }
    else
    {
        //VirtualConsole::GetHandle().Disconnect();
        _connected = false;
        Reset();
    }


}

void MainWindow::Reset()
{
    ui->connectButton->setText("Connect");
    ui->scanButton->setEnabled(false);
    ui->stopButton->setEnabled(false);

    _connected = false;

}



void MainWindow::on_scanButton_clicked()
{

    if(_connected)
    {
        ui->scanButton->setEnabled(false);
        ui->stopButton->setEnabled(true);

        int TRms = ui->editTR->text().toInt();

        //
        ScanTask* scantask = new ScanTask;
        scantask->tr_millisecond = TRms;

        qDebug()<<"MainWidow: onScanButton_clicked";
        qDebug()<<L"on_testButton_clicked !";

        VirtualConsole::GetHandle().PrepareScantask(scantask);
        VirtualConsole::GetHandle().Scan();

    }
    else
        QMessageBox::warning(this,"Warning", "Not connected", QMessageBox::Yes);//, QMessageBox::No);


}

void MainWindow::on_stopButton_clicked()
{

    ui->editInfo->appendPlainText(QString("Stoped!"));
    ui->scanButton->setEnabled(true);
    ui->stopButton->setEnabled(false);

    VirtualConsole::GetHandle().Stop();
    //qDebug()<<"Scan stopped!";
}

void MainWindow::on_testButton_clicked()
{

    qDebug()<<"on_testButton_clicked !";

    //Databin databin;
    //databin.Load(L"D:\\test_data\\RawData_256");

    /*

    uint16_t temp = 0x3424;
    QByteArray tempArray = QByteArray::fromRawData((char*)(&temp), sizeof(uint16_t)*5);

    for(int i = 0; i < 10000000; i ++)
    {

        _tcpSocket->write(tempArray);
        QApplication::processEvents();
        //Sleep(1000);

    }

    qDebug()<< "send another "<< tempArray.length()<<" bytes";
    */
}
