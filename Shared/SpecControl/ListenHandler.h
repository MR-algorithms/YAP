#pragma once
#include "EventHandler.h"
#include "event.h"
//ListenHander is an element of Reactor Design Pattern.
//1, It is a special handler which can create other handler. 
//According to the Reactor Pattern, a new ListenHandler should be created before Starting
//the pattern. We have not implemented the singleton pattern, though only one ListenHandler
//instance is considered. 
//2, About shake: for simplization purpose, the "connection success" is used to denote shaking 
//successfully.

class ListenHandler : public EventHandler {
public:
	explicit ListenHandler(int port);
	virtual ~ListenHandler();
	virtual HandleInfo get_handleinfo() const {
		return _listening_info;
	}
	virtual void handle_read();
	virtual void handle_write();
	virtual void handle_error();
	virtual bool handle_regist();
	
private:
	SOCKET Init(int port);
		
	HandleInfo _listening_info;
};
