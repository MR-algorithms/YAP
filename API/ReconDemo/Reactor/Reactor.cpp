#include "reactor.h"
#include "ReactorImpl.h"
#include <assert.h>
#include <new>
#include "Utilities\FormatString.h"

Reactor Reactor::reactor;

Reactor& Reactor::get_instance() 
{
	return reactor;
}

Reactor::Reactor() 
{
	DebugInfo::Output(L"Reactor::Reactor", L"new ReactorImpl()...",
		reinterpret_cast<int>(this), true);

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
