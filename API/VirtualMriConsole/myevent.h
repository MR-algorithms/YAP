#ifndef MYEVENT_H
#define MYEVENT_H



#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <string>
#include <iostream>
#include <chrono>

class MyEvent {
public:
    enum Type : int {
        quit  = 0 ,
        scan  = 1,
        stop  = 2,

    };

    explicit MyEvent(Type type)
        : m_type(type)
    { }

    virtual ~MyEvent()
    { }

    Type type() const { return m_type; }
private:
    Type m_type;
};

class EventQueue {
public:
    MyEvent* GetEvent();
    void PushEvent(MyEvent* evt);
private:

    std::condition_variable m_evtQueueCondVar;
    std::mutex m_evtQueueMtx;
    std::list<MyEvent*> m_eventQueue;
};


#endif // MYEVENT_H
