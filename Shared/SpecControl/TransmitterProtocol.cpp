#include "stdafx.h"
#include "TransmitterProtocol.h"


TransmitterProtocol::DataBlock::DataBlock(ProtocolType protocol_type)
{
	switch (protocol_type)
	{
	case ProtocolType_Pjf:
		break;
	default:
		break;
	}
	_test_variable = -1;

}


TransmitterProtocol::DataBlock::~DataBlock()
{
	
}

int TransmitterProtocol::DataBlock::get_itme_count()
{
	return items.size();
}