#include "stdafx.h"
#include "ReceiveData.h"
#include "Interface\IData.h"
#include "Interface/Client/DataHelper.h"

using namespace Yap;
using namespace std;
namespace Yap
{

	ReceiveData::ReceiveData() :
		ProcessorImpl(L"ReceiveData")
	{
	}

	ReceiveData::~ReceiveData()
	{
	}

	bool ReceiveData::OnInit()
	{
		AddInput(L"Input", 1, 
			DataTypeComplexDouble | 
			DataTypeComplexFloat | 
			DataTypeFloat | 
			DataTypeDouble | 
			DataTypeInt | 
			DataTypeShort | 
			DataTypeUnsignedInt | 
			DataTypeUnsignedShort);
		AddOutput(L"Output", YAP_ANY_DIMENSION, 
			DataTypeComplexDouble | 
			DataTypeComplexFloat | 
			DataTypeFloat |
			DataTypeDouble |
			DataTypeInt |
			DataTypeShort |
			DataTypeUnsignedInt |
			DataTypeUnsignedShort);
		AddProperty(PropertyInt, L"DataCount", L"The count of data flow times.");
		AddProperty(PropertyStruct, L"DataDimension", L"The dimension size of data and every dimension size as an Array.");
		return true;
	}

	IProcessor * ReceiveData::Clone()
	{
		return new(nothrow) ReceiveData(*this);
	}

	bool ReceiveData::Input(const wchar_t * name, IData * data)
	{
		if (name != L"Input")
			return false;
		if (data == nullptr)
			return false;
		DataHelper data_reverse = DataHelper(data);
		auto data_type = data_reverse.GetDataType();
		switch (data_type)
		{
			case DataTypeComplexDouble:
				
				break;
			case DataTypeComplexFloat:
				break;
			case DataTypeDouble:
				break;
			case DataTypeFloat:
				break;
			case DataTypeInt:
				break;
			case DataTypeShort:
				break;
			case DataTypeUnsignedInt:
				break;
			case DataTypeUnsignedShort:
				break;
			default:
				return false;
		}
		return true;
	}

}