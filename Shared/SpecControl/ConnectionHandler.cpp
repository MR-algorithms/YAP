#include "stdafx.h"
#include "reactor.h"
#include "ConnectionHandler.h"
#include <string.h>
#include <stdio.h>
#include <cassert>
#include <stdio.h>
#include <WinSock2.h>
#include <new>
#include "FormatString.h"
#include "Implement\LogUserImpl.h"

ConnectionHandler::ConnectionHandler(Handle connection) 
{
	_buf = new (std::nothrow)char[MAX_SIZE];
	assert(_buf != nullptr);
	memset(_buf, 0, MAX_SIZE);
	WSAEVENT event = WSACreateEvent();
	_connection_info = HandleInfo(event, connection, this);

}

ConnectionHandler::~ConnectionHandler() 
{
	//When you are finished receiving and sending data on the connection socket, or you are finished accepting new connections 
	//on ListeningSocket, you should close the sockets.// shutdown maybe needed.
	shutdown(_connection_info.handle, SD_BOTH);
	closesocket(_connection_info.handle);
	delete[] _buf;
}

void ConnectionHandler::handle_read() 
{
	//if (WSARecv(_socket_connection, _buf, MAX_SIZE) > 0) {
	//	WSASend(_socket_connection, _buf, strlen(_buf));
	//}
	handle_error();
}

void ConnectionHandler::handle_write() 
{
	// do nothing
}

void ConnectionHandler::handle_error()
{
	Reactor& r = Reactor::get_instance();
	r.remove(this);
}

bool ConnectionHandler::handle_regist()
{

		
	SOCKET handle = _connection_info.handle;
	WSAEVENT event = _connection_info.event;
	
	assert(INVALID_SOCKET != handle);
	if (SOCKET_ERROR == WSAEventSelect(handle, event, FD_READ))
	{
		int error = WSAGetLastError();
		closesocket(handle);
		Yap::LOG_ERROR(FormatStringW(L"ConnectionHandler::handle_regist(): EventSelect error, error code %d.", error).c_str(), 
			L"SpecControl");
		return false;
	}
	return true;

}

