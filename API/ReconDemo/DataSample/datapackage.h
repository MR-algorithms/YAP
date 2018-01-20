#ifndef DATAPACKAGE_H
#define DATAPACKAGE_H
#include <qbytearray.h>
#include "SampleDataProtocol.h"

//--copied from All::CmrPack.h
// item format
// |--type--||--type2-||buffer_size|
// |--2Byte-||--2Byte-||--4Byte----|

// pack format
// |--item1-||--item2-||--item3-||--item4-||buffer|
// |--8Byte-||--8Byte-||--8Byte-||--8Byte-||......|
//end of copy.

//reference: CmrPack.h, MsgPackbuffer.h, MsgPackBufferSocketIO.h

struct HeadItem //MsgPackBufferSocketIO
{
    uint16_t primary_type;
    uint16_t second_type;
    uint32_t size; // sizeof(T) * element_count
};

struct DataItem
{
    std::vector<char> data;
};

//
//1，4个字节的unsigned int- uint32_t "0xFFFFFFFF"
//2, 4个字节的unsigned int - uint32_t: item的个数
//3, 若干个item，总字节数 = item个数 x sizeof(HeadItem）

struct DataPackage
{
    uint32_t magic_anditem_count[2];
    std::vector<HeadItem> head;
    std::vector<DataItem> data;

    void AddItem(uint16_t data_type, uint16_t second_type, const char* value, int size);
    int BytesFromHeaditem();
    int BytesFromDataitem();
    void CheckSelf(bool checkHead = true, bool checkCmdid = true, bool checkData = true);
};

class MessageProcess
{

public:
    MessageProcess(){}
    static bool Pack(DataPackage &package, const SampleDataStart &start);
    static bool Pack(DataPackage &package, const SampleDataData  &data);
    static bool Pack(DataPackage &package, const SampleDataEnd   &end);
    static bool Unpack(const DataPackage &package, SampleDataStart &start);
    static bool Unpack(const DataPackage &package,  SampleDataData  &data);
    static bool Unpack(const DataPackage &package,  SampleDataEnd   &end);


};



#endif // DATAPACKAGE_H
