#include "databin.h"
#include <string>
#include <vector>
#include "EcnuRawDataReader.h"
#include <boost/shared_array.hpp>
#include <fstream>
#include <iomanip>
#include <cassert>

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


Databin::Databin()
{

}


void Databin::Load(std::wstring dataPath)
{
    std::shared_ptr<RawDataInfo> data_info = _dataInfo;
    std::wstring raw_data_folder = dataPath;
    int group_count = 1;
    int group_index = 0;

    //auto reader = Hardware::CSpectrometerFactory::CreateReader(system.spectrometer.spec_type, 0, raw_data_folder);
    CEcnuRawDataReader reader;

    wstring file_ext = L"fid"; //L"mrd" :
    unsigned int channel_mask = 0x0F;
    DstRawDataName name_function(file_ext, group_count, group_index);
    std::vector<std::wstring> path = name_function.GetNames(channel_mask);
    std::vector<std::string> path2;

    for (size_t i = 0; i < path.size(); ++i)
    {
        path[i] = raw_data_folder + L"\\" + path[i];
        std::string temp(path[i].begin(), path[i].end());
        path2.push_back(temp);

    }

    boost::shared_array<complex<float>> data(reinterpret_cast<complex<float>*>(reader.ReadAllData(data_info.get(), path2)));


    //CString data_info_str;
    //data_info_str.Format(_T("DataInfo %d %d %d %d %d %d %d"),
    //    data_info->freq_point_count, data_info->phase_point_count, data_info->slice_count,
    //    data_info->dim4, data_info->dim5, data_info->dim6, data_info->channel_count);
    //LOG_INFO(data_info_str);
    //OnScanComplete(data, data_info);

}


boost::shared_array<complex<float>> Databin::GetRawData(unsigned int channelIndex)
{

    assert(channelIndex + 1 < _dataInfo.get()->channel_count);

    int channelSize =  _dataInfo.get()->freq_point_count *
                        _dataInfo.get()->phase_point_count *
                        _dataInfo.get()->slice_count;

    std::complex<float>* srcChannel = _data.get() + channelIndex * channelSize;

    //拷贝一份返回给用户。需要这么做吗？
    boost::shared_array<complex<float>> destChannel(new complex<float>[channelSize]);

    memcpy(destChannel.get(), srcChannel, channelSize * sizeof(complex<float>));
    return destChannel;

}
//
boost::shared_array<complex<float>> Databin::GetRawData(unsigned int channelIndex, unsigned int sliceIndex)
{
    //assert(0);
    //return boost::shared_array<complex<float>>(nullptr);
    assert(channelIndex + 1 < _dataInfo.get()->channel_count);
    assert(sliceIndex + 1 < _dataInfo.get()->slice_count);

    unsigned int freqPointCount  = _dataInfo.get()->freq_point_count;
    unsigned int phasePointCount = _dataInfo.get()->phase_point_count;
    unsigned int sliceCount = _dataInfo.get()->slice_count;


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
    assert(channelIndex + 1 < _dataInfo.get()->channel_count);
    assert(sliceIndex + 1 < _dataInfo.get()->slice_count);
    assert(phaseIndex + 1 < _dataInfo.get()->phase_point_count);

    unsigned int freqPointCount = _dataInfo.get()->freq_point_count;
    complex<float>* srcLine = GetRawData(channelIndex, sliceIndex).get() + freqPointCount * phaseIndex;
    boost::shared_array<complex<float>> destLine(new complex<float>[freqPointCount]);

    memcpy(destLine.get(), srcLine, sizeof(complex<float>) * freqPointCount);
    return destLine;

}

