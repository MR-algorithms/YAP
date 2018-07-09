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
#include <QFileDialog>

//#include "databin.h"//test

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->connectButton->setEnabled(false);
    ui->scanButton->setEnabled(true);
    ui->stopButton->setEnabled(false);

    QRegExp rx("^(1|[0]?(\\.\\d{1,2})?)$");
    QRegExpValidator *pReg = new QRegExpValidator(rx, ui->editMaskFile);

    ui->editMaskFile->setValidator(pReg);

    //int test2 =99;
    //ui->editInfo->appendPlainText(QString("%1").arg(test2));

}

MainWindow::~MainWindow()
{

    delete ui;
}



void MainWindow::on_connectButton_clicked()
{

    //    QMessageBox::warning(this,"Warning", "Not connected", QMessageBox::Yes);

}



void MainWindow::on_scanButton_clicked()
{

    //QEvent evt(QEvent::Type(QEvent::User + 1));
    //QApplication::sendEvent(this, &evt);
    //return;


    //参考scantask赋值

    Scan::ScanTask reference_task;
    reference_task.pWnd = this;
    reference_task.trMs = ui->editTR->text().toInt();

    QString ip_address = ui->editReconHost->text();
    reference_task.ip_address = ip_address.toStdWString();
    reference_task.port = ui->editReconPort->text().toInt();

    //reference_task.dataPath =L"D:\\test_data\\RawData_256\\RawData";
    reference_task.dataPath =ui->editDataFile->text().toStdWString();

    //参考mask赋值。
    float rate = ui->editMaskFile->text().toFloat();
    Scan::Mask::MaskType type = static_cast<Scan::Mask::MaskType>( ui->maskComboBox->currentIndex() );

    //Hardcode: dataPath, channelCount, phaseCount,
    qDebug()<<"Hard code: dataPath, channelCount, phaseCount";
    int allPhaseCount=ui->editPhaseCount->text().toInt();
    auto scantask = Scan::ScantaskGenerator::Create(reference_task, Scan::Mask(rate, type,allPhaseCount, 4));

    qDebug()<<"MainWidow: onScanButton_clicked";

    VirtualConsole::GetHandle().PrepareScantask(scantask);
    if(!VirtualConsole::GetHandle().Scan())
    {
      QMessageBox::warning(this,"Warning", "Not connected", QMessageBox::Yes);//, QMessageBox::No);
    }
    else
    {
        ui->scanButton->setEnabled(false);
        ui->stopButton->setEnabled(true);

    }


}

void MainWindow::on_stopButton_clicked()
{

    ui->editInfo->appendPlainText(QString("Stoped!"));
    ui->scanButton->setEnabled(true);
    ui->stopButton->setEnabled(false);

    VirtualConsole::GetHandle().Stop();
    //qDebug()<<"Scan stopped!";
}


//父线程内通过改写虚函数event，来使得能够捕获消息，这样，两个线程就能够同时运行并能够进行线程间通信了。
bool MainWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::User+scanning:
    {
        //int test2 =342;

        //ui->editInfo->appendPlainText(QString("%1").arg(test2));
        _scan_count=VirtualConsole::GetHandle().GetSendIndex();
        QString message=tr("scanning:%1").arg(_scan_count);
        ui->editInfo->appendPlainText(message);

    }
        break;

    case QEvent::User + finished:
    {
        //ui->editInfo->appendPlainText(QString("stopped"));

        ui->editInfo->appendPlainText(QString("Finished!"));
        ui->scanButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        VirtualConsole::GetHandle().Stop();



    }
        break;

    default:
        break;
    }
    return QMainWindow::event(event);
}

//void MainWindow::on_testButton_clicked()
//{

    //qDebug()<<"on_testButton_clicked !";


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
//}

//选择原始数据.fid文件路径

void MainWindow::on_browsedatabutton_clicked()
{
    QString file_name = QFileDialog::getExistingDirectory(this,"Select the folder path","D:\\test_data");
    ui->editDataFile->setText(file_name);
}
