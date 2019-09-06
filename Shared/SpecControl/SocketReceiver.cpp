#include "stdafx.h"
#include "SocketReceiver.h"
#include "Implement/LogUserImpl.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <mutex>

using namespace std;


SocketReceiver::SocketReceiver()
{
}


SocketReceiver::~SocketReceiver()
{
	//�ر�ͨѶ��Ҫ���׽���
	closesocket(_socket_connect);

	//�رտͻ��˱�����׽���
	closesocket(_socket_listen);
	WSACleanup();//�ͷų�ʼ��Ws2_32.dll���������Դ��

}


bool SocketReceiver::Init(TransmitterProtocol::ProtocolType protocol_type)
{
	_protocol_type = protocol_type;

	int ii = 0;
	std::wstring error_info = L"";

	WSADATA wsa_data;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		error_info = L"��ʼ��Winsockʧ��";
		return false;
	}
	return true;

}
bool SocketReceiver::EstablishConnection()
{
	
	SOCKADDR_IN addr_srv;
	addr_srv.sin_family = AF_INET;
	addr_srv.sin_port = htons(8888);//�˿ں�
	//addrSrv.sin_addr.S_un.S_addr = inet_addr("192.168.56.101");
	addr_srv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//IP��ַ(���ڼ�����������ĵ�ַ)
													   //bzero(&(addrSrv.sin_zero) ); 
	std::wstring error_info = L"";

	//���������׽���
	
	_socket_listen = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_listen == SOCKET_ERROR)
	{
		wstringstream ss;
		ss << L"Socket() error" << WSAGetLastError() << std::endl;
		error_info += ss.str();
		return false;
	}

	
	if (!::bind(_socket_listen, (LPSOCKADDR)&addr_srv, sizeof(SOCKADDR_IN)))
	{
		wstringstream ss;
		ss << L"failed bind" << std::endl;
		error_info += ss.str();
	}

	if (listen(_socket_listen, 10) == SOCKET_ERROR)
	{
		wstringstream ss;
		ss << L"listen failed" << std::endl;
		error_info += ss.str();
	}

	int len = sizeof(SOCKADDR);

	SOCKADDR_IN addr_conn;
	_socket_connect = accept(_socket_listen, (SOCKADDR*)&addr_conn, &len);
	if (_socket_connect == SOCKET_ERROR)
	{
		wstringstream ss;
		ss << L"listen failed";
		error_info += ss.str();
		Yap::LOG_ERROR(L"<SocketReceiver> Listen failed!", L"BasicRecon");
	}
	wstringstream ss;
	ss << L"the IP address of conn is:" << inet_ntoa(addr_conn.sin_addr) << std::endl;
	std::wstring ss2str = ss.str();
	

	Yap::LOG_TRACE(ss2str.c_str(), L"SocketReceiver");
	return true;
}

//Reference page 32 of Network programming.
bool SocketReceiver::ReadBytes(std::vector<char> &receive_buffer, int buffer_size)
{
	receive_buffer.resize(buffer_size);
	char* recvbuff = receive_buffer.data();

	int ret, nLeft, idx;
	nLeft = buffer_size;
	idx = 0;
	while (left > 0)
	{
		ret = recv(_socket_connect, &recvbuff[idx], nLeft, 0);
		if (ret == SOCKET_ERROR)
		{
			//	Error
		}
		idx += ret;
		nLeft -= ret;
	}
	assert(nLeft == 0);
	return true;
}
std::shared_ptr<TransmitterProtocol::DataBlock> SocketReceiver::NextBlock()
{
	
	return nullptr;
	std::shared_ptr<TransmitterProtocol::DataBlock> data_block(new TransmitterProtocol::DataBlock(_protocol_type));
	
	(*data_block)._test_variable = 3;
	
	return data_block;
	
}


/*	
	int countRecv = 1;//��ʾ��Ҫ���ܼ���ͼ��
	while (countRecv)
	{
		//countRecv--;
		DataProtocol dataProtocol(0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr, nullptr);
		while (1)//ֻ���������������û�����ݣ�����ȫ�����ݣ�������������ݲ���9��������ͻ����
		{
			int irecv = recv(_socket_connect, (char*)&dataProtocol, 9 * sizeof(unsigned int), 0);
			std::cout << "irecv=" << irecv << std::endl;
			if (irecv == SOCKET_ERROR)
			{
				std::cout << "�������ݹؼ���Ϣʧ��" << std::endl;
			}
			else
			{
				std::cout << "�������ݹؼ���Ϣ�ɹ�" << std::endl;
				break;
			}
		}

		std::cout << "���յ������ݹؼ���Ϣ" << std::endl;
		std::cout << "  dataProtocol.totalSz" << dataProtocol.totalSz << std::endl
			<< "  dataProtocol.hdSz" << dataProtocol.hdSz << std::endl
			<< "  dataProtocol.dataSz" << dataProtocol.dataSz << std::endl
			<< "  dataProtocol.data_format" << dataProtocol.data_format << std::endl
			<< "  dataProtocol .cols" << dataProtocol.cols << std::endl
			<< "  dataProtocol .lins" << dataProtocol.lins << std::endl
			<< "  dataProtocol .ccha" << dataProtocol.ccha << std::endl
			<< "  dataProtocol .slcs" << dataProtocol.slcs << std::endl
			<< "  dataProtocol .cslc" << dataProtocol.cslc << std::endl;

		std::cout << "�������ݵ�ͷ��Ϣ" << std::endl;
		dataProtocol.hdBuf = new char[dataProtocol.hdSz];
		int hd_to_recv = dataProtocol.hdSz;
		int start_index = 0;
		while (true)
		{
			//int iRecv2 = recv(sockClient, (char *)(recvArr2+start_index), have_to_recv*sizeof(float), 0);
			int iRecv2 = recv(_sock_conn, (char *)(dataProtocol.hdBuf + start_index), hd_to_recv, 0);
			std::cout << "�Ѿ����յ���char���ݸ���=" << iRecv2 << "***********" << std::endl;
			std::cout << "��Ҫ���յ�char���ݸ���hd_to_recv - iRecv2=" << hd_to_recv - iRecv2 << "***********" << std::endl;
			if (iRecv2 >= hd_to_recv)
			{
				break;
			}
			else
			{
				hd_to_recv = hd_to_recv - iRecv2;
				start_index += iRecv2;
			}
		}
		for (int i = 0; i < 10; i++)
		{
			std::cout << i << " : " << dataProtocol.hdBuf[i] << std::endl;
		}
		std::cout << "�������ݵľ�����Ϣ" << std::endl;
		dataProtocol.dataBuf = new char[dataProtocol.dataSz];
		int data_to_recv = dataProtocol.dataSz;
		int start_idx = 0;
		while (true)
		{
			int iRecv3 = recv(_sock_conn, (char *)(dataProtocol.dataBuf + start_idx), data_to_recv, 0);
			std::cout << "�Ѿ����յ���char���ݸ���=" << iRecv3 << "***********" << std::endl;
			std::cout << "��Ҫ���յ�char���ݸ���data_to_recv - iRecv3=" << data_to_recv - iRecv3 << "***********" << std::endl;
			if (iRecv3 >= data_to_recv)
			{
				break;
			}
			else
			{
				data_to_recv = data_to_recv - iRecv3;
				start_idx += iRecv3;
			}
		}

		//����Format�Ĳ�ͬ��������ͬ���������������յ�������

		switch (dataProtocol.data_format)
		{
		case DATA_FORMAT_CXFL:
		{   
			int lenth = dataProtocol.dataSz / 8;
			std::complex<float>* raw_data_buffer = new std::complex<float>[lenth];
			memcpy(raw_data_buffer, dataProtocol.dataBuf, lenth * sizeof(std::complex<float>));
			for (int i = lenth - 10; i < lenth; i++)
			{
				std::cout << i << ":" << raw_data_buffer[i] << std::endl;
			}
			Dimensions dimensions;
			dimensions(DimensionReadout, 0U, dataProtocol.cols)
				(DimensionPhaseEncoding, 0U, dataProtocol.lins)
				(DimensionSlice, dataProtocol.cslc, 1)
				(DimensionChannel, 0U, dataProtocol.ccha);

			auto output_data = CreateData<complex<float>>(nullptr,
				reinterpret_cast<complex<float>*>(raw_data_buffer), dimensions);

			Feed(L"Output", output_data.get());
			//delete[] raw_data_buffer;
			break;
		}
		case DATA_FORMAT_FL:
		{
			int lenth = dataProtocol.dataSz / 4;
			float* raw_data_buffer = new float[lenth];
			memcpy(raw_data_buffer, dataProtocol.dataBuf, lenth * sizeof(float));
			for (int i = lenth - 10; i < lenth; i++)
			{
				std::cout << i << ":" << raw_data_buffer[i] << std::endl;
			}
			Dimensions dimensions;
			dimensions(DimensionReadout, 0U, dataProtocol.cols)
				(DimensionPhaseEncoding, 0U, dataProtocol.lins)
				(DimensionSlice, dataProtocol.cslc, 1)
				(DimensionChannel, 0U, dataProtocol.ccha);

			auto output_data = CreateData<float>(nullptr,
				reinterpret_cast<float*>(raw_data_buffer), dimensions);

			Feed(L"Output", output_data.get());
			break;
		}
		case DATA_FORMAT_SH:
		{
			int lenth = dataProtocol.dataSz / 2;
			short* recvArr = new short[lenth];
			memcpy(recvArr, dataProtocol.dataBuf, lenth * sizeof(short));
			for (int i = lenth - 10; i < lenth; i++)
			{
				std::cout << i << ":" << recvArr[i] << std::endl;
			}
			delete[]recvArr;
			break;
		}
		default:
			break;
		}

		

		if (dataProtocol.cslc == dataProtocol.slcs)//����֪�Ѿ�����������Ͼ��˳�ѭ������������ʹ�õ�cslc==slcs�����������dataProtocol�����һ����ʾ���ս����ı�־flag��
		{
			break;
		}
	}
	*/
	//return true;


