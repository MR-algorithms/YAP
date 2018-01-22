#include "CaseCollector.h"
#include "Implement\LogUserImpl.h"
#include "Client\DataHelper.h"
#include <iostream>

using namespace std;

Yap::CaseCollector::CaseCollector() : ProcessorImpl(L"CaseCollector")
{
	AddInput(L"Input", 2, DataTypeFloat);
	AddOutput(L"Output", 2, DataTypeFloat);
	LOG_TRACE(L"CaseCollector constructor", L"PythonRecon");
}

Yap::CaseCollector::CaseCollector(const CaseCollector& rhs):
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"CaseCollector copy constructor", L"PythonRecon");
}

Yap::CaseCollector::~CaseCollector()
{
	LOG_TRACE(L"CaseCollector destructor", L"PythonRecon");
}

bool Yap::CaseCollector::Input(const wchar_t * name, IData * data)
{
	assert(data != nullptr);
	assert(Inputs()->Find(name) != nullptr);

	DataHelper helper(data);
	auto dim1 = helper.GetDimension(DimensionUser1);
	assert(dim1.type != DimensionInvalid);
	assert(helper.GetDataType() == DataTypeFloat);

	_collector.insert(make_pair(_count++, CreateData<float>(data)));
	std::cout << _count << " (/" << dim1.length << ")" << std::endl;
	if (_count == dim1.length)
	{
		auto dim = helper.GetDimension(DimensionReadout);
		Dimensions dimension;
		dimension(DimensionReadout, 0U, dim.length)
			(DimensionPhaseEncoding, 0U, dim1.length);
		unsigned int total_length = dim.length * dim1.length;

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
