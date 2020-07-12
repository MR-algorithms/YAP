#include "reactor.h"
#include "ConnectionHandler.h"
#include <string.h>
#include <stdio.h>
#include <cassert>
#include <stdio.h>
#include <WinSock2.h>
#include <new>
#include "QtUtilities\FormatString.h"
#include "Implement\LogUserImpl.h"
#include "MessagePack/reconclientsocket.h"

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

    WSACloseEvent(_connection_info.event);
	shutdown(_connection_info.handle, SD_BOTH);
	closesocket(_connection_info.handle);
	delete[] _buf;
}

void ConnectionHandler::handle_read() 
{
	//if (WSARecv(_socket_connection, _buf, MAX_SIZE) > 0) {
	//	WSASend(_socket_connection, _buf, strlen(_buf));
	//}

    if(!_rconclient_socket)
        _rconclient_socket = std::shared_ptr<ReconClientSocket>(new ReconClientSocket(_connection_info.handle));

    //WSAResetEvent(_connection_info.event);
    _rconclient_socket->DataReceived();


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
            L"Reactor");
		return false;
	}

    //handle_read();
    //u_long block = 0;
    //if(0 != ioctlsocket(handle, FIONBIO, &block))
    //{
    //    Yap::LOG_ERROR(FormatStringW(L"SetUnblock Error %d", WSAGetLastError()).c_str(),
    //                    L"Reactor");
    //    return false;
    //}
	return true;

}


bool ConnectionHandler::handle_close()
{
    assert(0 &&L"Not Implemented!");
    return true;
}
