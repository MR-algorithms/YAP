#include "myevent.h"
#include <mutex>


MyEvent* EventQueue::GetEvent(std::timed_mutex &timeMutex, int milliSeconds)
{
    std::unique_lock<std::mutex> locker(_evtQueueMtx);
    bool timeOut = false;
    while(_eventQueue.empty() && !timeOut)
    {
        _evtQueueCondVar.wait(locker, [&]{
            if(milliSeconds < 0 || milliSeconds > 60000)
            {
                timeOut = false;
            }
            else
            {
                timeOut =  !timeMutex.try_lock_for(std::chrono::milliseconds(milliSeconds));
            }

            return timeOut;
        });

    }
    MyEvent* evt = _eventQueue.front();
    _eventQueue.pop_front();
    return evt;
}

void EventQueue::PushEvent(MyEvent* evt)
{
    _evtQueueMtx.lock();
    const bool bNeedNotify = _eventQueue.empty();
    _eventQueue.push_back(evt);
    _evtQueueMtx.unlock();
    if (bNeedNotify)
        _evtQueueCondVar.notify_all();
}
