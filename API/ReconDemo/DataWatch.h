#pragma once

#include <ccomplex>

class CDataWatch
{
public:
    static bool ExportData(const wchar_t * image_path, std::complex<float>* data, int width, int height, int file_index);
};
