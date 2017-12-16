#include "databin.h"
#include <string>
#include <vector>
#include "EcnuRawDataReader.h"
#include <boost/shared_array.hpp>
#include <fstream>
#include<iomanip>

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

    std::wstring raw_data_folder = dataPath;
    int group_count = 1;
    int group_index = 0;

    //auto reader = Hardware::CSpectrometerFactory::CreateReader(system.spectrometer.spec_type, 0, raw_data_folder);
    CEcnuRawDataReader reader;

    wstring file_ext = L"fid";//L"mrd" :
    unsigned int channel_mask = 0x0F;//应跟实际读取文件完全一致
    DstRawDataName name_function(file_ext, group_count, group_index);
    std::vector<std::wstring> path = name_function.GetNames(channel_mask);
    std::vector<std::string> path2;

    for (size_t i = 0; i < path.size(); ++i)
    {
        path[i] = raw_data_folder + L"\\" + path[i];
        std::string temp(path[i].begin(), path[i].end());
        path2.push_back(temp);

    }

    std::shared_ptr<RawDataInfo> data_info;

    boost::shared_array<complex<float>> data(reinterpret_cast<complex<float>*>(reader.ReadAllData(data_info.get(), path2)));


    //CString data_info_str;
    //data_info_str.Format(_T("DataInfo %d %d %d %d %d %d %d"),
    //    data_info->freq_point_count, data_info->phase_point_count, data_info->slice_count,
    //    data_info->dim4, data_info->dim5, data_info->dim6, data_info->channel_count);
    //LOG_INFO(data_info_str);
    //OnScanComplete(data, data_info);

}
