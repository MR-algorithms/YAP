#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <direct.h>


#include "API/Yap/ModuleManager.h"

#include "ProcessorlineManager.h"
#include "Processors/Display1D.h"
#include "Processors/Display2D.h"
#include "Processors/ReceiveData.h"

//#include "Messagepack/reconserver.h"
#include "imagelayoutwidget.h"
#include "Interface/Interfaces.h"
#include "Implement/ProcessorImpl.h"
#include "Client/DataHelper.h"
#include <QFileDialog>

#include "Messagepack/SampleDataProtocol.h"

#include <complex>
#include "QtUtilities/FormatString.h"
#include "Reactor/Reactor.h"
#include "Reactor/ListenHandler.h"
#include "Reactor/SampleDataServer.h"
#include "QDataEvent.h"
#include "Processors/Rawdata.h"

//#include <QThread>

const QEvent::Type SAMPLE_DATA_EVENT = (QEvent::Type)(QEvent::User + 101);

class DataObserver : public IDataObserver
{
public:
    DataObserver(MainWindow* console)
        : _console(console){}
    virtual void OnData(ScanSignal scan_signal, uint32_t scan_id, int channel_index, int phase_index) override
    {
        switch(scan_signal)
        {
        case SSScanStart:
            QApplication::postEvent(_console, new QDataEvent(SAMPLE_DATA_EVENT, SSScanStart,
                                                             scan_id, channel_index, phase_index));
            break;

        case SSChannelPhaseStep:
            QApplication::postEvent(_console, new QDataEvent(SAMPLE_DATA_EVENT, SSChannelPhaseStep,
                                                             scan_id, channel_index, phase_index));
            break;

        case SSScanFinished:
            QApplication::postEvent(_console, new QDataEvent(SAMPLE_DATA_EVENT, SSScanFinished,
                                                             scan_id, channel_index, phase_index));
            break;

        default:
            break;

        }
    }

private:
    MainWindow* _console;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{



    ui->setupUi(this);
    //导入需要的处理器
    SetImportedProcessors();

    ProcessorlineManager::GetHandle().Demo1D();

    ui->buttonFinish->setEnabled(false);

    on_buttonStart_clicked();


}

MainWindow::~MainWindow()
{
    //Yap::ImportedProcessorManager.Reset();
    if(_reactor_thread->joinable())
    {
        _reactor_thread->join();
    }
    delete ui;
}

void MainWindow::SetImportedProcessors()
{
    Yap::ImportedProcessorManager::RegisterProcessor(L"ReceiveData", new ReceiveData());
    Yap::ImportedProcessorManager::RegisterProcessor(L"Display1D", new Display1D(*ui->plot1d_widget));
    Yap::ImportedProcessorManager::RegisterProcessor(L"Display2D", new Display2D(*ui->imagelayout_widget));


    Databin::RawData::GetHandle().SetObserver(std::shared_ptr<IDataObserver>(new DataObserver(this)));

}

void MainWindow::on_actionOpen_triggered()
{
    int count_max = sizeof(unsigned int) * 8;

    enum OpenType{
        otDemo2D = 1,
        otPipeline,
        otNiuImage
    };
    OpenType opentype = otNiuImage;
    switch(opentype)
    {
    case otDemo2D:
        ProcessorlineManager::GetHandle().Demo2D();
        break;
    case otPipeline: {
        QString pipeline = QString("config//pipelines//test0_qt.pipeline");
        //The 1st processer in the pipeline should read data itself.
        ProcessorlineManager::GetHandle().RunPipeline(pipeline);
    }
        break;
    case otNiuImage: {
        char   buffer[MAX_PATH];
        getcwd(buffer, MAX_PATH);

        QString start_folder(buffer);
        start_folder +="\\Data";
        auto file_path = QFileDialog::getOpenFileName(this, tr("Load Data"), start_folder,
                  "Raw Data (*.fid);;NiuMriImage Files (*.niuimg);;Image Files (*.img);;All Files (*.*)", nullptr);
        if (!file_path.isEmpty()) {
            ProcessorlineManager::GetHandle().LoadData(file_path);
        }
    }
        break;
    default:
        break;
    }


}

void MainWindow::on_buttonStart_clicked()
{
    //
    /*
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
    */
    ui->buttonStart->setText("Waiting");
    ui->buttonStart->setEnabled(false);
    ui->buttonFinish->setEnabled(true);

    //Start Reactor in main thread or a single thread.:
    //DebugInfo::Output(L"Mainwindow::OnButtenStart()", L"Start Threadfunc_reactor",
    //                  reinterpret_cast<int>(this), true, DebugInfo::flow_type1);
    //Threadfunc_reactor();
    //return;

    DebugInfo::Output(L"Mainwindow::OnButtenStart()", L"bind Threadfunc_reactor",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type1);
    if(!_reactor_thread)
        _reactor_thread = std::shared_ptr<std::thread>(
                    new std::thread(std::bind(&MainWindow::Threadfunc_reactor, this)));

}


void MainWindow::Threadfunc_reactor()
{
    static int threadfunc_reactor_index = -1;
    threadfunc_reactor_index ++;
    assert(threadfunc_reactor_index ==0);//only once.

    QString port = ui->editListeningPort->text();

    //---
    std::wstring threadinfo(DebugInfo::get_threadinfo());
    DebugInfo::Output(L"Mainwindow::Threadfun_reactor()", L"Enter ...",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type1);

    EventHandler *handler = new ListenHandler(port.toInt());

    Reactor::get_instance().regist(handler);
    while (true)//check any event and dispatch();
    {
        Reactor::get_instance().dispatch();
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


void MainWindow::customEvent(QEvent *event)
{
    switch(event->type()){
    case SAMPLE_DATA_EVENT:
        OnSampleDataEvent(event);
        break;

    default:
        break;
    }
}

void MainWindow::OnSampleDataEvent(QEvent *event)
{
    if(QDataEvent* data_event = dynamic_cast<QDataEvent*>(event))
    {
        switch(data_event->GetScanSignal())
        {
        case SSScanStart:
        {
            uint32_t scan_id = data_event->GetScanID();
            ProcessorlineManager::GetHandle().OnScanStart(scan_id);
            break;
        }
        case SSChannelPhaseStep:
        {
            uint32_t scan_id = data_event->GetScanID();
            ProcessorlineManager::GetHandle().OnChannelPhasestep(scan_id);
            break;
        }
        case SSScanFinished:
        {
            uint32_t scan_id = data_event->GetScanID();
            ProcessorlineManager::GetHandle().OnScanFinished(scan_id);
            break;
        }
        default:
            break;
        }
    }
}

