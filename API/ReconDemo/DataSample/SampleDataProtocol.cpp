#include "SampleDataProtocol.h"
#include <QByteArray>
#include <QString>
#include <cassert>

#define PACK_ERROR_RETURN_FALSE(a)\
	if(mpeSuccess != (a))\
{\
	return false;\
}

SampleDataStart::SampleDataStart()
	: cmd_id(SAMPLE_DATA_START)
	, version(SAMPLE_DATA_PROTOCOL_VERSION)
	, scan_id(-1)
	, dim23456_size(0)
	, dim1_size(0)
	, dim2_size(0)
	, dim3_size(0)
	, dim4_size(0)
	, dim5_size(0)
	, dim6_size(0)
	, channel_mask(0)
{}

bool SampleDataStart::Pack( QByteArray &byteArray )
{
    byteArray = QByteArray::fromRawData((char*)this, sizeof(SampleDataStart));
	return true;
}

bool SampleDataStart::Unpack( QByteArray &byteArray )
{
    if(byteArray.length() != sizeof(SampleDataStart))
    {
        return false;
    }
    else
    {
        memcpy(this, byteArray.data(), byteArray.length());
    }

	return true;
}

SampleDataData::SampleDataData()
	: cmd_id(SAMPLE_DATA_DATA)
	, rp_id(-1)
	, dim23456_index(0)
	, rec(0)
	, coeff(1)
{}

bool SampleDataData::Pack( QByteArray &byteArray )
{
    byteArray = QByteArray::fromRawData((char*)this, sizeof(SampleDataData));
    return true;
}

bool SampleDataData::Unpack( QByteArray &byteArray )
{
    if(byteArray.length() != sizeof(SampleDataData))
    {
        return false;
    }
    else
    {
        memcpy(this, byteArray.data(), byteArray.length());
    }

    return true;
}

SampleDataEnd::SampleDataEnd( uint32_t code )
	: cmd_id(SAMPLE_DATA_END)
	, error_code(code)
{}
bool SampleDataEnd::Pack( QByteArray &byteArray )
{
    byteArray = QByteArray::fromRawData((char*)this, sizeof(SampleDataEnd));
    return true;
}

bool SampleDataEnd::Unpack( QByteArray &byteArray )
{
    if(byteArray.length() != sizeof(SampleDataEnd))
    {
        return false;
    }
    else
    {
        memcpy(this, byteArray.data(), byteArray.length());
    }

    return true;
}




bool IntAndFloatArray::Pack(QByteArray &byteArray)
{

    QByteArray data1 = QByteArray::fromRawData((char*)(&flag), sizeof(int));

    QByteArray data2 = QByteArray::fromRawData((char*)(data.data()),sizeof(float)* data.size());
    byteArray = data1 + data2;
    return true;

}

bool IntAndFloatArray::Unpack(QByteArray &byteArray)
{
    //
    int byteIndex = 0;

    int len = byteArray.length();
    this->flag = *(int*)(byteArray.data() + byteIndex);
    byteIndex += sizeof(this->flag);

    int count = (len - byteIndex)/ sizeof(float);

    this->data.resize(count);

    if( (len - byteIndex) % sizeof(float) != 0)
    {
        return false;
    }
    else
    {
        memcpy(this->data.data(), (char*)byteArray.data() + byteIndex, byteArray.length());
        return true;
    }


}


void IntAndFloatArray::CreateDemoStruct()
{

    data.resize(98);
    for(unsigned int i = 0; i < data.size(); i ++)
    {
        data[i] = float(i)* 1.0;
    }


}
