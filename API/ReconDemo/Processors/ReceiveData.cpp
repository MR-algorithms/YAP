#include "ReceiveData.h"

#include "Implement/DataObject.h"
#include "Implement/LogUserImpl.h"
#include "Client/DataHelper.h"
#include "Rawdata.h"
#include "QtUtilities\commonmethod.h"
#include "QtUtilities\DataWatch.h"

using namespace Yap;
using namespace std;
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

    _ref_data = data;

    StoreData(data);

    if(IsFinished(data))
    {
        ProcessData();

    }

    return true;

}

bool ReceiveData::StoreData(Yap::IData *data)
{
    if(!IsFinished(data))
    {
        if(!_rawdata.Ready())
        {
            InitScanData(data);
        }

        InsertPhasedata(data);
    }

    //Start thread if the processing thread does not running;
    //Send stop event if necessary.
    return true;
}

bool ReceiveData::ProcessData()
{
    for(int index = 0; index < _rawdata.MaxChannelCount(); index ++)
    {
        if(_rawdata.NeedProcess(index))
        {
            int freq_count;
            int phase_count;
            int slice_count;
            std::complex<float>* channel_data =
                    _rawdata.NewChanneldata(index, freq_count, phase_count, slice_count);

            CreateOutData(channel_data, index, freq_count, phase_count, slice_count);
        }
    }

    return true;
}

bool ReceiveData::IsFinished(Yap::IData *data) const
{
    //
    assert(data->GetVariables() != nullptr);

    VariableSpace variables(data->GetVariables());

    bool finished = variables.Get<bool>(L"Finished");
    return finished;

}


 bool ReceiveData::InitScanData(Yap::IData *data)
 {

    assert(data->GetVariables() != nullptr);

    VariableSpace variables(data->GetVariables());

    int freq_count = variables.Get<int>(L"freq_count");
    int phase_count = variables.Get<int>(L"phase_count");
    int slice_count = variables.Get<int>(L"slice_count");
    int channel_switch = variables.Get<int>(L"channel_switch");

    //Check if parameters in variables conform to the demension in data.

    _rawdata.Prepare(freq_count, phase_count, slice_count, channel_switch);

    return true;
 }

bool ReceiveData::InsertPhasedata(Yap::IData *data)
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
    _rawdata.InsertPhasedata(source, length, channel_index, slice_index, phase_index);

    wstringstream wss;
    wss<<L"-------Feed : phase index == "<<phase_index
      << L", channel_index == " << channel_index
      << L", slice_index == " << slice_index;

    wstring ws ;
    ws=wss.str(); //»ò ss>>strtEST;
    LOG_TRACE(ws.c_str(), L"ReconDemo");

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
    //
    return Yap::SmartPtr<Yap::IData>(_ref_data);
}
ReceiveData::~ReceiveData()
{
    //LOG_TRACE(L"<ReceiveData>  destructor called.", L"ReconDemo");
}
