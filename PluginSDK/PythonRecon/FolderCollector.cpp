#include "FolderCollector.h"
#include "Implement\LogUserImpl.h"
#include "Client\DataHelper.h"
#include <iostream>

using namespace std;

Yap::FolderCollector::FolderCollector() : ProcessorImpl(L"FolderCollector")
{
	AddInput(L"Input", 2, DataTypeFloat);
	AddOutput(L"Output", 2, DataTypeFloat);
}

Yap::FolderCollector::FolderCollector(const FolderCollector& rhs):
	ProcessorImpl(rhs)
{
}

Yap::FolderCollector::~FolderCollector()
{
}

bool Yap::FolderCollector::Input(const wchar_t * name, IData * data)
{
	assert(data != nullptr);
	assert(Inputs()->Find(name) != nullptr);
	assert(data->GetDataType() == DataTypeFloat);

	VariableSpace variable(data->GetVariables());
	if (!variable.Get<bool>(L"FolderFinished"))
	{
		_collector.insert(make_pair(_count++, CreateData<float>(data))); // 
	}
	else
	{
		// add labels data to last column of every row in a feature matrix.
		DataHelper helper(_collector[0].get());
		auto dim = helper.GetDimension(DimensionReadout);
		unsigned int row_size =unsigned int(_collector.size());
		Dimensions dimension;
		dimension(DimensionReadout, 0U, dim.length + 1)
			(DimensionPhaseEncoding, 0U, row_size);
		float * collect_data;
		try
		{
			collect_data = new  float[(dim.length + 1)* row_size];
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}
		size_t count = 0;
		for (unsigned int i = 0; i < _count; ++i)
		{
			memcpy(collect_data + count, GetDataArray<float>(_collector[i].get()), (dim.length) * sizeof(float));
			count += (dim.length + 1);
			VariableSpace var(_collector[i]->GetVariables());
			*(collect_data + count) = float(var.Get<int>(L"FolderLabel"));
		}

		auto out_data = CreateData<float>(nullptr, collect_data, dimension, nullptr);
		Feed(L"Output", out_data.get());
		_count = 0;
		_collector.clear();
	}

	return true;

}
