#pragma once
#include <stdint.h>
#include <vector>
//Try to define a simple transmitter protocol used to send or receiver data in the following form:
//DataLen(4 bytes) + DataType(4 byte) + data(content -- a struct or a series of floats/double/complex<float>/complex<double>)
//Note:
//1, Specific Application is not considered in this protocol; Application protocol can be  considered based on this protocol.

namespace TransmitterProtocol
{
	//Hope to be able to support different user protocols on which one can send or receive data between computers based.
	enum ProtocolType
	{
		ProtocolType_Pjf = 1,
	};

	//DataflowType
	const int DataflowType_DATA_FORMAT_CXFL = 1;
	const int DataflowType_DATA_FORMAT_FL = 2;
	const int DataflowType_DATA_FORMAT_SH = 3;
	const int DataflowType_PjfstructerHeader = 4;
	const int Dataflowtype_HeaderBuffer = 5;

	//struct IFlowStruct

	struct PjfDataProtocolHeader
	{
		unsigned int totalcount;
		unsigned int headercount;
		unsigned int datacount;
		unsigned int dataformat;
		unsigned int columncount;
		unsigned int linecount;
		unsigned int currentchannel;
		unsigned int slicecount;
		unsigned int currentslice;

		PjfDataProtocolHeader() : totalcount(0), headercount(0), datacount(0), dataformat(0), columncount(0),
			linecount(0), currentchannel(0), slicecount(0), currentslice(0) {}

		PjfDataProtocolHeader(unsigned int total_count, unsigned int header_count, unsigned int data_count,
			unsigned int data_format, unsigned int column_count, unsigned int line_count, unsigned int current_channel,
			unsigned int slice_count, unsigned int current_slice) :
			totalcount(total_count), headercount(header_count), datacount(data_count), dataformat(data_format), columncount(column_count),
			linecount(line_count), currentchannel(current_channel), slicecount(slice_count), currentslice(current_slice) {}

		PjfDataProtocolHeader& operator = (const PjfDataProtocolHeader& rhs) {
			totalcount	   = rhs.totalcount;
			headercount    = rhs.headercount;
			datacount	   = rhs.datacount;
			dataformat     = rhs.dataformat;
			columncount    = rhs.columncount;
			linecount      = rhs.linecount;
			currentchannel = rhs.currentchannel;
			slicecount     = rhs.slicecount;
			currentslice   = rhs.currentslice;
			return *this;
		}
		~PjfDataProtocolHeader() {};
	};

	//Defined for future.
	struct DataItem
	{
		int32_t lengh;
		int32_t type;
		std::vector<char> buffer;
	};

	//Pjf protocol, which is header + header_buffer +data_buffer£¬should be changed to adapting new scheme. 
	//One DataBlock is what is ready to be put into the process pipeline. it maybe a k-space data a image of a slice, or a k-space line of all the slices of 
	//one channel.
	struct DataBlock
	{
		
		std::vector<DataItem> items;
		
		int get_itme_count();
		int _test_variable;
		DataBlock(ProtocolType protocol_type);
		~DataBlock();
	
	};

}