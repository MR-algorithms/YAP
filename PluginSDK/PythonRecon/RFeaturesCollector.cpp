#include "stdafx.h"
#include "RFeaturesCollector.h"
#include "Implement\LogUserImpl.h"
#include "Client\DataHelper.h"
#include <vector>

using namespace Yap;
using namespace std;

Yap::RFeaturesCollector::RFeaturesCollector():
	ProcessorImpl(L"RFeaturesCollector")
{
	AddInput(L"Input", 3U, DataTypeFloat );
	AddOutput(L"Output", 2U, DataTypeFloat );
	LOG_TRACE(L"RadiomicsFeaturesCollector constructor", L"PythonRecon");
}

Yap::RFeaturesCollector::RFeaturesCollector(const RFeaturesCollector& rhs):
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"RadiomicsFeaturesCollector copy constructor", L"PythonRecon");
}

Yap::RFeaturesCollector::~RFeaturesCollector()
{
	LOG_TRACE(L"RadiomicsFeaturesCollector destructor", L"PythonRecon");
}

bool Yap::RFeaturesCollector::Input(const wchar_t * name, IData * data)
{
	assert(data != nullptr);
	assert(Inputs()->Find(name) != nullptr);

	DataHelper helper(data);
	auto dim1 = helper.GetDimension(DimensionUser1);
	auto dim2 = helper.GetDimension(DimensionUser2);
	assert(dim1.type != DimensionInvalid && dim2.type != DimensionInvalid);
	assert(helper.GetDataType() == DataTypeFloat);

	_collector.insert(make_pair(_count++, CreateData<float>(data)));
	if (_count == dim2.length)
	{
		unsigned int total_length = 0;
		for (unsigned int i = 0; i < _count; ++i)
		{
			DataHelper h(_collector[i].get());
			auto dim = h.GetDimension(DimensionReadout);
			total_length += dim.length;
		}
		Dimensions dimension;
		dimension(dim1.type, dim1.start_index, dim1.length)
			(DimensionReadout, 0U, total_length);
		float * collect_data;
		try
		{
			collect_data = new  float[total_length];
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}
		size_t count = 0;
		for (unsigned int i = 0; i < _count; ++i)
		{
			DataHelper h(_collector[i].get());
			auto dim = h.GetDimension(DimensionReadout);
			memcpy(collect_data + count, GetDataArray<float>(_collector[i].get()), (dim.length) * sizeof(float));
			count += dim.length;
		}

		auto out_data = CreateData<float>(nullptr, collect_data, dimension, nullptr);
			Feed(L"Output", out_data.get());
		_count = 0;
		_collector.clear();
	}
	return true;

}
