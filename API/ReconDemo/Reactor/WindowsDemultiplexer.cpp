#include "WindowsDemultiplexer.h"

#include <vector>
#include <iostream>
#include <errno.h>
#include <winsock2.h>
#include <assert.h>
#include <array>
#include "QtUtilities/FormatString.h"

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

    //std::array<HANDLE, 64> event_array;
    WSAEVENT event_array[WSA_MAXIMUM_WAIT_EVENTS];

	for (size_t i = 0; i < _handlers.size(); ++i)
	{
		event_array[i] = _handlers[i].event;
	}
    DWORD event_count = static_cast<DWORD>(_handlers.size());

    assert( event_count <= WSA_MAXIMUM_WAIT_EVENTS);

    //param1: count of the event objects.
    //param3: FALSE: return when any one of the event objects is signaled. return value indicates which event.
    //param4: WSA_INFINETE: returns only when one of the event objects is signaled.
    //the last param: ignore.


    DebugInfo::Output(L"WindowsDemultiplexer::wait_event()", L"Calling WSAWaitMultipleEvents",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type1);


    int index = ::WSAWaitForMultipleEvents(event_count, event_array,
		FALSE, WSA_INFINITE, FALSE);

    DebugInfo::Output(L"WindowsDemultiplexer::wait_event()", L"Receiveing an event",
                      reinterpret_cast<int>(this), false, DebugInfo::flow_type1);

	index = index - WSA_WAIT_EVENT_0;

	//Iterator to see if more than one is signaled.

    for (int i = index; i < event_count; i++)
	{
        //index2: only indicates if there is a signaled event object.
        int index2 = ::WSAWaitForMultipleEvents(static_cast<DWORD>(1), &event_array[i],
			TRUE, 1000, FALSE);
        if (index2 == WSA_WAIT_FAILED || index2 == WSA_WAIT_TIMEOUT)
		{
			continue;
		}
		else// There is some event to be handlered by the specified handler.
		{
			WSANETWORKEVENTS network_event;
            //params2: WSAEVENT: optional, the event will be set to a non-signaled state. how to do ¡°option¡±? //"_handlers[index].event"
            //It can be trusted that only one network event associated with a event object every time.
            int num_result = ::WSAEnumNetworkEvents(_handlers[i].handle, _handlers[i].event, &network_event);
            if(num_result == SOCKET_ERROR)
            {
                continue;
            }

            if (network_event.lNetworkEvents & FD_CLOSE)
            {
                assert(0 && L"Handle close Not Implemented!");
                _handlers[i].handler->handle_close();
            }
            else if (network_event.lNetworkEvents & FD_ACCEPT)
            {
                if(network_event.iErrorCode[FD_ACCEPT_BIT] != 0) {
                    assert(0);
                }
                else {
                    _handlers[i].handler->handle_read();
                }
            }
            else if (network_event.lNetworkEvents & FD_READ)
            {
                if(network_event.iErrorCode[FD_READ_BIT] != 0) {
                    assert(0);
                }
                else {
                    _handlers[i].handler->handle_read();
                }

			}
			else if (network_event.lNetworkEvents & FD_WRITE)
			{
                assert(0 && L"Socket Write Not Implemented!");
                _handlers[index].handler->handle_write();
			}
            else if (network_event.lNetworkEvents & FD_CLOSE)
            {
                _handlers[index].handler->handle_close();
            }
            else{
                if(network_event.iErrorCode[FD_CLOSE_BIT] == 0) {
                    //Why this bit is set to 0 receiving a event with "lNetworkEvents ==0"?
                    DebugInfo::Output(L"WindowsDemultiplexer::wait_event()", L"Receiveing an unexpected event with value==0",
                                      reinterpret_cast<int>(this), false, DebugInfo::flow_type1);

                    continue;
                }

                assert(0 && L"unexpected network event.");
            }


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
	if (!handler->handle_regist())
	{
		this->remove(handler);
	}
	return true;
	
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
