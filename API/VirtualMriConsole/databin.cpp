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

const int ChannelMaxSize = 32;//sieof(unsigned int) * 8;
//copied from mri2/PrescanManager.cpp
struct DstRawDataName
{
    DstRawDataName(const std::wstring& suffix_str, int gp_count, int gp_idx)
        : suffix(suffix_str), group_count(gp_count), group_index(gp_idx){}

    std::vector<std::wstring> GetNames(unsigned int channel_switch)
    {



        std::vector<std::wstring> output;
        for (size_t channel_index = 0; channel_index < ChannelMaxSize; ++channel_index)
        {
            if (channel_switch & (1 << channel_index))
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


Databin::Databin(const Scan::ScanTask & scantask):_ended(false), _current_scanmask_index(0),_communicator(new Communicator), _scantask(scantask)
{}
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
    unsigned int channel_switch = AllChannel(channelCount);
    DstRawDataName name_function(file_ext, group_count, group_index);
    std::vector<std::wstring> path = name_function.GetNames(channel_switch);
    std::vector<std::string> path2;

    for (size_t i = 0; i < path.size(); ++i)
    {
        path[i] = raw_data_folder + L"\\" + path[i];
        std::string temp(path[i].begin(), path[i].end());
        path2.push_back(temp);

    }



    _data = boost::shared_array<complex<float>>( reinterpret_cast<complex<float>*>(reader.ReadAllData(&_dataInfo, path2)));

    //
    assert(_dataInfo.phase_point_count == _scantask.mask.phaseCount);

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


    boost::shared_array<complex<float>> destChannel(new complex<float>[channelSize]);

    memcpy(destChannel.get(), srcChannel, channelSize * sizeof(complex<float>));
    //
    std::vector<complex<float>> dest;
    dest.resize(50);
    memcpy(dest.data(), destChannel.get(), sizeof(std::complex<float>) * 50);
    //
    return destChannel;

}
//
boost::shared_array<complex<float>> Databin::GetRawData(unsigned int channelIndex, unsigned int sliceIndex)
{
    assert(channelIndex < _dataInfo.channel_count);
    assert(sliceIndex  < _dataInfo.slice_count);

    unsigned int freqPointCount  = _dataInfo.freq_point_count;
    unsigned int phasePointCount = _dataInfo.phase_point_count;
    unsigned int sliceCount = _dataInfo.slice_count;


    unsigned int sliceSize = freqPointCount * phasePointCount;
    unsigned int channelSize = freqPointCount * phasePointCount * sliceCount;


    complex<float> *srcSlice = _data.get() + channelSize * channelIndex + sliceSize * sliceIndex;

    boost::shared_array<complex<float>> destSlice(new complex<float>[freqPointCount * phasePointCount]);

    memcpy(destSlice.get(), srcSlice, sliceSize * sizeof(complex<float>));

    return destSlice;


}



boost::shared_array<complex<float>> Databin::GetRawData(unsigned int channelIndex, unsigned int sliceIndex, unsigned int phaseIndex)
{
    //assert(0);
    //return boost::shared_array<complex<float>>(nullptr);
    assert(channelIndex  < _dataInfo.channel_count);
    assert(sliceIndex  < _dataInfo.slice_count);
    assert(phaseIndex  < _dataInfo.phase_point_count);

    unsigned int sliceCount      = _dataInfo.slice_count;
    unsigned int phasePointCount = _dataInfo.phase_point_count;
    unsigned int freqPointCount  = _dataInfo.freq_point_count;

    //complex<float>* srcLine = GetRawData(channelIndex, sliceIndex).get() + freqPointCount * phaseIndex;

    unsigned int sliceSize = freqPointCount * phasePointCount;
    unsigned int channelSize = freqPointCount * phasePointCount * sliceCount;

    complex<float>* srcLine = _data.get()
            + channelSize * channelIndex
            + sliceSize * sliceIndex
            + freqPointCount * phaseIndex;

    boost::shared_array<complex<float>> destLine(new complex<float>[freqPointCount]);
    memcpy(destLine.get(), srcLine, sizeof(complex<float>) * freqPointCount);

    return destLine;

}


unsigned int Databin::GetPhaseCount()
{
    return _dataInfo.phase_point_count;
}

void Databin::Start(int scan_id)
{
    SampleDataStart start;
    start.scan_id = scan_id;
    start.dim1_size = _dataInfo.freq_point_count;
    start.dim2_size = _dataInfo.phase_point_count;
    start.dim3_size =_dataInfo.slice_count;
    start.dim4_size = _dataInfo.dim4;
    start.dim5_size = _dataInfo.dim5;
    start.dim6_size = _dataInfo.dim6;

    start.channel_switch =AllChannel(_dataInfo.channel_count);

    start.dim23456_size = start.dim2_size
            * start.dim3_size
            * start.dim4_size
            * start.dim5_size
            * start.dim6_size;

    _start = start;

    DataPackage dataPackage;
    MessageProcess::Pack(dataPackage, start);
    //

    dataPackage.CheckSelf();

    SampleDataStart start2;
    MessageProcess::Unpack(dataPackage, start2);
    assert(start == start2);
    //

    _communicator.get()->Send(dataPackage);

    _current_scanmask_index = 0;




}

bool Databin::Go()
{

    //go
    //The dimension of CMR rawdata is different from the dimension of Communication protocol.

    int phase_count = _dataInfo.phase_point_count;

    int current_phase_index = _scantask.mask.data[_current_scanmask_index];



    for(int channel_index = 0; channel_index < ChannelMaxSize; channel_index ++)
    {

        if (_start.channel_switch & (1 << channel_index))
        {

            for(int slice_index = 0; slice_index < static_cast<int>(_start.dim3_size ); slice_index ++)
            {
                //
                SampleDataData data;

                data.coeff = 3.45f;
                data.rec   = channel_index;
                data.rp_id = 824;

                //dim23456 does not denote the sending order, but for receiveing end decode
                //every index.
                data.dim23456_index
                        = slice_index   * phase_count
                        + current_phase_index;


                boost::shared_array<complex<float>> aline
                        = GetRawData(channel_index, slice_index, current_phase_index);

                //
                std::vector<complex<float>> destline;
                destline.resize(_dataInfo.freq_point_count);
                memcpy(destline.data(), aline.get(), sizeof(std::complex<float>) * _dataInfo.freq_point_count);
                //

                data.data_value.resize( _dataInfo.freq_point_count);

                memcpy(data.data_value.data(), aline.get(), sizeof(std::complex<float>) * _dataInfo.freq_point_count);

                DataPackage dataPackage;
                MessageProcess::Pack(dataPackage, data);
                //
                dataPackage.CheckSelf();
                SampleDataData data2;
                MessageProcess::Unpack(dataPackage, data2);
                assert(data == data2);
                //
                _communicator.get()->Send(dataPackage);
            }
        }
    }

    _current_scanmask_index ++;

    return true;


}


void Databin::End()
{
    if(!_ended)
    {
        //end.
        SampleDataEnd end;

        end.error_code = 7;
        DataPackage dataPackage;
        MessageProcess::Pack(dataPackage, end);

        //
        dataPackage.CheckSelf();
        SampleDataEnd end2;
        MessageProcess::Unpack(dataPackage, end2);
        assert( end == end2);

        //
        _communicator.get()->Send(dataPackage);

        _ended = true;
    }
}


