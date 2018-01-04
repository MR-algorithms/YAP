#include "EcnuRawDataReader.h"
#include "EcnuRawDataFile.h"
#include <assert.h>

using namespace Hardware;

CEcnuRawDataReader::CEcnuRawDataReader()
{
}

CEcnuRawDataReader::~CEcnuRawDataReader(void)
{
}

float* CEcnuRawDataReader::ReadChannelData(RawDataInfo* pid, const std::string& file_path)
{
    assert(pid);
    pid->dim5 = 1;
    pid->dim6 = 1;
    return CEcnuRawDataFile::Read(file_path.c_str(), pid->freq_point_count, pid->phase_point_count, pid->slice_count, pid->dim4);
}

//
float* CEcnuRawDataReader::ReadAllData(RawDataInfo* pid, const std::vector<std::string>& files)
{
	float * buffer = nullptr;
	int freq_point_count = 0;
	int phase_point_count = 0;
	int slice_count = 0;
	try
	{
		for (size_t i = 0; i < files.size(); i ++)
		{
			float * channel_buffer = ReadChannelData(pid, files[i]);

			// 合法性判断
			if (channel_buffer == nullptr)		{
				if (buffer != nullptr)
				{
					delete [] buffer;
					buffer = nullptr;
				}
				break;
			}

			if (i == 0)
			{
				freq_point_count = pid->freq_point_count;
				phase_point_count = pid->phase_point_count;
				slice_count = pid->slice_count;

				buffer = new float[2 * pid->freq_point_count * pid->phase_point_count * pid->slice_count * files.size()];
				pid->channel_count = files.size();
			}

			if (freq_point_count == pid->freq_point_count &&
				phase_point_count == pid->phase_point_count &&
				slice_count == pid->slice_count)
			{
				// 所有通道的数据大小应该相同,否则说明文件错误。
				int channel_size = 2 * pid->freq_point_count * pid->phase_point_count * pid->slice_count;
				memcpy(buffer + i * channel_size, channel_buffer, channel_size * sizeof (float));
				delete channel_buffer;
			}
			else
			{
				delete [] buffer;
				buffer = nullptr;
				delete channel_buffer;
				break;
			}
		}
	}
    catch (std::bad_alloc&)
	{
        if (buffer != nullptr)
		{
			delete [] buffer;
			buffer = nullptr;
		}
		return buffer;
	}

	return buffer;
}

bool CEcnuRawDataReader::Prepare(const std::string& info)
{
	return true;
}
