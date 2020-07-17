#include "ReceiveData.h"

#include "Implement/DataObject.h"
#include "Implement/LogUserImpl.h"
#include "Client/DataHelper.h"
#include "Rawdata.h"
#include "QtUtilities\commonmethod.h"
#include "QtUtilities\DataWatch.h"
#include <QDebug>

using namespace Yap;
using namespace std;
using namespace Databin;

const bool cnstTEST = true;

ReceiveData::ReceiveData() : ProcessorImpl(L"ReceiveData")
{
    LOG_TRACE(L"<ReceiveData> constructor called.", L"ReconDemo");
    AddInput(L"Input",   YAP_ANY_DIMENSION, DataTypeFloat | DataTypeComplexFloat);
    AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeFloat | DataTypeComplexFloat);


}

ReceiveData::ReceiveData(const ReceiveData &rhs) :  ProcessorImpl(rhs)
{
    LOG_TRACE(L"<ReceiveData> copy constructor called.", L"ReconDemo");
}

bool ReceiveData::Input(const wchar_t * port, IData *data)
{
    //return true;

    if (wstring(port) != L"Input")
        return false;

    assert(data);
    assert(data->GetVariables() != nullptr);
    VariableSpace variables(data->GetVariables());
    int scan_signal = variables.Get<int>(L"scan_signal");
    switch(scan_signal)
    {
    case 1:
    {
        //Threadfunc_running();
        //if(!_thread_running)
        //    _thread_running = std::shared_ptr<std::thread>(
        //                new std::thread(std::bind(&ReceiveData::Threadfunc_running, this)));
    }
        break;
    case 2:
    {
        int phase_index = variables.Get<int>(L"phase_index");

        qDebug()<<"---------scanning phase step =" <<phase_index << "**********";
    }
        break;
    case 3:
    {

        return true;
    }
        break;
    default:
        break;
    }

}

void ReceiveData::Threadfunc_running()
{
    //---------
    for(int index = 0; index < RawData::GetHandle().MaxChannelIndex(); index ++)
    {
        if(RawData::GetHandle().NeedProcess(index))
        {
            int freq_count;
            int phase_count;
            int slice_count;
            std::complex<float>* channel_data =
                RawData::GetHandle().NewChanneldata(index, freq_count, phase_count, slice_count);

            CreateOutData(channel_data, index, freq_count, phase_count, slice_count);
        }
    }

    return;

}



bool ReceiveData::IsFinished(Yap::IData *data) const
{
    //
    //assert(data->GetVariables() != nullptr);
    //VariableSpace variables(data->GetVariables());
    //bool finished = variables.Get<bool>(L"Finished");
    //return finished;
    return true;

}

bool ReceiveData::CreateOutData(std::complex<float>* channel_data, int channel_index,
                                int freq_count, int phase_count, int slice_count)
{
    unsigned int width = freq_count;
    unsigned int height = phase_count;
    //slice_count;
    unsigned int dim4 = 1;
    Dimensions dimensions;
    dimensions(DimensionReadout, 0U, width)
        (DimensionPhaseEncoding, 0U, height)
        (DimensionSlice, 0U, slice_count)
        (Dimension4, 0U, dim4)
        (DimensionChannel, channel_index, 1);
    //
    auto ref_data = CreateRefIData();
    auto output = CreateData<complex<float>>(ref_data.get(), channel_data, dimensions);
    Feed(L"Output", output.get());
    LOG_TRACE(L"<ReceiveData> Feed channeldata.", L"ReconDemo");
    //
    //Log
    wstringstream wss;
    wss<<L"-------Feed : channel index == "<<channel_index;
    wstring ws ;
    ws=wss.str(); //»ò ss>>strtEST;
    LOG_TRACE(ws.c_str(), L"ReconDemo");
    //Out the specified channel rawdata.
    //

    if(false){
        for(int i=0; i <slice_count; i++)
        {
            wstringstream wss;
            wss<<L"d:\\temp";
            wstring wpath = wss.str();
            CDataWatch::ExportData(wpath.c_str(),
                                   channel_data + channel_index*freq_count*phase_count*slice_count,
                                   256, 256, i);
        }
    }
    //

    return true;
}


Yap::SmartPtr<Yap::IData> ReceiveData::CreateRefIData()
{
    return Yap::SmartPtr<Yap::IData>();
}

ReceiveData::~ReceiveData()
{
    //LOG_TRACE(L"<ReceiveData>  destructor called.", L"ReconDemo");
}
