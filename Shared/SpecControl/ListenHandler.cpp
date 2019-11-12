#include "stdafx.h"

#include "ListenHandler.h"
#include "EventHandler.h"
#include "Reactor.h"
#include "ConnectionHandler.h"
#include "EventDemultiplexer.h"
#include "Implement\LogUserImpl.h"
//#include "FormatString.h"

#include <stdio.h>
#include <new>
#include <assert.h>


ListenHandler::ListenHandler()
{
	SOCKET listening_socket = Init(8000);

	WSAEVENT event = WSACreateEvent();
	if (event == WSA_INVALID_EVENT)
	{
		int error = WSAGetLastError();
		closesocket(listening_socket);
		//Yap::LOG_ERROR(FormatStringW(L"CreateEvent error, error code %d.", error).c_str(), L"SpecControl");
		Yap::LOG_ERROR(L"<ListenHandler> Create event failed!", L"SpecControl");
		
	}
	_listening_info = HandleInfo(event, listening_socket, this);


}

ListenHandler::~ListenHandler() 
{
	
	shutdown(_listening_info.handle, SD_BOTH);
	closesocket(_listening_info.handle);
}
//ListenHandler is a special read function other than usual Handler. 
//When the server received an Event of connection request, it can accept the connection socket.
void ListenHandler::handle_read() 
{
	
	//----------
	int connection = accept(_listening_info.handle, NULL, NULL);
	
	//Create new connection handler and register it.
	EventHandler* handler = new (std::nothrow)ConnectionHandler(connection);
	assert(handler != NULL);
	
	Reactor::get_instance().regist(handler);
}

void ListenHandler::handle_write() {
	// do nothing
}

void ListenHandler::handle_error() {
	// do nothing
}




SOCKET ListenHandler::Init(int port)
{
	
	SOCKET listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 创建了一个流套接字
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int result_code = bind(listening_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (result_code == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		closesocket(listening_socket);
		//LOG_ERROR(FormatStringW(L"bind socket error, error code %d.", error).c_str());
		return INVALID_SOCKET;
	}

	result_code = listen(listening_socket, 5);
	if (result_code == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		closesocket(listening_socket);
		//LOG_ERROR(FormatStringW(L"listen error, error code %d.", error).c_str());
		return INVALID_SOCKET;
	}
	
	return listening_socket;
}

bool ListenHandler::handle_regist()
{
	//assert(0);
	
	//------------
	SOCKET handle = _listening_info.handle;
	WSAEVENT event = _listening_info.event;

	if (SOCKET_ERROR == WSAEventSelect(handle, event, FD_CLOSE | FD_ACCEPT))
	{
		int error = WSAGetLastError();
		closesocket(handle);
		//LOG_ERROR(FormatStringW(L"EventSelect error, error code %d.", error).c_str());
		return false;
	}
	return true;
		
}