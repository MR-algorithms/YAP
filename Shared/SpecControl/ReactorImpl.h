#pragma once

#include "EventHandler.h"
#include "EventDemultiplexer.h"


class ReactorImpl {
public:
	ReactorImpl();
	~ReactorImpl();
	int  regist(EventHandler* handler);
	void remove(EventHandler* handler);
	void dispatch();

private:
	bool Init();
	EventDemultiplexer* _demultiplexer;
	
	
};


