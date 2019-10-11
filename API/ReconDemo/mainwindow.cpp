#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <direct.h>


#include "API/Yap/ModuleManager.h"

#include "datamanager.h"
#include "Processors/Display1D.h"
#include "Processors/Display2D.h"
#include "Processors/ReceiveData.h"

#include "DataSample/reconserver.h"
#include "imagelayoutwidget.h"
#include "Interface/Interfaces.h"
#include "Implement/ProcessorImpl.h"
#include "Client/DataHelper.h"
#include "datamanager.h"
#include <QFileDialog>

#include "DataSample/SampleDataProtocol.h"

#include <complex>
#include<QThread>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{



    ui->setupUi(this);
    //导入需要的处理器
    SetImportedProcessors();

    DataManager::GetHandle().Demo1D();

    ui->buttonFinish->setEnabled(false);

    on_buttonStart_clicked();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetImportedProcessors()
{
    Yap::ImportedProcessorManager::RegisterProcessor(L"ReceiveData", new ReceiveData());
    Yap::ImportedProcessorManager::RegisterProcessor(L"Display1D", new Display1D(*ui->plot1d_widget));
    Yap::ImportedProcessorManager::RegisterProcessor(L"Display2D", new Display2D(*ui->imagelayout_widget));


}

void MainWindow::on_actionOpen_triggered()
{
    enum OpenType{
        otDemo = 1,
        otPipeline,
        otNiuImage
    };
    OpenType opentype = otPipeline;
    switch(opentype)
    {
    case otDemo:
        //流水线的特点：数据驱动，数据不会停止，如果中间有停止机制（停在内存里），将造成拥堵。
        //流水线末端：显示，或者存储等数据堆积则是可以理解的。
        DataManager::GetHandle().Demo2D();
        break;
    case otPipeline:
        DataManager::GetHandle().LoadPipeline();
        break;
    case otNiuImage:
        {

            char   buffer[MAX_PATH];
            getcwd(buffer, MAX_PATH);

            QString start_folder(buffer);
            start_folder +="\\Data";



            auto file_path = QFileDialog::getOpenFileName(this, tr("Load File"), start_folder,
                      "Raw Data (*.fid);;NiuMriImage Files (*.niuimg);;Image Files (*.img);;All Files (*.*)", nullptr);
            if (!file_path.isEmpty())
            {

                //test

                DataManager::GetHandle().Load(file_path);

            }
        }
        break;
    default:
        break;
    }





}

void MainWindow::on_buttonStart_clicked()
{
    if (!reconServer.get())
    {
        QString port = ui->editListeningPort->text();
        reconServer = std::shared_ptr<ReconServer>(new ReconServer(this, port.toInt()));
        connect(reconServer.get(), &ReconServer::signalDataReceived, this, &MainWindow::slotDataReceived);
        connect(reconServer.get(), &ReconServer::signalDisconnected,this,&MainWindow::slotDisconnected);
        ui->buttonStart->setText("Waiting");
        ui->buttonStart->setEnabled(false);
        ui->buttonFinish->setEnabled(true);
   }
}



void MainWindow::slotDataReceived(int length)
{
    if(!ui->buttonFinish->isEnabled())
        ui->buttonFinish->setEnabled(true);
    QString len=QString::number(length);
    ui->editInfo->appendPlainText(len);

    //

    //assert(0);
    return;

}

void MainWindow::slotDisconnected(int socketDescriptor)//主线程接收数据完毕
{

    //ui->buttonFinish->setText("Finished");
    ui->buttonFinish->setEnabled(false);

    ui->editInfo->appendPlainText(tr("FinishReceiveDta"));
    QString len=QString::number(socketDescriptor);
    ui->editInfo->appendPlainText(len);

    return;
}



void MainWindow::on_buttonBrowsePipeline_clicked()
{
    char   buffer[MAX_PATH];
    getcwd(buffer, MAX_PATH);
    QString start_folder(buffer);
    start_folder +="/config/pipelines";
    QString pipeline_path = QFileDialog::getOpenFileName(this, tr("Get pipeline path"), start_folder,
              "Pipeline Path (*.pipeline);;All Files (*.*)", nullptr);
    ui->editPipeline->setText(pipeline_path);
}

void MainWindow::on_buttonFinish_clicked()
{

    //reconServer.get()->slotDisconnected(-2);//主线程断开连接，结束数据的接收


}
