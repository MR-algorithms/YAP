#include "virtualconsole.h"
#include <cassert>
#include "communicator.h"
#include "mask.h"
#include "scantask.h"
#include "myevent.h"
#include "databin.h"

VirtualConsole VirtualConsole::s_instance;
using namespace Scan;


namespace _details{

enum EThreadState{ idle, scanning, paused, };



class VirtualConsoleImpl
{
public:
    VirtualConsoleImpl();
    ~VirtualConsoleImpl();
    bool SetReconHost(const wchar_t *ip_address, unsigned short port);

    bool PrepareScantask(const ScanTask& task);

    bool Connect();
    void Disconnect();
    bool Scan();
    void Stop();

private:


    // Thread function used to carry out the scan task.
    static unsigned int ThreadFunction(VirtualConsoleImpl * This);
    bool Init();

public:
    ScanTask _scanTask;
    std::mutex _scanTask_mutex;
    std::shared_ptr<EventQueue> _evtQueue;
    bool _initialized;

    std::thread _thread;
    EThreadState _threadState;

    std::timed_mutex _timeMutex1;
    int _timeIndex;

};

VirtualConsoleImpl::VirtualConsoleImpl():_initialized(false), _evtQueue(new EventQueue)
{
    Init();
}
VirtualConsoleImpl::~VirtualConsoleImpl()
{
    // Communicator::GetHandle().disconnect();
}

bool VirtualConsoleImpl::Init()
{
    if (_initialized)
        return true;


    _threadState = idle;

    _initialized = true;

    return _initialized;
}



/**
    \todo Add state checking.
    \bug Hardcoding file name.
*/
unsigned int VirtualConsoleImpl::ThreadFunction(VirtualConsoleImpl *This)
{

    std::unique_lock<std::mutex> Lock_scantask(This->_scanTask_mutex);//mutex::lock:locks the mutex, blocks if the mutex is not available
    ScanTask scantask = This->_scanTask;
    Lock_scantask.unlock();

    Databin databin;
    databin.Load(scantask.dataPath, scantask.mask.channelCount);

    assert(This->_threadState == idle);

    for(;;)
    {
        MyEvent *evt = This->_evtQueue->GetEvent(This->_timeMutex1, scantask.trMs);
        switch(evt->type())
        {
        case MyEvent::scan:
        {
            This->_threadState = scanning;
            This->_timeMutex1.lock();
            This->_timeIndex = 0;
            int scan_id = 180106;

            databin.Start(scan_id, scantask.mask.channelCount);
            qDebug()<<"scan Event";

        }
            break;

        case MyEvent::time:
        {
            //Doing something.


            databin.Go();
            //end of doing
            This->_timeIndex ++;
            qDebug() << "time: "<< This->_timeIndex;
        }
            break;

        case MyEvent::stop:
        {
            databin.End();

            This->_timeMutex1.unlock();
            This->_timeIndex = 0;
            This->_threadState = idle;

            qDebug()<<"stop Event";
            return 1;
        }

        default:
            break;

        }


    }

    return 1;
}


bool VirtualConsoleImpl::SetReconHost(const wchar_t *ip_address, unsigned short port)
{

    Communicator::GetHandle().SetRemoteHost(ip_address, port);


    return true;
}
bool VirtualConsoleImpl::Connect()
{
    Communicator::GetHandle().Connect();
    return true;
}

void VirtualConsoleImpl::Disconnect()
{
    Communicator::GetHandle().Disconnect();

}


bool VirtualConsoleImpl::PrepareScantask(const ScanTask& scan_task)
{

    std::unique_lock<std::mutex> Lock_scantask(_scanTask_mutex);//mutex::lock:locks the mutex, blocks if the mutex is not available

    _scanTask = scan_task;
    return true;
}


bool VirtualConsoleImpl::Scan()
{

    //_timeMutex1.lock();//
    _thread = std::thread(ThreadFunction , this);

    _evtQueue.get()->PushEvent(new MyEvent(MyEvent::scan));


    return true;
}

void VirtualConsoleImpl::Stop()
{
    _evtQueue.get()->PushEvent(new MyEvent(MyEvent::stop));
    _thread.join();

}

}

// ===========================

VirtualConsole::VirtualConsole()
{
    _impl = std::make_shared<_details::VirtualConsoleImpl>();
}

bool VirtualConsole::PrepareScantask(const ScanTask& task)
{
    assert(_impl);

    return _impl->PrepareScantask(task);
}


bool VirtualConsole::SetReconHost(const wchar_t* ip_address, unsigned short port)
{
    assert(_impl);

    return _impl->SetReconHost(ip_address, port);
}
bool VirtualConsole::Connect()
{
    assert(_impl);
    return _impl->Connect();

}

void VirtualConsole::Disconnect()
{
    assert(_impl);
    _impl->Disconnect();

}

bool VirtualConsole::Scan()
{
    assert(_impl);

    return _impl->Scan();
}

void VirtualConsole::Stop()
{
    assert(_impl);
    _impl->Stop();
}

