#include "stdafx.h"
#include "reactor.h"
#include "ReactorImpl.h"
#include <assert.h>
#include <new>

Reactor Reactor::reactor;

Reactor& Reactor::get_instance() 
{
	return reactor;
}

Reactor::Reactor() 
{
	_reactor_impl = new (std::nothrow)ReactorImpl();
	assert(_reactor_impl != NULL);
}

Reactor::~Reactor() 
{
	if (_reactor_impl != NULL) {
		delete _reactor_impl;
		_reactor_impl = NULL;
	}
}


int Reactor::regist(EventHandler* handler) 
{
	return _reactor_impl->regist(handler);
}

void Reactor::remove(EventHandler* handler) 
{
	return _reactor_impl->remove(handler);
}

void Reactor::dispatch() 
{
	return _reactor_impl->dispatch();
}
