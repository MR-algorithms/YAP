#pragma once

#include <map>
#include "EventHandler.h"
#include <WinSock2.h>
#include <vector>
#include "EventHandler.h"


class EventDemultiplexer {
public:
	EventDemultiplexer() {}
	virtual ~EventDemultiplexer() {}
	virtual int wait_event() = 0;
	virtual bool regist(EventHandler* handler) = 0;
	virtual int remove(EventHandler* handler) = 0;
};

