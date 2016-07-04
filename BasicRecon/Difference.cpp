#include "stdafx.h"
#include "Difference.h"
#include "DataHelper.h"
#include <complex>

using namespace Yap;
using namespace std;

CDifference::CDifference() : CProcessorImp(L"Difference"),
	_start_data(nullptr),
	_is_set_start(false)
{
	AddInputPort(L"Start", 2, DataTypeDouble);
	AddInputPort(L"Destination", 2, DataTypeDouble);
	AddOutputPort(L"Output", 2, DataTypeDouble);
}


CDifference::~CDifference()
{
}

bool Yap::CDifference::Input(const wchar_t * port, IData * dest_data)
{
	if (std::wstring(port) != L"Destination")
		return false;

	if (!_is_set_start)
		return false; //need  a start image.

	CDataHelper dest(dest_data);
	if (dest.GetDataType() != DataTypeDouble)
		return false;
	if (dest.GetDimensionCount() != 2)
		return false;

	CDataHelper start(_start_data);

	if (dest.GetWidth() != start.GetWidth() || dest.GetHeight() != start.GetHeight())
		return false;

	Yap::CDimensions dims;
	dims(DimensionReadout, 0, dest.GetWidth())
		(DimensionPhaseEncoding, 0, dest.GetHeight());

	auto difference = CSmartPtr<CDoubleData>(new CDoubleData(&dims));
	Difference(reinterpret_cast<double*>(start.GetData()),
		reinterpret_cast<double*>(dest.GetData()), start.GetDataSize(),
		reinterpret_cast<double*>(difference->GetData()));

	Feed(L"Output", difference.get());

	return true;
}

bool Yap::CDifference::SetStartData(const wchar_t * port, IData * start_data)
{
	if (std::wstring(port) != L"Start")
		return false;

	if (start_data->GetDataType() != DataTypeDouble)
		return false;
	if (start_data->GetDimension()->GetDimensionCount() != 2)
		return false;

	 _start_data = start_data;
	 _is_set_start = true;

	 return true;
}

IData * Yap::CDifference::GetStartData()
{
	return _start_data;
}

bool Yap::CDifference::ResetStartData()
{
	_start_data = nullptr;
	_is_set_start = false;

	return true;
}

bool Yap::CDifference::Difference(double* start, double* dest, size_t size, double* difference)
{
	assert(start != nullptr && dest != nullptr && difference != nullptr);

	auto start_end = start + size;
	while (start != start_end)
	{
		*(difference++) = *(start++) - *(dest++);
	}

	return true;
}
