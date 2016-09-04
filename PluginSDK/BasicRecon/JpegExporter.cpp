#include "stdafx.h"

#include "JpegExporter.h"
#include "../../Shared/Interface/Client/DataHelper.h"

#define min(a, b) (a) < (b) ? (a) : (b)
#define max(a, b) (a) > (b) ? (a) : (b)

#include <boost\smart_ptr\shared_array.hpp>
#include "../../Shared/Utilities\macros.h"

#include <comdef.h>
#include <GdiPlus.h>
#include <sstream>

#pragma comment(lib, "Gdiplus.lib")

using namespace Gdiplus;
using namespace Yap;
using namespace std;


namespace Yap { namespace Implementation 
{
	INT GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
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

	class CJpegExporterImp
	{
	public:
		~CJpegExporterImp() {
			Gdiplus::GdiplusShutdown(_gdiplusToken);
		}
	private:
		CJpegExporterImp() {
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, NULL);
		}

		CJpegExporterImp(const CJpegExporterImp& rhs) {
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, NULL);
		}


		void ExportImage(float* image, int width, int height, const wchar_t * output_folder)
		{
			boost::shared_array<unsigned char> buffer(new unsigned char[width * height * 4]);
			float* data_cursor = image;

			auto result = std::minmax_element(data_cursor, data_cursor + width * height);
			float min_data = *result.first;
			float max_data = *result.second;

			for (unsigned char* buffer_cursor = buffer.get(); buffer_cursor < buffer.get() + width * height * 4; buffer_cursor += 4)
			{
				auto temp = StretchPixelData(data_cursor, min_data, max_data);
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
			static unsigned int jpeg_index = 0;
			file_name << ++jpeg_index;

			wstring file_path = output_folder;
			if (wcslen(output_folder) > 3)
			{
				file_path += L"\\";
			}
			file_path += file_name.str();
			file_path += L".jpg";

			Gdiplus::Status status = bitmap.Save(const_cast<wchar_t*>(file_path.c_str()), &encoderClsid, NULL);
		}

		unsigned short StretchPixelData(float* data, float min_value, float max_data)
		{
			double rate = 255.0 / (double)(max_data - min_value);
			return static_cast<unsigned short>((*data - min_value) * rate);
		}

		ULONG_PTR _gdiplusToken;

		friend class CJpegExporter;
	};
}}

using namespace Yap::Implementation;

CJpegExporter::CJpegExporter() :
	ProcessorImpl(L"JpegExporter")
{
	AddInputPort(L"Input", 2, DataTypeFloat);
	AddProperty(PropertyString, L"ExportFolder", L"Set folder used to hold exported images.");

	_impl = shared_ptr<CJpegExporterImp>(new CJpegExporterImp);
}

CJpegExporter::CJpegExporter(const CJpegExporter& rhs)
	: ProcessorImpl(rhs)
{
	_impl = std::shared_ptr<CJpegExporterImp>(new CJpegExporterImp(*rhs._impl));
}

CJpegExporter::~CJpegExporter()
{
}

IProcessor* CJpegExporter::Clone()
{
	try
	{
		auto processor = new CJpegExporter(*this);
		return processor;
	}
	catch(std::bad_alloc&)
	{
		return nullptr;
	}
}

bool CJpegExporter::OnInit()
{
	return true;
}

bool CJpegExporter::Input( const wchar_t * name, IData * data)
{
	assert((data != nullptr) && (GetDataArray<float>(data) != nullptr));
	assert(_impl);

	TODO(What if the data is not of float type ? );

	CDataHelper data_helper(data);
	_impl->ExportImage(GetDataArray<float>(data),
		data_helper.GetWidth(), data_helper.GetHeight(),
		GetString(L"ExportFolder"));

	return true;
}

