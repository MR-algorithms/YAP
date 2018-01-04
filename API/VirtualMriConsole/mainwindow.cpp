#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QByteArray>
#include "SampleDataProtocol.h"
#include <QDebug>
#include <vector>
#include "Windows.h"
#include "virtualconsole.h"
#include "scantask.h"
#include "mask.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->connectButton->setEnabled(true);
    ui->scanButton->setEnabled(false);
    ui->stopButton->setEnabled(false);

    QRegExp rx("^(1|[0]?(\\.\\d{1,2})?)$");
    QRegExpValidator *pReg = new QRegExpValidator(rx, ui->editMaskFile);

    ui->editMaskFile->setValidator(pReg);

}

MainWindow::~MainWindow()
{
    VirtualConsole::GetHandle().Disconnect();

    delete ui;
}



void MainWindow::on_connectButton_clicked()
{

    QString ip_address = ui->editReconHost->text();
    std::wstring temp = ip_address.toStdWString();

    VirtualConsole::GetHandle().SetReconHost(temp.c_str(), ui->editReconPort->text().toInt());
    VirtualConsole::GetHandle().Connect();

    //ui->connectButton->setText("Disconnect");
    ui->connectButton->setEnabled(false);
    ui->scanButton->setEnabled(true);




}



void MainWindow::on_scanButton_clicked()
{

    ui->scanButton->setEnabled(false);
    ui->stopButton->setEnabled(true);

    int trMs = ui->editTR->text().toInt();
    float rate = ui->editMaskFile->text().toFloat();
    Scan::Mask::MaskType type = static_cast<Scan::Mask::MaskType>( ui->maskComboBox->currentIndex() );

    //
    auto scantask = Scan::ScantaskGenerator::Create(trMs, Scan::Mask(rate, type),L"D:\\test_data\\RawData_256\\RawData", 4);//因为样本数据是四个通道。

    qDebug()<<"MainWidow: onScanButton_clicked";

    VirtualConsole::GetHandle().PrepareScantask(scantask);
    VirtualConsole::GetHandle().Scan();

    //QMessageBox::warning(this,"Warning", "Not connected", QMessageBox::Yes);//, QMessageBox::No);


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
