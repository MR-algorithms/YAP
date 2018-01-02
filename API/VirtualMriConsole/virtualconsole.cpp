#include "virtualconsole.h"
#include <cassert>
#include "communicator.h"
#include "mask.h"
#include "myevent.h"

VirtualConsole VirtualConsole::s_instance;

namespace _details{

enum EThreadState{ idle, scanning, paused, };

std::timed_mutex g_timeMutex1;
int g_times = 0;

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
    static unsigned int ThreadFunction(VirtualConsoleImpl * This);
    static void SendFunction(ScanTask& task);
    bool Init();

public:
    std::shared_ptr<ScanTask> _scanTask;
    std::mutex _scanTask_mutex;
    std::shared_ptr<EventQueue> _evtQueue;
    bool _initialized;

    std::thread _thread;
    EThreadState _threadState;

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


    _threadState = idle;

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


    assert(This->_threadState == idle);

    for(;;)
    {
        MyEvent *evt = This->_evtQueue->GetEvent(g_timeMutex1, This->_scanTask.get()->tr_millisecond);
        switch(evt->type())
        {
        case MyEvent::quit:
        {
            This->_threadState = idle;
            qDebug()<<"quit Event";

            return 1;
        }
            break;
        case MyEvent::scan:
        {
            //This->_scanTask_mutex.lock();
            This->_threadState = scanning;
            g_timeMutex1.lock();

            //SendFunction(*This->_scanTask.get());

            qDebug()<<"scan Event";

        }
            break;
        case MyEvent::stop:
        {
            g_timeMutex1.unlock();
            //This->_scanTask_mutex.unlock();
            g_times = 0;
            qDebug()<<"stop Event";
        }
            break;

        case MyEvent::time:
        {
            g_times ++;
            qDebug() << "time: "<< g_times;
        }
        default:
            break;

        }


    }

    return 1;
}


void VirtualConsoleImpl::SendFunction(ScanTask &task)
{
    //std::chrono::milliseconds dura(2000);
    //std::this_thread::sleep_for(dura);
    //std::unique_lock<std::mutex> Lock_scantask(This->_scanTask_mutex);
    //Do something.

    while (!g_timeMutex1.try_lock_for(std::chrono::milliseconds(1000))) {
      qDebug() << "time: "<< g_times;

      g_times ++;

    }
    int test = 3;

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

    std::unique_lock<std::mutex> Lock_scantask(_scanTask_mutex);//mutex::lock:locks the mutex, blocks if the mutex is not available

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

}

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
