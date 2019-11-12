#include "stdafx.h"
#include "WindowsDemultiplexer.h"

#include <vector>
#include <iostream>
#include <errno.h>
#include <winsock2.h>
#include <assert.h>
#include <array>

WindowsDemultiplexer::WindowsDemultiplexer() 
{
	
}

WindowsDemultiplexer::~WindowsDemultiplexer() 
{
	std::vector<HandleInfo>::iterator iter = _handlers.begin();
	for (; iter != _handlers.end(); ++iter)
	{
		delete (*iter).handler;
	}

}


int WindowsDemultiplexer::wait_event() 
{
	if (_handlers.size() == 0 || _handlers.size() >= 63)
	{
		return false;
	}

	std::array<HANDLE, 64> event_array;
	for (size_t i = 0; i < _handlers.size(); ++i)
	{
		event_array[i] = _handlers[i].event;
	}

	
	//WSA_INFINITE 
	int index = ::WSAWaitForMultipleEvents(static_cast<DWORD>(_handlers.size()), event_array.data(),
		FALSE, WSA_INFINITE, FALSE);
	index = index - WSA_WAIT_EVENT_0;

	//Iterator to see if more than one is signaled.
	for (int i = index; i < _handlers.size(); i++)
	{
		index = ::WSAWaitForMultipleEvents(static_cast<DWORD>(1), event_array.data() + i,
			TRUE, 1000, FALSE);
		if (index = WSA_WAIT_FAILED || index == WSA_WAIT_TIMEOUT)
		{
			continue;
		}
		else// There is some event to be handlered by the specified handler.
		{
			WSANETWORKEVENTS network_event;
			::WSAEnumNetworkEvents(_handlers[index].handle, _handlers[index].event, &network_event);
			if (network_event.lNetworkEvents & FD_ACCEPT ||
				network_event.lNetworkEvents & FD_READ)
			{
				_handlers[index].handler->handle_read();

			}
			else if (network_event.lNetworkEvents & FD_WRITE)
			{
				_handlers[index].handler->handle_write();
			}
			//handler->handle_write();

		}
	}
	
	
	return true;
}

int WindowsDemultiplexer::find_index(HandleInfo const &handle_info) const
{
	for (int index = 0; index != _handlers.size(); ++index)
	{
		if (_handlers[index].handle == handle_info.handle)
		{
			return index;
		}
	}

	return -1;
}

//Check if the handler is in the Handlers, push_back if is not.
bool WindowsDemultiplexer::regist(EventHandler* handler)
{
	HandleInfo handle_info = handler->get_handleinfo();
	if (find_index(handle_info) < 0)
	{
		_handlers.push_back(handle_info);

	}
	
	return handler->handle_regist();
	
}

int WindowsDemultiplexer::remove(EventHandler* handler) 
{
	
	HandleInfo handle_info = handler->get_handleinfo();
	
	int index = find_index(handle_info);
	assert(index > 0);

	delete _handlers[index].handler;

	_handlers.erase(_handlers.begin() + index, _handlers.begin() + index + 1);

	return 0;
}
