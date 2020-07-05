#include "ListenHandler.h"
#include "EventHandler.h"
#include "Reactor.h"
#include "ConnectionHandler.h"
#include "EventDemultiplexer.h"
#include "Implement\LogUserImpl.h"
#include "QtUtilities\FormatString.h"
#include <sstream>

#include <stdio.h>
#include <new>
#include <assert.h>


ListenHandler::ListenHandler(int port)
{
	SOCKET listening_socket = Init(port);

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
    WSACloseEvent(_listening_info.event);
	shutdown(_listening_info.handle, SD_BOTH);
	closesocket(_listening_info.handle);
}
//ListenHandler's read function is different from that of ConnectHandler. 
//When the server received an Event of connection request, it can accept the connection socket.
void ListenHandler::handle_read() 
{
	
	//----------
    //WSAResetEvent(_listening_info.event);

    //int connection = accept(_listening_info.handle, NULL, NULL);
	
    //Check if the accept is successful.
    SOCKADDR_IN addrConn;
    int len = sizeof(SOCKADDR);
    int connection = accept(_listening_info.handle, (SOCKADDR*)&addrConn, &len);
    if (connection == SOCKET_ERROR)
    {
        std::wstringstream wss;
        wss << L"failed accept" << std::endl;
        //DebugInfo::Output(L"ListenHandler::Hand_read", wss.str(),
        //    reinterpret_cast<int>(this), true);
    }
    std::wstringstream wss;
    wss << "the IP address of conn is:" << inet_ntoa(addrConn.sin_addr) << std::endl;
    //DebugInfo::Output(L"ListenHandler::Hand_read", wss.str(),
    //    reinterpret_cast<int>(this), true);


    //
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
	
	std::wstring  werror_info;
	if (listening_socket == SOCKET_ERROR)
	{
		
		std::wstringstream ss;
		ss << L"Socket() error" << WSAGetLastError() << std::endl;
		werror_info = ss.str();
		
	}

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
		Yap::LOG_ERROR(FormatStringW(L"bind socket error, error code %d.", error).c_str(), L"SpecControl");
		return INVALID_SOCKET;
	}
	/*
	if (!::bind(listening_socket, (LPSOCKADDR)&server_addr, sizeof(SOCKADDR_IN)))
	{
		std::wstringstream ss;
		ss << L"failed bind" << L"error_code : "<<WSAGetLastError()<<std::endl;
		werror_info = ss.str();
	}
	*/
	int result_code2 = listen(listening_socket, 5);

	if (result_code2 == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		closesocket(listening_socket);
		Yap::LOG_ERROR(FormatStringW(L"listen error, error code %d.", error).c_str(), L"SpecControl");
		return INVALID_SOCKET;
	}
	
	return listening_socket;
}

bool ListenHandler::handle_regist()
{
	
	SOCKET handle = _listening_info.handle;
	WSAEVENT event = _listening_info.event;

	if (SOCKET_ERROR == WSAEventSelect(handle, event, FD_CLOSE | FD_ACCEPT))
	{
		int error = WSAGetLastError();
		closesocket(handle);
		Yap::LOG_ERROR(FormatStringW(L"ListenHandler::Handle_regist(): EventSelect error, error code %d.", error).c_str(), L"SpecControl");
		return false;
	}
	return true;
		
}


bool ListenHandler::handle_close()
{
    assert(0 &&L"Not Implemented!");
    return true;
}
