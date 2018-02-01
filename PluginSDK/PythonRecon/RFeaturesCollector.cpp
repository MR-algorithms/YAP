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
	assert(data->GetDataType() == DataTypeFloat);
	VariableSpace variable(data->GetVariables());
	if (!variable.Get<bool>(L"Finished"))
	{
		_collector.insert(make_pair(_count++, CreateData<float>(data)));
	}
	else
	{
		DataHelper helper(_collector[0].get());
		auto dim = helper.GetDimension(DimensionReadout);
		unsigned int total_length = dim.length * _count;

		Dimensions dimension;
		dimension(DimensionReadout, 0U, dim.length)
			(DimensionPhaseEncoding, 0U, _count);

		float * collect_data;
		try
		{
			collect_data = new  float[total_length];
		}
		catch (const std::bad_alloc&)
		{
			LOG_ERROR(L"allocate space error.", L"RFeaturesCollector--PythonRecon");
			return false;
		}
		size_t count = 0;
		for (unsigned int i = 0; i < _count; ++i)
		{
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
