#pragma once

#include <WinSock2.h>
typedef SOCKET Handle;
class EventHandler;
struct HandleInfo
{
	WSAEVENT event;
	Handle handle;
	EventHandler * handler;
    HandleInfo() :event(0), handle(INVALID_SOCKET), handler(nullptr) { }
	HandleInfo(WSAEVENT event1, Handle handle1, EventHandler* handler1) : event(event1), handle(handle1), handler(handler1) { }
	HandleInfo(HandleInfo const &rhs) {
		event = rhs.event;
		handle = rhs.handle;
		handler = rhs.handler;
	}
	HandleInfo& operator=(HandleInfo const &rhs) {
		event = rhs.event;
		handle = rhs.handle;
		handler = rhs.handler;
		return *this;
	}

};
class EventHandler {
public:
	EventHandler() {}
	virtual ~EventHandler() {}
	virtual HandleInfo get_handleinfo() const = 0;
	virtual void handle_read() = 0;
	virtual void handle_write() = 0;
	virtual void handle_error() = 0;
	virtual bool handle_regist() = 0;
    virtual bool handle_close() = 0;
	
};


