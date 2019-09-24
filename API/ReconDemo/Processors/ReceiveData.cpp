#include "ReceiveData.h"

#include "Implement/DataObject.h"
#include "Client/DataHelper.h"

using namespace Yap;
using namespace std;

ReceiveData::ReceiveData() : ProcessorImpl(L"ReceiveData"),_data(nullptr)
{

    AddInput(L"Input",   YAP_ANY_DIMENSION, DataTypeFloat | DataTypeComplexFloat);
    AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeFloat | DataTypeComplexFloat);
    //_datav.clear();
    TestWhatcanbeTested();
}

ReceiveData::ReceiveData(const ReceiveData &rhs) :  ProcessorImpl(rhs)
{
    //LOG_TRACE(L"NiuMriDisplay2D constructor called.", L"NiuMri");
}

void ReceiveData::TestWhatcanbeTested() const
{
    //通道数跟mask的关系
    unsigned int mask1 = 0x35;//4个通道；0011,0101
    unsigned int mask2 = 0xa1;//3个通道；1010,0001
    assert(this->GetChannelCountUsed(mask1) == 4);
    assert(this->GetChannelCountUsed(mask2) == 3);
    assert(this->GetChannelMaskindex(mask1, 0) == 0);
    assert(this->GetChannelMaskindex(mask1, 1) == -1);
    assert(this->GetChannelMaskindex(mask1, 2) == 1);
    assert(this->GetChannelMaskindex(mask1, 3) == -1);
    assert(this->GetChannelMaskindex(mask1, 4) == 2);
    assert(this->GetChannelMaskindex(mask1, 5) == 3);



}
bool ReceiveData::Input(const wchar_t * port, IData *data)
{

    if (wstring(port) != L"Input")
        return false;

    //
    if(!IsFinished(data))
    {
        if(_data.get() == nullptr)
        //if(_datav.size() == 0)
        {
            InitScanData(data);
            InsertPhasedata(data);
        }
        else
        {
            InsertPhasedata(data);
        }

        CreateOutData(data);



    }
    else
    {
        //return Feed(L"Output", data);
        //_datav.clear();

        _data.reset();
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

     if(_data.get() == nullptr)
     //if(_datav.size() == 0)
     {
        _dataInfo.freq_count = variables.Get<int>(L"freq_count");
        _dataInfo.phase_count = variables.Get<int>(L"phase_count");
        _dataInfo.slice_count = variables.Get<int>(L"slice_count");
        _dataInfo.dim4 = variables.Get<int>(L"dim4");
        _dataInfo.dim5 = variables.Get<int>(L"dim5");
        _dataInfo.dim6 = variables.Get<int>(L"dim6");
        _dataInfo.channel_mask = variables.Get<int>(L"channel_mask");

        int channel_count = GetChannelCountUsed(_dataInfo.channel_mask);
        //
        assert(channel_count == 1);

        int temp = _dataInfo.freq_count * _dataInfo.phase_count * _dataInfo.slice_count * channel_count;
        _data = std::shared_ptr< std::complex<float> >(
                 new std::complex<float>[_dataInfo.freq_count * _dataInfo.phase_count * _dataInfo.slice_count * channel_count]);
        //_datav.resize(_dataInfo.freq_count * _dataInfo.phase_count * _dataInfo.slice_count * channel_count);

     }
     return true;
 }

int ReceiveData::GetChannelMaskindex(unsigned int channelMask, int channelIndex) const
{

    int channelIndexInMask = -1;
    bool used = false;
    if (channelMask & (1 << channelIndex))
    {
        used = true;
    }

    if(!used)
    {

    }
    else
    {
        for(int i = 0; i <= channelIndex; i ++)
        {
            if (channelMask & (1 << i))
            {
                channelIndexInMask ++;
            }

        }

    }

    return channelIndexInMask;

}
int ReceiveData::GetChannelCountUsed(unsigned int channelMask) const
{
    int count_max = sizeof(unsigned int) * 8;
    int countInMask = 0;
    for(int channel_index = 0; channel_index < count_max; channel_index ++)
    {
        if (channelMask & (1 << channel_index))
        {
            countInMask ++;
        }

    }

    return countInMask;
}

bool ReceiveData::InsertPhasedata(Yap::IData *data)
{



    assert(data->GetVariables() != nullptr);

    VariableSpace variables(data->GetVariables());

    int channel_index = variables.Get<int>(L"channel_index");
    int slice_index = variables.Get<int>(L"slice_index");
    int phase_index = variables.Get<int>(L"phase_index");

    int channelMaskindex = GetChannelMaskindex(_dataInfo.channel_mask, channel_index);

    DataHelper input_data(data);
    assert(input_data.GetActualDimensionCount() == 1);
    assert(_dataInfo.freq_count == input_data.GetWidth() );
    assert(input_data.GetDataType() == DataTypeComplexFloat);

    auto phase_data = GetDataArray<std::complex<float>>(data);
    InsertPhasedata(phase_data, channelMaskindex, slice_index, phase_index);
    return true;


}
bool ReceiveData::InsertPhasedata(std::complex<float> *data, int channel_index, int slice_index, int phase_index)
{
    int freqCount  = _dataInfo.freq_count;
    int phaseCount = _dataInfo.phase_count;
    int sliceCount = _dataInfo.slice_count;

    int channelSize = freqCount * phaseCount * sliceCount;
    int sliceSize = freqCount * phaseCount;


    int temp =  channelSize * channel_index
              + sliceSize   * slice_index
              + freqCount   * phase_index;
    memcpy(_data.get() + channelSize * channel_index
                       + sliceSize   * slice_index
                       + freqCount   * phase_index,
           data, freqCount * sizeof(std::complex<float>));


    return true;

}

//end is not included.
std::vector<std::complex<float>> ReceiveData::LookintoPtr(std::complex<float> *data, int size, int start, int end)
{
    std::vector<std::complex<float>> view_data;
    int view_size = end - start;

    assert(start < end);
    assert(end <= size);

    view_data.resize(view_size);
    memcpy(view_data.data(), data + start, view_size * sizeof(std::complex<float>));
    return view_data;
}

bool ReceiveData::CreateOutData(Yap::IData *data)
{
    unsigned int width = _dataInfo.freq_count;
    unsigned int height = _dataInfo.phase_count;
    unsigned int slice_count = _dataInfo.slice_count;
    unsigned int dim4 = _dataInfo.dim4;
    unsigned int channel_count = this->GetChannelCountUsed(_dataInfo.channel_mask);

    for(unsigned int channel_index = 0; channel_index < channel_count; channel_index++)
    {
        //unsigned int maskIndex = this->GetChannelMaskindex(_dataInfo.channel_mask, channel_index);

        Dimensions dimensions;
        dimensions(DimensionReadout, 0U, width)
            (DimensionPhaseEncoding, 0U, height)
            (DimensionSlice, 0U, slice_count)
            (Dimension4, 0U, dim4)
            (DimensionChannel, channel_index, 1);


        complex<float> *channel_buffer = new std::complex<float>[width * height * slice_count];

        complex<float> * raw_data_buffer = _data.get() + width * height * slice_count * channel_index;
        memcpy( channel_buffer, raw_data_buffer, width * height * slice_count * sizeof(std::complex<float>));

        //
        VariableSpace variables(data->GetVariables());
        variables.AddVariable(L"bool", L"test_data", L"test.");
        variables.Set(L"test_data", false);

        //

        auto output = CreateData<complex<float>>(data, channel_buffer, dimensions);

        Feed(L"Output", output.get());


    }

    return true;
}
ReceiveData::~ReceiveData()
{
//    LOG_TRACE(L"NiuMriDisplay2D destructor called.", L"NiuMri");
}
