#ifndef MYEVENT_H
#define MYEVENT_H



#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <string>
#include <iostream>
#include <chrono>
#include <mutex>

class MyEvent {
public:
    enum Type {
        scan,
        stop,
        time,
        quit,
   };

    explicit MyEvent(Type type): _type(type){ }
    virtual ~MyEvent() { }
    Type type() const { return _type; }
private:
    Type _type;
};

class EventQueue {
public:
    MyEvent* GetEvent(std::timed_mutex &timeMutex, int milliSeconds);
    void PushEvent(MyEvent* evt);
private:
    std::condition_variable _evtQueueCondVar;
    std::mutex _evtQueueMtx;
    std::list<MyEvent*> _eventQueue;
};


#endif // MYEVENT_H
