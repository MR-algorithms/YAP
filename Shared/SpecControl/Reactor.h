#pragma once

#include "EventHandler.h"


class ReactorImpl;

class Reactor {
public:
	static Reactor& get_instance();
	int regist(EventHandler* handler);
	void remove(EventHandler* handler);
	void dispatch();

private:
	Reactor();
	~Reactor();
	Reactor(const Reactor&);
	//Reactor& operator=(const Reactor&);

private:
	ReactorImpl* _reactor_impl;
	static Reactor reactor;
};

