#include "virtualconsole.h"
#include <cassert>
#include "communicator.h"
#include "mask.h"
#include "myevent.h"

VirtualConsole VirtualConsole::s_instance;

namespace _details{

enum EThreadState{ idle, scanning, paused, };

class VirtualConsoleImpl
{
public:
    VirtualConsoleImpl();
    ~VirtualConsoleImpl();
    bool SetReconHost(const wchar_t *ip_address, unsigned short port);

    bool PrepareScantask(ScanTask& task);

    bool Connect();
    bool Scan();
    void Stop();
    void Quit();

private:


    // Thread function used to carry out the scan task.
    static unsigned int ThreadFunction(const std::string& name , EventQueue *queue);
    bool Init();

    std::shared_ptr<ScanTask> _scanTask;
    std::mutex _scanTask_mutex;
    EventQueue _evtQueue;
    bool _initialized;

};
VirtualConsoleImpl::VirtualConsoleImpl():_initialized(false), _scanTask(new ScanTask)
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

    std::thread thread1(ThreadFunction , "thread1" , &_evtQueue);//join or detach?

    _initialized = true;

    return _initialized;
}



/**
    \todo Add state checking.
    \bug Hardcoding file name.
*/
unsigned int VirtualConsoleImpl::ThreadFunction(const std::string& name , EventQueue *queue)
{

    for(;;)
    {
        MyEvent *evt = queue->GetEvent();
        switch(evt->type())
        {
        case MyEvent::quit:
            qDebug()<<"quit Event";
            return 1;
            break;
        case MyEvent::scan:
            qDebug()<<"scan Event";
            break;
        case MyEvent::stop:
            qDebug()<<"stop Event";
            break;

        default:
            break;

        }
        //检查定时器。

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


bool VirtualConsoleImpl::PrepareScantask(ScanTask& scan_task)
{
    //assert(scan_task);

    std::lock_guard<std::mutex> Lock_scantask(_scanTask_mutex);//mutex::lock:locks the mutex, blocks if the mutex is not available

    _scanTask = std::shared_ptr<ScanTask>(&scan_task);
    return true;
}


bool VirtualConsoleImpl::Scan()
{
    _evtQueue.PushEvent(new MyEvent(MyEvent::scan));


    return true;
}

void VirtualConsoleImpl::Stop()
{
    _evtQueue.PushEvent(new MyEvent(MyEvent::stop));

}
void VirtualConsoleImpl::Quit()
{
    _evtQueue.PushEvent(new MyEvent(MyEvent::quit));
    //join the thread according to thread id.

}

};

// ===========================

VirtualConsole::VirtualConsole()
{
    _impl = std::make_shared<_details::VirtualConsoleImpl>();
}

bool VirtualConsole::PrepareScantask(ScanTask &task)
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
void VirtualConsole::Quit()
{
    assert(_impl);
    _impl->Quit();
}
