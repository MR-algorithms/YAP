#pragma once
#include <vector>
#include <string>



struct RawDataInfo
{
    unsigned int freq_point_count;
    unsigned int phase_point_count;
    unsigned int slice_count;
    unsigned int dim4;
    unsigned int dim5;
    unsigned int dim6;
    unsigned int channel_count;

    RawDataInfo() : freq_point_count(0), phase_point_count(0), slice_count(0), dim4(0), dim5(0), dim6(0), channel_count(0) {}
    bool operator == (const RawDataInfo& rhs) const{
        return freq_point_count == rhs.freq_point_count &&
            phase_point_count == rhs.phase_point_count &&
            slice_count == rhs.slice_count &&
            dim4 == rhs.dim4 &&
            dim5 == rhs.dim5 &&
            dim6 == rhs.dim6 &&
            channel_count == rhs.channel_count;
    }
};

//struct  ReorderInfo;
namespace Hardware
{
    class CRawDataReader
	{
	public:
        CRawDataReader(void){};
		virtual ~CRawDataReader(void){};

        virtual bool Prepare(const std::string& info) = 0;
        virtual float* ReadAllData(RawDataInfo* pid, const std::vector<std::string>& files) = 0;
        virtual float* ReadChannelData(RawDataInfo* pid, const std::string& file_path) = 0;
	};
};

