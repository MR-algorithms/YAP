#include "stdafx.h"

#include "ReactorImpl.h"
#include <new>
#include <assert.h>
#include "WindowsDemultiplexer.h"
#include <WinSock2.h>

ReactorImpl::ReactorImpl() 
{
	_demultiplexer = new (std::nothrow)WindowsDemultiplexer();
	assert(_demultiplexer != NULL);
	assert(Init());
}

ReactorImpl::~ReactorImpl() 
{
	
	if (_demultiplexer != NULL) 
	{
		delete _demultiplexer;
	}

	WSACleanup();
}
int ReactorImpl::regist(EventHandler* handler) 
{
	
	return _demultiplexer->regist(handler);
}

void ReactorImpl::remove(EventHandler* handler) 
{
	_demultiplexer->remove(handler);

	
	
}

void ReactorImpl::dispatch() 
{
	_demultiplexer->wait_event();
}

//Winsock library initialize.
bool ReactorImpl::Init()
{
	WSADATA wsa_data;
	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h*/
	int result_code = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result_code != 0)
	{
		//log
		result_code = WSAGetLastError();
		//LOG_ERROR(FormatStringW(L"WSAStartUp Failed, Error code %d.", result_code).c_str());
		return false;
	}
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	if (LOBYTE(wsa_data.wVersion) != 2 || HIBYTE(wsa_data.wVersion) != 2)
	{
		//LOG_ERROR(FormatStringW(L"WSAVersion Error %d, %d.", LOBYTE(wsa_data.wVersion), HIBYTE(wsa_data.wVersion)).c_str());
		WSACleanup();
		return false;
	}
	return true;
}


