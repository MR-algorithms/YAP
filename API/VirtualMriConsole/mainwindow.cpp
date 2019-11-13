#include <WinSock2.h>
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
#include <time.h>

void MainWindow::test_func()
{
    WSADATA wsa;
    ::WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET  s;
    SOCKADDR_IN serveraddr;
    int port = 10;

    s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int connect_return = ::connect(s, (SOCKADDR*)(&serveraddr), sizeof(serveraddr));

    int error_code = WSAGetLastError();
    assert( connect_return != SOCKET_ERROR);

    ::closesocket(s);
    WSACleanup();


}

//#include "databin.h"//test

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //test_func();
    ui->setupUi(this);
    ui->connectButton->setEnabled(false);
    ui->scanButton->setEnabled(true);
    ui->stopButton->setEnabled(false);

    QRegExp rx("^(1|[0]?(\\.\\d{1,2})?)$");
    QRegExpValidator *pReg = new QRegExpValidator(rx, ui->editMaskFile);

    ui->editMaskFile->setValidator(pReg);
    ui->editMaskFile->setText("0.05");

    //int test2 =99;
    //ui->editInfo->appendPlainText(QString("%1").arg(test2));

    srand((int)time(0));
    for(int x=0;x<10;x++)
    {
          qDebug()<< "rand() == "<< rand();
    }



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

    //reference_task.dataPath =L"D:\\Data\\cmr_data\\RawData_256\\RawData";
    reference_task.dataPath =ui->editDataFile->text().toStdWString();


    float rate = ui->editMaskFile->text().toFloat();
    Scan::Mask::MaskType type = static_cast<Scan::Mask::MaskType>( ui->maskComboBox->currentIndex() );

    //Hardcode: dataPath, channelCount, phaseCount,
    qDebug()<<"Hard code: dataPath, channelCount, phaseCount in Mainwindow::on_startButton_clicked";
    int allPhaseCount=ui->editPhaseCount->text().toInt();
    auto scantask = Scan::ScantaskGenerator::Create(reference_task, Scan::Mask(rate, type,allPhaseCount, 4/*ChannelCount*/));

    qDebug()<<"MainWidow2: onScanButton_clicked";

    VirtualConsoleNS::VirtualConsole::GetHandle().PrepareScantask(scantask);
    if(!VirtualConsoleNS::VirtualConsole::GetHandle().Scan())
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

    VirtualConsoleNS::VirtualConsole::GetHandle().Stop();
    //qDebug()<<"Scan stopped!";
}



bool MainWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::User+VirtualConsoleNS::scanning:
    {
        //int test2 =342;

        //ui->editInfo->appendPlainText(QString("%1").arg(test2));
        unsigned int scan_count=VirtualConsoleNS::VirtualConsole::GetHandle().GetSendIndex();
        QString message=tr("scanning:%1").arg(scan_count);
        ui->editInfo->appendPlainText(message);

    }
        break;

    case QEvent::User + VirtualConsoleNS::finished:
    {
        //ui->editInfo->appendPlainText(QString("stopped"));

        ui->editInfo->appendPlainText(QString("Finished!"));
        //
        unsigned int scan_count=VirtualConsoleNS::VirtualConsole::GetHandle().GetSendIndex();

        QString message=tr("<scanning:%1>").arg(scan_count);
        ui->editInfo->appendPlainText(message);
        //
        ui->scanButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        VirtualConsoleNS::VirtualConsole::GetHandle().Stop();



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
    QString file_name = QFileDialog::getExistingDirectory(this,"Select the folder path","D:\\Data\\cmr_data");
    ui->editDataFile->setText(file_name);
}
