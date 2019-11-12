#include "stdafx.h"
#include "reactor.h"
#include "ConnectionHandler.h"
#include <string.h>
#include <stdio.h>
#include <cassert>
#include <stdio.h>
#include <WinSock2.h>
#include <new>


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

	//assert(0);
	if (INVALID_SOCKET != _connection_info.handle)
	{
		//What does it mean with parameters "NLL, 0"?
		if (0 != WSAEventSelect(_connection_info.handle, NULL, 0))
		{
			//LOG_ERROR(FormatStringW(L"SetEventNULL Error %d", WSAGetLastError()).c_str());
			
		}
		u_long block = 0;
		if (0 != ioctlsocket(_connection_info.handle, FIONBIO, &block))
		{
			//LOG_ERROR(FormatStringW(L"SetUnblock Error %d", WSAGetLastError()).c_str());
			
		}
		//items[index].handler(socket);
		return true;
	}
	return false;

}

