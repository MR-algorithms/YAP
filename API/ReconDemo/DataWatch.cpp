#include "DataWatch.h"

#include <boost\smart_ptr\shared_array.hpp>
//#include "Utilities\macros.h"

#include <comdef.h>
#include <GdiPlus.h>
#include <sstream>

#pragma comment(lib, "Gdiplus.lib")

using namespace Gdiplus;
using namespace std;


namespace FromYap { namespace details
{
    static INT GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
    {
        UINT  num = 0;          // number of image encoders
        UINT  size = 0;         // size of the image encoder array in bytes

        ImageCodecInfo* pImageCodecInfo = NULL;

        GetImageEncodersSize(&num, &size);
        if (size == 0)
            return -1;  // Failure

        pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
        if (pImageCodecInfo == NULL)
            return -1;  // Failure

        GetImageEncoders(num, size, pImageCodecInfo);

        for (UINT j = 0; j < num; ++j)
        {
            if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
            {
                *pClsid = pImageCodecInfo[j].Clsid;
                free(pImageCodecInfo);
                return j;  // Success
            }
        }

        free(pImageCodecInfo);
        return -1;  // Failure
    }

    class JpegExporterImp
    {
    public:
        ~JpegExporterImp() {
            Gdiplus::GdiplusShutdown(_gdiplusToken);
        }
    private:
        JpegExporterImp() {
            Gdiplus::GdiplusStartupInput gdiplusStartupInput;
            GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, NULL);
        }

        JpegExporterImp(const JpegExporterImp& rhs) {
            Gdiplus::GdiplusStartupInput gdiplusStartupInput;
            GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, NULL);
        }

        template<typename T>
        void ExportImage(T* image, int width, int height, const wchar_t * output_folder, bool stretch_pixel_data, int jpeg_index2)
        {
            boost::shared_array<unsigned char> buffer(new unsigned char[width * height * 4]);
            T* data_cursor = image;

            auto result = std::minmax_element(data_cursor, data_cursor + width * height);
            T min_data = *result.first;
            T max_data = *result.second;

            for (unsigned char* buffer_cursor = buffer.get(); buffer_cursor < buffer.get() + width * height * 4; buffer_cursor += 4)
            {
                unsigned short temp;
                if (stretch_pixel_data)
                {
                    temp = StretchPixelData(data_cursor, min_data, max_data);
                }
                else
                {
                    temp = static_cast<unsigned short>(*data_cursor);
                }
                if (data_cursor == NULL)
                {
                    break;
                }
                *buffer_cursor = *(buffer_cursor + 1) = *(buffer_cursor + 2) = static_cast<unsigned char>(temp);
                *(buffer_cursor + 3) = 255;
                ++data_cursor;
            }

            Gdiplus::Bitmap bitmap(width, height, width * 4, PixelFormat32bppRGB, buffer.get());
            CLSID  encoderClsid;
            if (!GetEncoderClsid(L"image/jpeg", &encoderClsid))
            {
                return;
            }
            wostringstream file_name;
            //static unsigned int jpeg_index = 0;
            //file_name << ++jpeg_index;
            file_name <<jpeg_index2;

            wstring file_path = output_folder;
            if (wcslen(output_folder) > 3)
            {
                file_path += L"\\";
            }
            file_path += file_name.str();
            file_path += L".jpg";

            Gdiplus::Status status = bitmap.Save(const_cast<wchar_t*>(file_path.c_str()), &encoderClsid, NULL);
        }

        template<typename T>
        unsigned short StretchPixelData(T* data, T min_value, T max_data)
        {
            double rate = 255.0 / (double)(max_data - min_value);
            return static_cast<unsigned short>((*data - min_value) * rate);
        }

        ULONG_PTR _gdiplusToken;

        friend class CDataWatch;
    };
}}
using namespace FromYap;
using namespace FromYap::details;
bool CDataWatch::ExportData(const wchar_t * image_path,
							 std::complex<float>* data,
							 int width, 
                             int height,
                             int file_index)
{

    boost::shared_array<float> temp(new float[width * height]);
    for (int i = 0; i < width * height; ++i)
    {
        temp[i] = abs(data[i]);
    }

    auto impl = shared_ptr<JpegExporterImp>(new JpegExporterImp);
    impl->ExportImage(temp.get(),
                      width, height,
                      image_path,
                      true,
                      file_index);
	return true;
}

