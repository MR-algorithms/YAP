#pragma once
//xhb:
//This is a socket interface implementing base function for data receiver.
//class version: 1.0
//
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <complex>
#include <math.h>
#include "winsock2.h"
#include <memory>
#include "TransmitterProtocol.h"
#include "Implement/LogUserImpl.h"


#pragma comment(lib,"ws2_32.lib")//引用库文件
#pragma warning(disable:4996)

class SocketReceiver
{
public:
	SocketReceiver();
	~SocketReceiver();

	bool Init(TransmitterProtocol::ProtocolType protocol_type);
	bool EstablishConnection();//Establish a connection to data source and return (only one connection is supported in version 1.0. Q: return value / reference.

	std::shared_ptr<TransmitterProtocol::DataBlock> NextBlock();
	//data_package receive_data();
	//void send_data(data_package const &data);
protected:

	bool ReadBytes(std::vector<char> &receive_buffer, int buffer_size);

	TransmitterProtocol::ProtocolType _protocol_type;
	SOCKET _socket_listen;
	SOCKET _socket_connect;
	
	
	
};

