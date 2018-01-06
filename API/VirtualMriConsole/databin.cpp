#include "databin.h"
#include <string>
#include <vector>
#include "EcnuRawDataReader.h"
#include <boost/shared_array.hpp>
#include <fstream>
#include <iomanip>
#include <cassert>
#include "SampleDataProtocol.h"
#include "datapackage.h"
#include "communicator.h"

using namespace std;

//copied from mri2/PrescanManager.cpp
struct DstRawDataName
{
    DstRawDataName(const std::wstring& suffix_str, int gp_count, int gp_idx)
        : suffix(suffix_str), group_count(gp_count), group_index(gp_idx){}

    std::vector<std::wstring> GetNames(unsigned int channel_mask)
    {



        std::vector<std::wstring> output;
        for (size_t channel_index = 0; channel_index < sizeof(channel_mask) * 8; ++channel_index)
        {
            if (channel_mask & (1 << channel_index))
            {

                std::wostringstream filename;

                if (group_count > 1)
                {
                    filename << group_count << L"_" << group_index << L"ChannelData"
                         << setfill(L'0') << setw(2) << channel_index + 1 << L".fid";
                }
                else
                {

                    filename << L"ChannelData"
                        << setfill(L'0') << setw(2) << channel_index + 1 << L".fid";

                }
                output.push_back(filename.str());
            }
        }
        return output;
    }

    std::wstring suffix;
    int group_count;
    int group_index;
};


unsigned int Databin::AllChannel(int channelCount)
{
    unsigned int selectAll = 0;
    for(int channel_index = 0; channel_index < channelCount; channel_index ++)
    {
         unsigned int selectOne = (1 << channel_index);
         selectAll += selectOne;
    }

    return selectAll;

}
void Databin::Load(std::wstring dataPath, int channelCount)
{
    std::wstring raw_data_folder = dataPath;
    int group_count = 1;
    int group_index = 0;

    //auto reader = Hardware::CSpectrometerFactory::CreateReader(system.spectrometer.spec_type, 0, raw_data_folder);
    CEcnuRawDataReader reader;

    wstring file_ext = L"fid"; //L"mrd" :
    unsigned int channel_mask = AllChannel(channelCount);
    DstRawDataName name_function(file_ext, group_count, group_index);
    std::vector<std::wstring> path = name_function.GetNames(channel_mask);
    std::vector<std::string> path2;

    for (size_t i = 0; i < path.size(); ++i)
    {
        path[i] = raw_data_folder + L"\\" + path[i];
        std::string temp(path[i].begin(), path[i].end());
        path2.push_back(temp);

    }



    _data = boost::shared_array<complex<float>>( reinterpret_cast<complex<float>*>(reader.ReadAllData(&_dataInfo, path2)));


    //CString data_info_str;
    //data_info_str.Format(_T("DataInfo %d %d %d %d %d %d %d"),
    //    data_info->freq_point_count, data_info->phase_point_count, data_info->slice_count,
    //    data_info->dim4, data_info->dim5, data_info->dim6, data_info->channel_count);
    //LOG_INFO(data_info_str);
    //OnScanComplete(data, data_info);

}

boost::shared_array<complex<float>> Databin::GetRawData()
{
    int TotalSize =  _dataInfo.freq_point_count *
                     _dataInfo.phase_point_count *
                     _dataInfo.slice_count *
                     _dataInfo.channel_count;

     //拷贝一份返回给用户。
    boost::shared_array<complex<float>> destChannel(new complex<float>[TotalSize]);

    memcpy(destChannel.get(), _data.get(), TotalSize * sizeof(complex<float>));
    return destChannel;

}

boost::shared_array<complex<float>> Databin::GetRawData(unsigned int channelIndex)
{

    assert(channelIndex + 1 < _dataInfo.channel_count);

    int channelSize =  _dataInfo.freq_point_count *
                        _dataInfo.phase_point_count *
                        _dataInfo.slice_count;

    std::complex<float>* srcChannel = _data.get() + channelIndex * channelSize;

    //拷贝一份返回给用户。
    boost::shared_array<complex<float>> destChannel(new complex<float>[channelSize]);

    memcpy(destChannel.get(), srcChannel, channelSize * sizeof(complex<float>));
    return destChannel;

}
//
boost::shared_array<complex<float>> Databin::GetRawData(unsigned int channelIndex, unsigned int sliceIndex)
{
    //assert(0);
    //return boost::shared_array<complex<float>>(nullptr);
    assert(channelIndex + 1 < _dataInfo.channel_count);
    assert(sliceIndex + 1 < _dataInfo.slice_count);

    unsigned int freqPointCount  = _dataInfo.freq_point_count;
    unsigned int phasePointCount = _dataInfo.phase_point_count;
    unsigned int sliceCount = _dataInfo.slice_count;


    unsigned int sliceSize = freqPointCount * phasePointCount;
    unsigned int channelSize = freqPointCount * phasePointCount * sliceCount;


    complex<float> *srcSlice = _data.get() + channelSize * channelIndex + sliceCount * sliceIndex;

    boost::shared_array<complex<float>> destSlice(new complex<float>[freqPointCount * phasePointCount]);

    memcpy(destSlice.get(), srcSlice, sliceSize * sizeof(complex<float>));

    return destSlice;


}



boost::shared_array<complex<float>> Databin::GetRawData(unsigned int channelIndex, unsigned int sliceIndex, unsigned int phaseIndex)
{
    //assert(0);
    //return boost::shared_array<complex<float>>(nullptr);
    assert(channelIndex + 1 < _dataInfo.channel_count);
    assert(sliceIndex + 1 < _dataInfo.slice_count);
    assert(phaseIndex + 1 < _dataInfo.phase_point_count);

    unsigned int freqPointCount = _dataInfo.freq_point_count;
    complex<float>* srcLine = GetRawData(channelIndex, sliceIndex).get() + freqPointCount * phaseIndex;
    boost::shared_array<complex<float>> destLine(new complex<float>[freqPointCount]);

    memcpy(destLine.get(), srcLine, sizeof(complex<float>) * freqPointCount);
    return destLine;

}


unsigned int Databin::GetPhaseCount()
{
    return _dataInfo.phase_point_count;
}

void Databin::Start(int scan_id, int channel_count)
{
    SampleDataStart start;
    start.scan_id = scan_id;
    start.dim1_size = _dataInfo.freq_point_count;
    start.dim2_size = _dataInfo.phase_point_count;
    start.dim3_size = _dataInfo.slice_count;
    start.dim4_size = _dataInfo.dim4;
    start.dim5_size = _dataInfo.dim5;
    start.dim6_size = _dataInfo.dim6;

    start.channel_mask = AllChannel(channel_count);

    start.dim23456_size = start.dim2_size
            * start.dim3_size
            * start.dim4_size
            * start.dim5_size
            * start.dim6_size;

    DataPackage dataPackage;
    MessageProcess::Pack(dataPackage, start);
    //
    Communicator::GetHandle().Send(dataPackage);




}
void Databin::Go()
{
    //go
    if(CanbeFinished())
    {
        End();
    }
}
void Databin::End()
{
    if(!_ended)
    {
        //end.
        _ended = true;
    }
}


