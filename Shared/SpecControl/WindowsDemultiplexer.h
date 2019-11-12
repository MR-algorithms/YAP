#pragma once

#include <vector>
#include "EventHandler.h"
#include "EventDemultiplexer.h"

//Note: Dumultiplexer class is running in the same thread as user, and it doesn't create any thread for simple disgnn.

class WindowsDemultiplexer : public EventDemultiplexer {
public:
	WindowsDemultiplexer();
	virtual ~WindowsDemultiplexer();
	virtual int wait_event();
	virtual bool regist(EventHandler* handler);
	virtual int remove(EventHandler* handler);
private:
	std::vector<HandleInfo> _handlers;
	int find_index(HandleInfo const &handle_info) const;
	
	
};
