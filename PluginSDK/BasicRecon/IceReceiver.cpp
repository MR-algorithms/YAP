#include "stdafx.h"
#include "IceReceiver.h"
#include "SpecControl/SocketReceiver.h"
#include "Implement/LogUserImpl.h"

#include <sstream>
#include <iostream>

#include <iomanip>
#include <fstream>

using namespace Yap;
using namespace std;
using namespace TransmitterProtocol;

//using boost::assign::list_of;

IceReceiver::IceReceiver(void) :
	ProcessorImpl(L"IceReceiver")
{
	AddInput(L"Input", 0, DataTypeUnknown);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexFloat);

}

IceReceiver::IceReceiver(const IceReceiver & rhs)
	: ProcessorImpl(rhs)
{
}

IceReceiver::~IceReceiver()
{
}

bool IceReceiver::Input(const wchar_t * name, IData * data)
{
	// Should not pass in data to start raw data file reading.
	assert(data == nullptr);

	SocketReceiver socket_receiver;
	if (!socket_receiver.Init(ProtocolType::ProtocolType_Pjf)) 
	{
		cout << "_socket_receiver.Init() failed" << endl;
		return false;
	}
	std::shared_ptr<DataBlock> data_block = socket_receiver.NextBlock();

	if (data_block)
	{
		OutputDatablock(*data_block);
		return true;
	}
	else//If data_block is nullptr, what shall we do?
	{
		return false;
	}
}

 

void IceReceiver::OutputDatablock(DataBlock& data_block)
{
	data_block._test_variable = 12;
	/*
	PjfDataProtocolHeader header_structer();
	header_structer = static_cast<PjfDataProtocolHeader>(data_block.items[0].buffer.data());


	DataProtocol dataProtocol(0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr, nullptr);
	while (1)//ֻ���������������û�����ݣ�����ȫ�����ݣ�������������ݲ���9��������ͻ����
	{
		int irecv = recv(_sock_conn, (char*)&dataProtocol, 9 * sizeof(unsigned int), 0);
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
		memcpy(raw_data_buffer, dataProtocol.dataBuf, lenth*sizeof(std::complex<float>));
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
		memcpy(raw_data_buffer, dataProtocol.dataBuf, lenth*sizeof(float));
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
		memcpy(recvArr, dataProtocol.dataBuf, lenth*sizeof(short));
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
	*/
}


