#include "myevent.h"


MyEvent* EventQueue::GetEvent()
{
    std::unique_lock<std::mutex> locker(m_evtQueueMtx);
    while(m_eventQueue.empty())
        m_evtQueueCondVar.wait(locker);
    MyEvent* evt = m_eventQueue.front();
    m_eventQueue.pop_front();
    return evt;
}

void EventQueue::PushEvent(MyEvent* evt)
{
    m_evtQueueMtx.lock();
    const bool bNeedNotify = m_eventQueue.empty();
    m_eventQueue.push_back(evt);
    m_evtQueueMtx.unlock();
    if (bNeedNotify)
        m_evtQueueCondVar.notify_all();
}
