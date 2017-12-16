#include "EcnuRawDataFile.h"
#include <fstream>
#include <cmath>
#include <cassert>

using namespace Hardware;
using namespace std;

/**
Read raw data from raw data file.
@return Pointer to the buffer used to store the raw-data. Should be released by the caller of this function.
@param width Output parameter used to store the width of the image (in pixel).
@param height Output, height of the image.
@param slices Output, number of slices read.
*/


float* CEcnuRawDataFile::Read(const char* file_path,
							  unsigned int& width,
							  unsigned int& height,
							  unsigned int& slices,
							  unsigned int& dim4)
{


    ifstream file(file_path, ios::binary);

    // read raw data header
    EcnuRawSections sections;
    if (!file.read(reinterpret_cast<char*>(&sections), sizeof(EcnuRawSections)))
        return nullptr;

    // read dimension information
    file.seekg(sizeof(EcnuRawSections) + sections.Section1Size + sections.Section2Size + sections.Section3Size,
        ios::beg);

    // Version 1.5701001以上版本, 允许浮点误差
    if (sections.FileVersion - 1.5701 > 0.00000005)
    {
        int buf[5];
        file.read(reinterpret_cast<char*>(buf), sizeof(int) * 5);
        assert(buf[4] == 1);
        width = buf[0];
        height = buf[1];
        slices = buf[2] * buf[3];
        dim4 = buf[3];
    }
    else
    {
        // 1.5702版本以上的谱仪版本，数据增加到5维，目前暂时第5维为1.
        int buf[4];
        file.read(reinterpret_cast<char*>(buf), sizeof(int) * 4);
        width = buf[0];
        height = buf[1];
        slices = buf[2] * buf[3];
        dim4 = buf[3];
    }

    unsigned buffer_size = width * height * 2 * slices;
    float * data = new float[buffer_size];

    // read actual data
    if (file.read(reinterpret_cast<char*>(data), buffer_size * sizeof(float)))
    {
        return data;
    }
    else
    {
        delete[]data;
        return nullptr;
    }
}
