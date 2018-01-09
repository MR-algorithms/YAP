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

    bool PrepareScantask(const ScanTask& task);
    bool Scan();
    void Stop();

private:


    // Thread function used to carry out the scan task.
    static unsigned int ThreadFunction(VirtualConsoleImpl * This);
    bool Init(const wchar_t *ip_address, unsigned short port);

public:
    ScanTask _scanTask;
    std::mutex _scanTask_mutex;
    std::shared_ptr<EventQueue> _evtQueue;
    std::mutex _event_mutex;
    bool _connected;

    EThreadState _threadState;
    std::thread _thread;

    std::timed_mutex _timeMutex1;
    int _timeIndex;

};

VirtualConsoleImpl::VirtualConsoleImpl():_connected(false), _evtQueue(new EventQueue)
{

}
VirtualConsoleImpl::~VirtualConsoleImpl()
{
    if(_connected)
    {
        std::unique_lock<std::mutex> Lock_event(_event_mutex);

        _evtQueue.get()->PushEvent(new MyEvent(MyEvent::stop));

        _thread.join();
    }
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

    //连接。
    Communicator::GetHandle().SetRemoteHost(scantask.ip_address.c_str(), scantask.port);
    bool succeed = Communicator::GetHandle().Connect();
    if(!succeed)
    {
       // return -1;
    }
    //
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


            bool finished = false;
            databin.Go(finished);
            if(finished)
            {
                databin.End();
                This->_timeMutex1.unlock();
                This->_timeIndex = 0;
                This->_threadState = idle;
                qDebug()<<"finish scan.";
                //通知主线程:更新状态，回收扫描线程。
                return 1;
            }
            else
            {
                //end of doing
                This->_timeIndex ++;
                qDebug() << "time: "<< This->_timeIndex;
            }
        }
            break;

        case MyEvent::stop:
        {
            databin.End();

            This->_timeMutex1.unlock();
            This->_timeIndex = 0;
            This->_threadState = idle;

            Communicator::GetHandle().Disconnect();
            return 1;

            qDebug()<<"stop Event";
        }
            break;

        default:
            break;

        }


    }

    return 1;
}



bool VirtualConsoleImpl::PrepareScantask(const ScanTask& scan_task)
{

    std::unique_lock<std::mutex> Lock_scantask(_scanTask_mutex);//mutex::lock:locks the mutex, blocks if the mutex is not available

    _scanTask = scan_task;
    return true;
}


bool VirtualConsoleImpl::Scan()
{




    _threadState = idle;
    _thread = std::thread(ThreadFunction , this);

    //如果连接不成功，可以稍微等待观察返回值吗？
    _timeMutex1.unlock();//
    //rand();
    std::unique_lock<std::mutex> Lock_event(_event_mutex);

    _evtQueue.get()->PushEvent(new MyEvent(MyEvent::scan));


    return true;
}

void VirtualConsoleImpl::Stop()
{
    std::unique_lock<std::mutex> Lock_event(_event_mutex);

    _evtQueue.get()->PushEvent(new MyEvent(MyEvent::stop));

    _thread.join();
    _connected = false;


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
