#include "datapackage.h"
#include "CmrPackType.h"
#include "CmrPackItemTypeHelper.h"
#include <array>
#include <assert.h>

void DataPackage::AddItem(uint16_t data_type, uint16_t second_type, const char* value, int size)
{
    HeadItem headitem;
    headitem.primary_type = data_type;
    headitem.second_type  = second_type;
    headitem.value_size = size;

    headitems.push_back(headitem);

    //
    ValueItem valueitem;
    valueitem.value.resize(size);
    //for(int i = 0; i < size; i ++)
    //{
    //    valueitem.data[i] = value[i];
    //}

    memcpy(valueitem.value.data(), value, size);

    valueitems.push_back(valueitem);

}

int DataPackage::BytesFromHeaditems() const
{
    int bytes = 0;
    //"head.size()" is the count of headitmes in "head".
    for(int i = 0; i < static_cast< int >( headitems.size()); i ++)
    {
        //head[i].size is the member varible "size" of the one element "head[i]"(headitem), rather that the size of one element (headitem).
        bytes += headitems[i].value_size;
    }
    return bytes;
}

int DataPackage::BytesFromValueitems() const
{
    int bytes = 0;
    //data.size(): the count of dataitems in "data".
    for(int i = 0; i < static_cast<int>( valueitems.size() ); i ++)
    {

        //vector<item>.size: the count of items;
        //Aitem.size: the member variable "size" of the item(Aitem). It happened that item has a member variable "size".
        //So "size" is not a proper name for the member variable.
        bytes += valueitems[i].value.size();
    }
    return bytes;

}

void DataPackage::CheckSelf(bool checkHead, bool checkCmdid, bool checkData)
{
    //
    if(checkHead)
    {
        int headitem_count = magic_anditem_count[1];
        assert( headitems.size() == headitem_count );

    }

    //
    if(checkCmdid)
    {
        int cmd_id;
        assert(valueitems[0].value.size() == sizeof(uint32_t));
        memcpy(&cmd_id, valueitems[0].value.data(), sizeof(uint32_t));

        switch(cmd_id)
        {
        case SAMPLE_DATA_START:
            assert(headitems.size() == 11);
            break;
        case SAMPLE_DATA_DATA:
            assert(headitems.size() == 6);
            break;
        case SAMPLE_DATA_END:
            assert(headitems.size() == 2);
            break;
        default:
            break;
        }

    }

    //
    if(checkData)
    {
        assert( BytesFromValueitems() == BytesFromHeaditems());
    }

}

bool MessageProcess::Pack(DataPackage &package, const SampleDataStart &start)
{
    std::array<uint32_t, 11> info = {
            start.cmd_id,
            start.version,
            start.scan_id,
            start.dim23456_size,
            start.dim1_size,
            start.dim2_size,
            start.dim3_size,
            start.dim4_size,
            start.dim5_size,
            start.dim6_size,
            start.channel_switch };

    package.magic_anditem_count[0] = 0xFFFFFFFF;
    package.magic_anditem_count[1] = info.size();

    for(int i =0; i < static_cast<int>( info.size() ); i ++)
    {
        char* value = reinterpret_cast<char*>( &info[i] );
        package.AddItem(cmr::PackItemTypeHelper<uint32_t>::value,
                        mstValue,
                        const_cast<char*>( value ),
                        sizeof(uint32_t));
    }
    return true;
}
bool MessageProcess::Pack(DataPackage &package, const SampleDataData &data)
{



    package.magic_anditem_count[0] = 0xFFFFFFFF;
    package.magic_anditem_count[1] = 6;


    package.AddItem(cmr::PackItemTypeHelper<uint32_t>::value,
                    mstValue,
                    reinterpret_cast<const char*>( &data.cmd_id ),
                    sizeof(uint32_t));

    package.AddItem(cmr::PackItemTypeHelper<uint32_t>::value,
                    mstValue,
                    reinterpret_cast<const char*>( &data.rp_id ),
                    sizeof(uint32_t));


    package.AddItem(cmr::PackItemTypeHelper<uint32_t>::value,
                    mstValue,
                    reinterpret_cast<const char*>( &data.dim23456_index ),
                    sizeof(uint32_t));

    package.AddItem(cmr::PackItemTypeHelper<uint32_t>::value,
                    mstValue,
                    reinterpret_cast<const char*>( &data.rec ),
                    sizeof(uint32_t));

    package.AddItem(cmr::PackItemTypeHelper<float>::value,
                    mstValue,
                    reinterpret_cast<const char*>( &data.coeff ),
                    sizeof(float));

    //std::vector<std::complex<float>> data;

    //assure the data size is same as the frequence_points in sampledataStart.

    package.AddItem(cmr::PackItemTypeHelper<std::complex<float>>::value,
                    mstArray,
                    reinterpret_cast<const char*>(data.data_value.data()),
                    sizeof(std::complex<float>) * data.data_value.size() );


    return true;

}
bool MessageProcess::Pack(DataPackage &package, const SampleDataEnd &end)
{
    std::array<uint32_t, 2> info = {
            end.cmd_id,
            end.error_code,
           };

    //AddItem(PackItemTypeHelper<T>::value, mstValue, (char*)(&value), sizeof(T)))；
    package.magic_anditem_count[0] = 0xFFFFFFFF;
    package.magic_anditem_count[1] = info.size();

    for(int i =0; i < static_cast<int>( info.size()); i ++)
    {
        char* value = reinterpret_cast<char*>( &info[i] );

        package.AddItem(cmr::PackItemTypeHelper<uint32_t>::value,
                        mstValue,
                        const_cast<char*>( value ),
                        sizeof(uint32_t));
    }
    return true;
}
bool MessageProcess::Unpack(const DataPackage &package, SampleDataStart &start)
{
    uint32_t info[11];
    for(int i = 0; i < static_cast<int>( package.valueitems.size() ); i ++)
    {
        ValueItem valueitem = package.valueitems[i];
        memcpy(reinterpret_cast<char*>(&info[i]), valueitem.value.data(), valueitem.value.size());
    }

    start.cmd_id    = info[0];
    start.version   = info[1];
    start.scan_id   = info[2];
    start.dim23456_size   = info[3];
    start.dim1_size   = info[4];
    start.dim2_size   = info[5];
    start.dim3_size   = info[6];
    start.dim4_size   = info[7];
    start.dim5_size   = info[8];
    start.dim6_size   = info[9];
    start.channel_switch   = info[10];
    return true;

}
bool MessageProcess::Unpack(const DataPackage &package, SampleDataData  &data)
{
    //包自完备检查--回波数据长度除以复数类型大小是否复数数据点数，是否等于Frequence_Points.

    assert(package.valueitems.size());
    uint32_t info[4];
    for(int i = 0; i < 4; i ++)
    {
        ValueItem valueitem = package.valueitems[i];
        memcpy(reinterpret_cast<char*>(&info[i]), valueitem.value.data(), valueitem.value.size());
    }

    data.cmd_id     = info[0];
    data.rp_id      = info[1];
    data.dim23456_index   = info[2];
    data.rec        = info[3];

    //
    ValueItem valueitem1 = package.valueitems[4];
    memcpy(reinterpret_cast<char*>(&data.coeff), valueitem1.value.data(), valueitem1.value.size());
    //


    ValueItem valueitem2 = package.valueitems[5];
    int complex_points = valueitem2.value.size() / sizeof(std::complex<float>);

    data.data_value.resize( complex_points);
    memcpy(data.data_value.data(), valueitem2.value.data(), valueitem2.value.size());


    return true;

}
bool MessageProcess::Unpack(const DataPackage &package, SampleDataEnd   &end)
{
    uint32_t info[2];
    for(int i = 0; i < static_cast<int>( package.valueitems.size() ); i ++)
    {
        ValueItem valueitem = package.valueitems[i];
        memcpy(reinterpret_cast<char*>(&info[i]), valueitem.value.data(), valueitem.value.size());
    }

    end.cmd_id    = info[0];
    end.error_code   = info[1];
    return true;

}
