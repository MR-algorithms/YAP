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

bool SampleDataStart::Pack( QByteArray &bytearray )
{
    bytearray = QByteArray::fromRawData((char*)this, sizeof(SampleDataStart));
	return true;
}

bool SampleDataStart::Unpack( QByteArray &bytearray )
{
    if(bytearray.length() != sizeof(SampleDataStart))
    {
        return false;
    }
    else
    {
        memcpy(this, bytearray.data(), bytearray.length());
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

bool SampleDataData::Pack( QByteArray &bytearray )
{
    bytearray = QByteArray::fromRawData((char*)this, sizeof(SampleDataData));
    return true;
}

bool SampleDataData::Unpack( QByteArray &bytearray )
{
    if(bytearray.length() != sizeof(SampleDataData))
    {
        return false;
    }
    else
    {
        memcpy(this, bytearray.data(), bytearray.length());
    }

    return true;
}

SampleDataEnd::SampleDataEnd( uint32_t code )
	: cmd_id(SAMPLE_DATA_END)
	, error_code(code)
{}
bool SampleDataEnd::Pack( QByteArray &bytearray )
{
    bytearray = QByteArray::fromRawData((char*)this, sizeof(SampleDataEnd));
    return true;
}

bool SampleDataEnd::Unpack( QByteArray &bytearray )
{
    if(bytearray.length() != sizeof(SampleDataEnd))
    {
        return false;
    }
    else
    {
        memcpy(this, bytearray.data(), bytearray.length());
    }

    return true;
}


void ConvertToMystruct(QByteArray &dataArray, IntAndFloatArray &mystruct)
{
    IntAndFloatArray *test4;
    test4 = (IntAndFloatArray*)dataArray.data();
    //
    char *chmsg = dataArray.data();

    int x = dataArray.length();
    int y = sizeof(IntAndFloatArray);

    if(dataArray.length() != sizeof(IntAndFloatArray))
    {
        return ;
    }
    else
    {
        memcpy(&mystruct, dataArray.data(), dataArray.length());
    }

}
