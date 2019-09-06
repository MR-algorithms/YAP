#include "virtualconsole.h"
#include <cassert>
#include "communicator.h"
#include "mask.h"
#include "scantask.h"
#include "myevent.h"
#include "databin.h"
#include <future>
#include <utility>
#include "QEvent"
#include "QApplication"
#include "QThread"


VirtualConsole VirtualConsole::s_instance;
using namespace Scan;


namespace _details{

class VirtualConsoleImpl
{
public:
    VirtualConsoleImpl();
    ~VirtualConsoleImpl();

    bool PrepareScantask(const ScanTask& task);
    bool Scan();
    void Stop();
    unsigned int GetSendIndex(){return _sendIndex;}

private:


    // Thread function used to carry out the scan task.
    static unsigned int ThreadFunction(VirtualConsoleImpl * This, std::promise<bool> &promiseObj);
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

    int _sendIndex;//表示已经发送了几条k空间数据（所有通道所有层的一行算一条）

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
unsigned int VirtualConsoleImpl::ThreadFunction(VirtualConsoleImpl *This, std::promise<bool> &promiseObj)
{

    std::unique_lock<std::mutex> Lock_scantask(This->_scanTask_mutex);//mutex::lock:locks the mutex, blocks if the mutex is not available
    ScanTask scantask = This->_scanTask;
    Lock_scantask.unlock();


    Databin databin;
    databin.GetCommunicator().get()->SetRemoteHost(scantask.ip_address.c_str(), scantask.port);
    bool succeed = databin.GetCommunicator().get()->Connect();

    vector<int> mask_vec=scantask.mask.data;
    int temp=scantask.mask.data.size();
    databin.SetSendPhaseCount(temp);

    //hard coding. set true for debugging.
    //If Succeed = true, it will always
    succeed = true;
    if(!succeed)
    {
        promiseObj.set_value(false);
        return -1;
    }
    else
    {
        promiseObj.set_value(true);
    }

    This->_connected = true;

    //

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
            This->_sendIndex = 0;
            int scan_id = 180106;

            databin.Start(scan_id, scantask.mask.channelCount);
            qDebug()<<"scan Event";

        }
            break;

        case MyEvent::time:
        {
            //Doing something.


            if(!databin.CanbeFinished())
            {
                /*
                vector<int> mask_vec=scantask.mask.data;
                int temp=scantask.mask.data.size();
                databin.SetSendPhaseCount(temp);
                */
                if(!databin.Go(mask_vec))
                {
//                    This->_timeMutex1.unlock();
//                    This->_sendIndex = 0;
                    qDebug()<<"Apply for unusual finished.";
                    //QApplication::postEvent(scantask.pWnd, new QEvent(QEvent::Type(QEvent::User + finished)));
                    databin.SetCurrentPhaseIndex(temp);
                }
                if(databin.CanbeFinished())
                {
                    This->_timeMutex1.unlock();
                    This->_sendIndex = 0;
                    qDebug()<<"Apply for finished.";

                    //其中，scantask.pWnd存储的是创建该线程的窗口，也就是该线程的父线程，主要用于该子线程向父线程发送特定事件，以便进行线程间通信。
                    //然后在任何你想要和父线程进行通信的地方，通过：
                    //QApplication::postEvent(scantask.pWnd, new QEvent(MyEvent));
                    //将该事件发送出去。const QEvent::Type MyEvent = (QEvent::Type)5001;建议用5000以上唯一的标识

                    QApplication::postEvent(scantask.pWnd, new QEvent(QEvent::Type(QEvent::User + finished)));
                }
                else
                {
                    //end of doing
                    This->_sendIndex ++;
                    qDebug() << "send: "<< This->_sendIndex << "times";


                    //QEvent evt(QEvent::Type(QEvent::User + scanning));
                    QApplication::postEvent(scantask.pWnd, new QEvent(QEvent::Type(QEvent::User + scanning)));
                }
            }
        }
            break;

        case MyEvent::stop:
        {
            This->_timeMutex1.unlock();

            databin.End();

            This->_sendIndex = 0;
            This->_threadState = idle;

            databin.GetCommunicator().get()->Disconnect();


            qDebug()<<"stop Event";

            return 1;
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

    std::promise<bool> promiseObj;
    std::future<bool> futureObj = promiseObj.get_future();

    //

    _threadState = idle;
    _thread = std::thread(ThreadFunction , this, std::ref(promiseObj));


    if( futureObj.get() )
    {
        //_timeMutex1.unlock();//
        //rand();
        std::unique_lock<std::mutex> Lock_event(_event_mutex);

        _evtQueue.get()->PushEvent(new MyEvent(MyEvent::scan));
        return true;

    }
    else
    {
        //连接不成功
        qDebug() << "connecting fail.";
        _thread.join();
        return false;

    }


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

unsigned int VirtualConsole::GetSendIndex()
{
    assert(_impl);
    return _impl->GetSendIndex();
}
