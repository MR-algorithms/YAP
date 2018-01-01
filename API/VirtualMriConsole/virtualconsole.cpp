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
    //static member function. 静态成员函数怎么访问
    static unsigned int ThreadFunction(VirtualConsoleImpl * This);
    bool Init();

public:
    std::shared_ptr<ScanTask> _scanTask;
    std::mutex _scanTask_mutex;
    std::shared_ptr<EventQueue> _evtQueue;
    bool _initialized;

    std::thread _thread;

};
VirtualConsoleImpl::VirtualConsoleImpl():_initialized(false), _scanTask(new ScanTask),_evtQueue(new EventQueue)
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


    _thread = std::thread(ThreadFunction , this);

    _initialized = true;

    return _initialized;
}



/**
    \todo Add state checking.
    \bug Hardcoding file name.
*/
unsigned int VirtualConsoleImpl::ThreadFunction(VirtualConsoleImpl *This)
{


    for(;;)
    {
        MyEvent *evt = This->_evtQueue->GetEvent();
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


        std::chrono::milliseconds dura(30000);
        std::this_thread::sleep_for(dura);
        std::lock_guard<std::mutex> Lock_scantask(This->_scanTask_mutex);
        //Do something.

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
    _evtQueue.get()->PushEvent(new MyEvent(MyEvent::scan));


    return true;
}

void VirtualConsoleImpl::Stop()
{
    _evtQueue.get()->PushEvent(new MyEvent(MyEvent::stop));

}
void VirtualConsoleImpl::Quit()
{
    _evtQueue.get()->PushEvent(new MyEvent(MyEvent::quit));
    _thread.join();

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
