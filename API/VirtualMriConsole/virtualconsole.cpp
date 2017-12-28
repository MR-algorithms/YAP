#include "virtualconsole.h"
#include <cassert>
#include "communicator.h"
#include "mask.h"

VirtualConsole VirtualConsole::s_instance;

namespace _details{

enum EThreadState{ idle, scanning, paused, };

class VirtualConsoleImpl
{
public:
    VirtualConsoleImpl();
    ~VirtualConsoleImpl();
    bool SetReconHost(wchar_t *ip_address, unsigned short port);

    bool PrepareScan(ScanTask& task);

    bool Start();
    void Stop();

private:
    int _tr_millisecond;

    std::shared_ptr<ScanTask> _scanTask;
/*
    handle m_hEventScan;        //!< Event used to start scan.
    HANDLE m_hEventStop;        //!< Event used to stop scan.
    HANDLE m_hEventQuitThread;  //!< Event used to quit the scan thread.
    HANDLE m_hTimer;          	/// Timer used to trigger scan status checking.
    HANDLE m_mutexExpTask;		//!< Mutex used to protect m_pExpTask.
*/
    // Thread function used to carry out the scan task.
    static unsigned int ThreadFunction();
    static bool TimerEvent();
    bool Init();
    bool _initialized;

    bool SetMask(Mask& mask);
    bool SetData();
    bool SetTr(int tr_millisecond);




};
VirtualConsoleImpl::VirtualConsoleImpl():_initialized(false), _scanTask(new ScanTask)
{

}

bool VirtualConsoleImpl::Init()
{
    if (_initialized)
        return true;
/*    m_hEventScan		= ::CreateEvent(NULL, true, 0, NULL);
    m_hEventStop		= ::CreateEvent(NULL, true, 0, NULL);
    m_hEventQuitThread	= ::CreateEvent(NULL, true, 0, NULL);
    m_hEventSpecInited  = ::CreateEvent(NULL, true, 0, NULL);
\
    m_mutexExpTask		= ::CreateMutex(NULL, FALSE, NULL);
    m_hTimer			= ::CreateWaitableTimer(NULL, false, NULL);

    assert(m_hEventScan);
    assert(m_hEventStop);
    assert(m_hEventQuitThread);
    assert(m_hEventSpecInited);

    assert(m_hTimer);
    assert(m_mutexExpTask);


    AfxBeginThread(ThreadFunction, (LPVOID)this);

    // 等待谱仪初始化
    DWORD dwResult = WaitForSingleObject(m_hEventSpecInited, 10000);
    ASSERT(dwResult != WAIT_ABANDONED_0);

    if (dwResult == WAIT_TIMEOUT)
    {
        _initialized = false;
    }
    else if (dwResult == WAIT_OBJECT_0)
    {
        _initialized = true;
        ::ResetEvent(m_hEventSpecInited);
    }
*/
    return _initialized;
}



/**
    \todo Add state checking.
    \bug Hardcoding file name.
*/
unsigned int VirtualConsoleImpl::ThreadFunction()
{
   /*
    VirtualConsoleImpl* This = reinterpret_cast<VirtualConsoleImpl*>(pVoid);


    ::SetEvent(This->m_hEventSpecInited);

    EThreadState state = idle;

    HANDLE handles[4];
    handles[0] = This->m_hEventScan;
    handles[1] = This->m_hEventStop;
    handles[2] = This->m_hEventQuitThread;
    handles[3] = This->m_hTimer;

    CScanTask scan_task;

    LARGE_INTEGER li;
    li.QuadPart = -10000000;	// 1 sec from now, time in 100ns.

    for(;;)
    {
        DWORD dwResult = ::WaitForMultipleObjects( 4, handles, FALSE, INFINITE);
        ASSERT(dwResult != WAIT_FAILED);
        switch (dwResult - WAIT_OBJECT_0)
        {
        case 0:	// scan
            {
                if (state == idle)
                {
                    state = scanning;
                    ::SetWaitableTimer(This->m_hTimer, &li, scan_task.m_nQueryInterval, NULL, NULL, TRUE);
                }

            }
            break;

        case 1:	// stop
            {
                assert(state == scanning ||state == idle);
                ::ResetEvent(This->m_hEventStop);
                if(state = scanning)
                {
                    //..
                    ::CancelWaitableTimer(This->m_hTimer);
                    //eg. send the end struct of sample data.

                }
            }
            break;

        case 2:	// quit
            {
                ::ResetEvent(This->m_hEventQuitThread);
                assert(state == idle);
                return 1;
            }
            break;
        case 3:	// timer
            {
                ::CancelWaitableTimer(This->m_hTimer);


            break;


        default:
            ASSERT(0);
        }
    }
    */
    return 1;
}

bool VirtualConsoleImpl::TimerEvent()
{

    static int i = 0;
    i ++;

    return true;
    //
    /*
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
    */

}

/*
void MainWindow::SendTestSample(int index, bool &finished)
{
    //on_testButton_clicked();
    //return;

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

    finished = (index >= 50);
}

 */
VirtualConsoleImpl::~VirtualConsoleImpl()
{
  /*  CloseHandle(m_hEventScan);
    CloseHandle(m_hEventStop);
    CloseHandle(m_hEventQuitThread);
    CloseHandle(m_hTimer);
    CloseHandle(m_mutexExpTask);
*/
    Communicator::GetHandle().disconnect();
}


bool VirtualConsoleImpl::SetTr(int tr_millisecond)
{
    _tr_millisecond = tr_millisecond;
    return true;

}


bool VirtualConsoleImpl::SetMask(Mask& mask)
{
    assert(0);
    return false;
}

bool VirtualConsoleImpl::SetData()
{
    assert(0);
    return false;
}


bool VirtualConsoleImpl::SetReconHost(wchar_t *ip_address, unsigned short port)
{
    assert(0);

    Communicator::GetHandle().SetRemoteHost(ip_address, port);

    Communicator::GetHandle().Connect();

    return false;
}

bool VirtualConsoleImpl::PrepareScan(ScanTask& scan_task)
{
    //assert(scan_task);
    bool bSucceed = false;
/*
    DWORD result = ::WaitForSingleObject(m_mutexExpTask, 2000);

    assert(result != WAIT_ABANDONED_0);
    if (result == WAIT_TIMEOUT)
    {
        bSucceed = false;
    }
    else if (result == WAIT_OBJECT_0)
    {
        _scan_task = scan_task;
    }

    ::ReleaseMutex(m_mutexExpTask);
*/
    return bSucceed;
}


bool VirtualConsoleImpl::Start()
{
    assert(0);

    return false;
}

void VirtualConsoleImpl::Stop()
{
    assert(0);
}

};

// ===========================

VirtualConsole::VirtualConsole()
{
    _impl = std::make_shared<_details::VirtualConsoleImpl>();
}

bool VirtualConsole::PrepareScan(ScanTask &task)
{
    assert(_impl);

    return _impl->PrepareScan(task);
}


bool VirtualConsole::SetReconHost(wchar_t* ip_address, unsigned short port)
{
    assert(_impl);

    return _impl->SetReconHost(ip_address, port);
}

bool VirtualConsole::Start()
{
    assert(_impl);

    return _impl->Start();
}

void VirtualConsole::Stop()
{
    assert(_impl);
    _impl->Stop();
}
