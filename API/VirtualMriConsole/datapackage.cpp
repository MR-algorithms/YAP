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
    headitem.size = size;

    head.push_back(headitem);//值拷贝？

    //
    DataItem dataitem;
    dataitem.data.resize(size);
    //for(int i = 0; i < size; i ++)
    //{
    //    dataitem.data[i] = value[i];
    //}

    memcpy(dataitem.data.data(), value, size);

    data.push_back(dataitem);//值拷贝？

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
            start.channel_mask };

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


    //std::vector<std::complex<float>> data;

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
                    reinterpret_cast<const char*>(data.data.data()),
                    sizeof(std::complex<float>) * data.data.size() );


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
    //包检查
    uint32_t info[11];
    for(int i = 0; i < static_cast<int>( package.data.size() ); i ++)
    {
        DataItem dataitem = package.data[i];
        memcpy(reinterpret_cast<char*>(&info[i]), dataitem.data.data(), dataitem.data.size());
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
    start.channel_mask   = info[10];
    return true;

}
bool MessageProcess::Unpack(const DataPackage &package, SampleDataData  &data)
{
    //包自完备检查--回波数据长度除以复数类型大小是否复数数据点数，是否等于Frequence_Points.

    assert(package.data.size());
    uint32_t info[4];
    for(int i = 0; i < 4; i ++)
    {
        DataItem dataitem = package.data[i];
        memcpy(reinterpret_cast<char*>(&info[i]), dataitem.data.data(), dataitem.data.size());
    }

    data.cmd_id     = info[0];
    data.rp_id      = info[1];
    data.dim23456_index   = info[2];
    data.rec        = info[3];

    //
    DataItem dataitem1 = package.data[4];
    memcpy(reinterpret_cast<char*>(&data.coeff), dataitem1.data.data(), dataitem1.data.size());
    //


    DataItem dataitem2 = package.data[5];
    int complex_points = dataitem2.data.size() / sizeof(std::complex<float>);

    data.data.resize( complex_points);
    memcpy(data.data.data(), dataitem2.data.data(), dataitem2.data.size());


    return true;

}
bool MessageProcess::Unpack(const DataPackage &package, SampleDataEnd   &end)
{
    //包检查
    uint32_t info[2];
    for(int i = 0; i < static_cast<int>( package.data.size() ); i ++)
    {
        DataItem dataitem = package.data[i];
        memcpy(reinterpret_cast<char*>(&info[i]), dataitem.data.data(), dataitem.data.size());
    }

    end.cmd_id    = info[0];
    end.error_code   = info[1];
    return true;

}
