#pragma once
#include "EventHandler.h"
#include "EventDemultiplexer.h"
#include "event.h"

class ConnectionHandler : public EventHandler {
public:
	ConnectionHandler(Handle connection);
	virtual ~ConnectionHandler();
	virtual HandleInfo get_handleinfo() const {
		return _connection_info;
	}
	virtual void handle_read();
	virtual void handle_write();
	virtual void handle_error();
	virtual bool handle_regist();
	
private:

	HandleInfo _connection_info;
	char* _buf;
	static const int MAX_SIZE = 1024;
};

