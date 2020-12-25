#include "stdafx.h"
#include "Flip.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <string>
#include <complex>

using namespace std;
using namespace Yap;

Flip::Flip() : ProcessorImpl(L"Flip")
{
	AddInput(L"Input", 2, DataTypeUnsignedShort | DataTypeShort);
	AddOutput(L"Output", 2, DataTypeUnsignedShort | DataTypeShort);
	AddProperty<bool>(L"FlipH", false, L"Flip Horizontal.");
	AddProperty<bool>(L"FlipV", true, L"Flip Vertical.");

}

Flip::Flip(const Flip& rhs)
	:ProcessorImpl(rhs)
{
}

Flip::~Flip()
{
}
/**
	\remark backup function.
*/

bool Flip::Input(const wchar_t * port, IData * data)
{
	
	if (std::wstring(port) != L"Input")
	{
		LOG_ERROR(L"<Flip> Error input port name!", L"BasicRecon");
		return false;
	}

	DataHelper helper(data);
	

	Dimensions data_dimensions(data->GetDimensions());
	int width{ 1 }, height{ 1 };
	width = helper.GetWidth();
	height = helper.GetHeight();
	assert(1 == helper.GetSliceCount());

	bool FlipH( GetProperty<bool>(L"FlipH") );
	bool FlipV( GetProperty<bool>(L"FlipV") );
	
	//
	
	

	if (data->GetDataType() == DataTypeShort)
	{
		assert(0, L"Not Implemented.");
	}

	unsigned short* buffer = Yap::GetDataArray<unsigned short>(data);

	if (FlipH) 
	{

		for (int row_index = 0; row_index < width; ++row_index)
		{
			unsigned short * line = buffer + row_index * width;

			for (int i = 0; i < width / 2; i++)
			{
				swap(*(line + i), *(line + width - 1 - i));
			}
		}

	}
	

	if (FlipV)
	{
		unsigned short * temp = new unsigned short[width];

		for (int j = 0; j < height/2; ++j)
		{
			unsigned short * line1 = buffer + j * width;
			unsigned short * line2 = buffer + (height - 1 - j) * width;

			memcpy(temp, line1, sizeof(unsigned short) * width);
			memcpy(line1, line2, sizeof(unsigned short) * width);
			memcpy(line2, temp, sizeof(unsigned short) * width);
			
		}
		delete[]temp;


	}
	return Feed(L"Output", data);
	
}