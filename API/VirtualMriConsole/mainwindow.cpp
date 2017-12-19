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

    //TestSampleStruct();

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

        VirtualConsole::GetHandle().SetReconHost(ip_address, ui->editReconPort->text().toInt());
        VirtualConsole::GetHandle().Start();
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
    _index1 = 0;
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
        std::shared_ptr<ScanTask> scantask(new ScanTask);
        scantask.get()->tr_millisecond = TRms;

        VirtualConsole::GetHandle().PrepareScan(*scantask.get());
        VirtualConsole::GetHandle().Start();

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
    Databin databin;
    databin.Load(L"D:\\test_data\\RawData_256");

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
