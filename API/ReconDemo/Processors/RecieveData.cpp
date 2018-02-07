#include "RecieveData.h"

#include "Implement/DataObject.h"
#include "Client/DataHelper.h"

using namespace Yap;
using namespace std;

RecieveData::RecieveData() : ProcessorImpl(L"RecieveData"),_data(nullptr)
{

    AddInput(L"Input",   YAP_ANY_DIMENSION, DataTypeFloat | DataTypeComplexFloat);
    AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeFloat | DataTypeComplexFloat);
}


RecieveData::RecieveData(const RecieveData &rhs) :  ProcessorImpl(rhs)
{
    //LOG_TRACE(L"NiuMriDisplay2D constructor called.", L"NiuMri");
}

bool RecieveData::Input(const wchar_t * port, IData *data)
{
    if (wstring(port) != L"Input")
        return false;

    //
    if(!IsFinished(data))
    {
        if(_data.get() == nullptr)
        {
            InitScanData(data);
        }
        else
        {
            InsertPhasedata(data);
        }

    }

    auto output = CreateOutData();

    return Feed(L"Output", output.get());
}


bool RecieveData::IsFinished(Yap::IData *data)
{
    //
    assert(data->GetVariables() != nullptr);

    VariableSpace variables(data->GetVariables());

    bool finished = variables.Get<bool>(L"Finished");
    return finished;

}


 bool RecieveData::InitScanData(Yap::IData *data)
 {

     assert(data->GetVariables() != nullptr);

     VariableSpace variables(data->GetVariables());

     if(_data.get() == nullptr)
     {
         _dataInfo.freq_count = variables.Get<int>(L"freq_count");
         _dataInfo.phase_count = variables.Get<int>(L"phase_count");
         _dataInfo.slice_count = variables.Get<int>(L"slice_count");
         _dataInfo.dim4 = variables.Get<int>(L"dim4");
         _dataInfo.dim5 = variables.Get<int>(L"dim5");
         _dataInfo.dim6 = variables.Get<int>(L"dim6");
         _dataInfo.channel_mask = variables.Get<int>(L"channel_mask");

          int channel_count = GetChannelCountInData(_dataInfo.channel_mask);
         _data = std::shared_ptr< std::complex<float> >(
                     new std::complex<float>[_dataInfo.freq_count * _dataInfo.phase_count * _dataInfo.slice_count * channel_count]);

     }
     return true;
 }

int RecieveData::GetChannelIndexInData(unsigned int channelMask, int channelIndex)
{
    int channelIndexInData = -1;
    for(int i = 0; i < channelIndex; i ++)
    {
        if (channelMask & (1 << i))
        {
            channelIndexInData ++;
        }

    }

    return channelIndexInData;

}
int RecieveData::GetChannelCountInData(unsigned int channelMask)
{
    int count_max = sizeof(unsigned int);
    int countInData = 0;
    for(int channel_index = 0; channel_index < count_max; channel_index ++)
    {
        if (channelMask & (1 << channel_index))
        {
            countInData ++;
        }

    }

    return countInData;
}

bool RecieveData::InsertPhasedata(Yap::IData *data)
{

    assert(data->GetVariables() != nullptr);

    VariableSpace variables(data->GetVariables());

    int channel_index = variables.Get<int>(L"channel_index");
    int slice_index = variables.Get<int>(L"slice_index");
    int phase_index = variables.Get<int>(L"phase_index");

    int channelIndexInData = GetChannelIndexInData(_dataInfo.channel_mask, channel_index);

    DataHelper input_data(data);
    assert(input_data.GetActualDimensionCount() == 1);
    assert(_dataInfo.freq_count == input_data.GetWidth() );


    auto phase_data = std::shared_ptr< std::complex<float>> (
                                      GetDataArray<std::complex<float>>(data) );
    InsertPhasedata(phase_data.get(), channelIndexInData, slice_index, phase_index);

    return true;
}
bool RecieveData::InsertPhasedata(std::complex<float> *data, int channel_index, int slice_index, int phase_index)
{
    return true;
}

RecieveData::~RecieveData()
{
//    LOG_TRACE(L"NiuMriDisplay2D destructor called.", L"NiuMri");
}
