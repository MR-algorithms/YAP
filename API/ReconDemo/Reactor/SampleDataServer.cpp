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

#include <QEvent>
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
bool SampleDataServer::OnDataServer(DataPackage &package, int cmd_id)
{
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
    if(!RawData::GetHandle().Ready())
    {
        _sample_start = start;
        int channel_switch = _sample_start.channel_switch;
        int scan_id = _sample_start.scan_id;
        int freq_count = _sample_start.dim1_size;
        int phase_count = _sample_start.dim2_size;
        int slice_count = _sample_start.dim3_size;
        RawData::GetHandle().Prepare(scan_id, freq_count, phase_count, slice_count, channel_switch);

        _observer->OnDataBegin(start.scan_id);
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

    assert(RawData::GetHandle().PhaseCount()==data_size);

    std::complex<float> * data_vector2 = new std::complex<float>[data_size];

    for(int i = 0; i < data_size; i ++)
    {
      data_vector2[i] = data.data_value[i];
    }
    RawData::GetHandle().InsertPhasedata(data_vector2, data_size, channel_index, slice_index, phase_index);
    //-
    _observer->OnDataData(_sample_start.scan_id, phase);
    return true;
}

bool SampleDataServer::OnDataEnd(SampleDataEnd &end)//cmr::MsgUnpack& msg_pack, TransferInfo& info)
{
    _observer->OnDataEnd(_sample_start.scan_id);
    return true;
}

/*
bool SampleDataServer::InsertPhasedata(Yap::IData *data)
{
   assert(data->GetVariables() != nullptr);
   VariableSpace variables(data->GetVariables());
   int channel_index = variables.Get<int>(L"channel_index");
   int slice_index = variables.Get<int>(L"slice_index");
   int phase_index = variables.Get<int>(L"phase_index");

   DataHelper helpler(data);
   assert(helpler.GetActualDimensionCount() == 1);
   assert(helpler.GetDataType() == DataTypeComplexFloat);
   auto source = GetDataArray<std::complex<float>>(data);
   int length = helpler.GetWidth();

   //if(cnstTEST)
      // assert(!CommonMethod::IsComplexelementZero<std::complex<float>>(source, length) );
   RawData::GetHandle().InsertPhasedata(source, length, channel_index, slice_index, phase_index);

   wstringstream wss;
   wss<<L"-------Feed : phase index == "<<phase_index
     << L", channel_index == " << channel_index
     << L", slice_index == " << slice_index;

   wstring ws ;
   ws=wss.str(); //»ò ss>>strtEST;
   LOG_TRACE(ws.c_str(), L"ReconDemo");

   return true;
}
*/
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

void SampleDataServer::SetObserver(std::shared_ptr<SampleDataObserver> observer)
{
    //boost::unique_lock<boost::mutex> lk(_state_mutex);
    _observer = observer;
}

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

/*
    while(true)
    {
        std::wstringstream ss;
        ss<<L"---io.read()----"<<debug_count++;
        std::wstring debug_info;
        debug_info = ss.str();
        //DebugInfo::Output(L"SampleDataClient::RunSampleDataClientSession()", debug_info.c_str(),
        //                  reinterpret_cast<int>(this), false, DebugInfo::accept_flow);
        //
        if(false == io.Read(_socket, msg_bufferr))
        {
            DebugInfo::Output(L"SampleDataClient::RunSampleDataClientSession()",
                              L"Test accept0",
                              reinterpret_cast<int>(this), false, DebugInfo::accept_flow);

            LOG_ERROR(L"Data Client IO Read error.");
            OnError();
            break;
        }
        uint32_t cmd_id;
        cmr::MsgUnpack unpack(&msg_bufferr);
        if(mpeSuccess != unpack.Convert(0, cmd_id))
        {
            LOG_ERROR(L"Convert Header Error.");
            OnError();

            DebugInfo::Output(L"SampleDataClient::RunSampleDataClientSession()",
                              L"Test accept1",
                              reinterpret_cast<int>(this), false, DebugInfo::accept_flow);

            break;
        }

        if(SAMPLE_DATA_START == cmd_id)
        {
            {
                boost::unique_lock<boost::mutex> lk(_state_mutex);
                _current_scan_observer = _observer;
            }
            DebugInfo::Output(L"SampleDataClient::RunSampleDataClientSession()", L"calling OnDataStart()...",
                              reinterpret_cast<int>(this), false);
            if(!OnDataStart(unpack, info))
            {
                LOG_ERROR(L"OnDataStart Error.");
                OnError();
                break;
            }
        }
        else if(SAMPLE_DATA_DATA == cmd_id)
        {
            DebugInfo::Output(L"SampleDataClient::RunSampleDataClientSession()", L"calling OnDataData()...",
                              reinterpret_cast<int>(this), false);
            if (!OnDataData(unpack, info))
            {
                LOG_ERROR(L"OnDataData Error.");
                OnError();
                break;
            }
        }
        else if(SAMPLE_DATA_END == cmd_id)
        {
            DebugInfo::Output(L"SampleDataClient::RunSampleDataClientSession()", L"calling OnDataEnd()...",
                              reinterpret_cast<int>(this), false);
            if(!OnDataEnd(unpack, info))
            {
                LOG_ERROR(L"OnDataEnd Error.");
                OnError();
                break;
            }
        }
        else if(SAMPLE_DATA_HEARTBEAT == cmd_id)
        {

            DebugInfo::Output(L"SampleDataClient::RunSampleDataClientSession()",
                              debug_info.c_str(),
                              reinterpret_cast<int>(this), false, DebugInfo::accept_flow);

        }
        else
        {
            LOG_ERROR(L"Unknown Msgr.");
            OnError();
            DebugInfo::Output(L"SampleDataClient::RunSampleDataClientSession()",
                              L"Test accept3",
                              reinterpret_cast<int>(this), false, DebugInfo::accept_flow);

            break;
        }
    }
    */
}



