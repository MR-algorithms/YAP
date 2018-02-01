#include "stdafx.h"
#include "SliceMerger.h"

#include <complex.h>
#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

using namespace Yap;
using namespace std;

SliceMerger::SliceMerger(void) :
	ProcessorImpl(L"SliceMerger")
{
	AddInput(L"Input", 2, DataTypeAll);
	AddOutput(L"Output", 3, DataTypeAll);

	AddProperty<int>(L"SliceCount", 0, L"Slice count");
}

SliceMerger::SliceMerger(const SliceMerger& rhs)
	: ProcessorImpl(rhs)
{
}

SliceMerger::~SliceMerger()
{
}

bool SliceMerger::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	static int slice_num = 0;
	auto slice_count = GetProperty<int>(L"SliceCount");
	assert(slice_count > 0);

	if (slice_num == 0)
	{
		DataHelper helper(data);
		auto width = helper.GetWidth();
		auto height = helper.GetHeight();

		Yap::Dimensions dims;
		dims(DimensionReadout, 0, width)
			(DimensionPhaseEncoding, 0, height)
			(DimensionSlice, 0, slice_count);
		
		TODO(让SliceMerger适应不同数据类型);
		_data = CreateData<unsigned short>(data, &dims);

		++slice_num;
	}

	assert(slice_num != 0);
	DataHelper helper(data);
	auto width = helper.GetWidth();
	auto height = helper.GetHeight();
	auto input_array = GetDataArray<unsigned short>(data);
	auto output_array = GetDataArray<unsigned short>(_data.get());

	auto slice_start = output_array + width * height * (slice_num++ - 1);
	for (unsigned int row = 0; row < height; ++row)
	{
		memcpy(slice_start + row * width, input_array + row * width, width * sizeof(unsigned short));
	}

	if (slice_num > slice_count)
	{
		Feed(L"Output", _data.get());
	}

	return true;
}