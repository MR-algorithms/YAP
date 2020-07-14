#include <cassert>
#include "StringHelper.h"
#include "Implement/DataObject.h"
#include "QtUtilities\commonmethod.h"
#include "SampleDataServer.h"
#include "QtUtilities/FormatString.h"
#include "Processors/Rawdata.h"
#include <algorithm>
#include <qmessagebox>
#include <io.h>
#include <direct.h>
#include <fstream>
#include <QDateTime>
#include <QFileInfo>
#include "Client/DataHelper.h"

#include <QApplication>

using namespace std;
using namespace Yap;
using namespace Databin;

SampleDataServer SampleDataServer::s_instance;

SampleDataServer::SampleDataServer()
{

}
SampleDataServer& SampleDataServer::GetHandle()
{
    return s_instance;
}
SampleDataServer::~SampleDataServer()
{
}
bool SampleDataServer::OnDataParsing(DataPackage &package, int cmd_id)
{
    DebugInfo::Output(L"SampleDataServer::OnDataParsing()", L"Enter ...",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type2);
    switch(cmd_id)
    {
    case SAMPLE_DATA_START:
    {
        SampleDataStart start;
        MessageProcess::Unpack(package, start);
        OnDataStart(start);

    }
        break;
    case SAMPLE_DATA_DATA:
    {
        SampleDataData data;
        MessageProcess::Unpack(package, data);
        OnDataData(data);

    }
        break;
    case SAMPLE_DATA_END:
    {
        SampleDataEnd end;
        MessageProcess::Unpack(package, end);
        OnDataEnd(end);

    }
        break;
    default:
        break;
    }
    return true;
}

bool SampleDataServer::OnDataStart(SampleDataStart &start)//cmr::MsgUnpack& msg_pack, TransferInfo& info)
{
    DebugInfo::Output(L"SampleDataServer::OnDataStart()", L"Enter ...",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type2);

    if(!RawData::GetHandle().Ready())
    {
        _sample_start = start;
        int channel_switch = _sample_start.channel_switch;
        int scan_id = _sample_start.scan_id;
        int freq_count = _sample_start.dim1_size;
        int phase_count = _sample_start.dim2_size;
        int slice_count = _sample_start.dim3_size;
        RawData::GetHandle().Prepare(scan_id, freq_count, phase_count, slice_count, channel_switch);

    }
    return true;
}

bool SampleDataServer::OnDataData(SampleDataData &data)//cmr::MsgUnpack& msg_pack, TransferInfo& info)
{
    //
    int channel_switch = _sample_start.channel_switch;
    int channel_index = data.rec;
    int phase_index, slice_index;
    calculate_dimindex(_sample_start, data.dim23456_index, phase_index, slice_index);

    //
    int data_size = data.data_value.size();

    const ChannelData& channel = RawData::GetHandle().GetChannelData(channel_index);
    assert(channel.phase_count==data_size);

    std::complex<float> * data_vector2 = new std::complex<float>[data_size];

    for(int i = 0; i < data_size; i ++)
    {
      data_vector2[i] = data.data_value[i];
    }
    RawData::GetHandle().InsertPhasedata(data_vector2, data_size, channel_index, slice_index, phase_index);
    //-
    return true;
}

bool SampleDataServer::OnDataEnd(SampleDataEnd &end)//cmr::MsgUnpack& msg_pack, TransferInfo& info)
{
    return true;
}

void SampleDataServer::calculate_dimindex(SampleDataStart &start, int dim23456_index, int &phase_index, int &slice_index)
{

    //sending end: dim2 = phase_count, dim3 = slice_count;
    //dim4 maybe echo images or other else.
    int phase_count = start.dim2_size;
    int slice_count = start.dim3_size;
    int dim4_size = start.dim4_size;
    int dim5_size = start.dim5_size;
    int dim6_size = start.dim6_size;
    int dim23456_size = start.dim23456_size;


    assert(dim4_size == 1);
    assert(dim5_size == 1);
    assert(dim6_size == 1);
    assert(phase_count * slice_count == dim23456_size);

    //Decode the order of data received, according to the encoded order in the sender.

    CommonMethod::split_index(dim23456_index, phase_count, slice_count, phase_index, slice_index);

}
/*
bool SampleDataServer::Run()
{
    if(_thread)
    {
        return false;
    }
    DebugInfo::Output(L"SampleDataServer::Run()", L"bind SampleDataServer::ThreadFunction thread function",
                      reinterpret_cast<int>(this), true);

    _thread = std::shared_ptr<std::thread>(new std::thread(std::bind(&SampleDataServer::ThreadFunction, this)));
    return true;

}


void SampleDataServer::ExitThread()
{
    if (_thread)
    {
        if(_thread->joinable())
        {
            //_thread->interrupt();
            _thread->join();
            _thread.reset();
        }
    }
}


void SampleDataServer::ThreadFunction()
{
    DebugInfo::Output(L"SampleDataServer:ThreadFunction()", L"Enter...",
                      reinterpret_cast<int>(this), true);
}

*/

