// PipelineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winbase.h>
#include <iostream>
#include "..\Interface\Interface.h"
#include <string>
#include <vector>

using namespace std;

void DebugOutput(IPortEnumerator& ports)
{
	for (auto port = ports.GetFirstPort(); port != nullptr; port = ports.GetNextPort())
	{
		wcout << "Name: " << port->GetName() << "\t Dimensions: " << port->GetDimensions()
			<< "\t DataType: " << port->GetDataType() << endl;
	}
}

void DebugOutput(IProcessor& processor)
{
	auto processor_id = processor.GetId();
	wcout << L"Processor: " << wstring(processor_id) << endl;
	auto in_ports = processor.GetInputPortEnumerator();
	if (in_ports != nullptr)
	{
		wcout << L"In port(s): \n";
		DebugOutput(*in_ports);
	}
	else
	{
		wcout << "No in port found.\n";
	}

	auto out_ports = processor.GetOutputPortEnumerator();
	if (out_ports != nullptr)
	{
		wcout << L"Out port(s): \n";
		DebugOutput(*out_ports);
	}
	else
	{
		wcout << "No in port found.\n";
	}

}

class CDimensionsImp : public IDimensions
{
public:
	CDimensionsImp(unsigned int width, unsigned int height) : _width(width), _height(height) {}
	virtual unsigned int GetDimensionCount() override
	{
		return 2;
	}

	virtual bool GetDimensionInfo(unsigned int index, 
		OUT DimensionType& dimension_type, 
		OUT unsigned int& start_index, 
		OUT unsigned int& length) override
	{
		if (index == 0)
		{
			dimension_type = DimensionReadout;
			start_index = 0;
			length = _width;
		}
		else if (index == 1)
		{
			dimension_type = DimensionPhaseEncoding;
			start_index = 0;
			length = _height;
		}
		else
		{
			return false;
		}

		return true;
	}
protected:
	unsigned int _width;
	unsigned int _height;
};

class CDataImp : public IData
{
public:
	CDataImp(double * data, unsigned int width, unsigned height) : 
		_data(data), _dimensions(width, height)
	{
	}
	virtual void * GetData() override
	{
		return _data;
	}

	virtual IDimensions * GetDimension() override
	{
		return &_dimensions;
	}
protected:
	double * _data;
	CDimensionsImp _dimensions;
};

int main()
{
	auto module = ::LoadLibrary(L"BasicRecon.dll");
	if (module == NULL)
	{
		wcout << L"Failed to load plugin.\n";
		return -1;
	}

	auto get_processor_manager = reinterpret_cast<IProcessorManager * (*)()>
		(::GetProcAddress(module, "GetProcessorManager"));

	if (get_processor_manager == nullptr)
	{
		wcout << L"Failed to find GetProcessorManager().\n";
		return -1;
	}

	auto processor_manager = get_processor_manager();
	if (processor_manager == nullptr)
	{
		wcout << L"Failed to get IProcessorManager.\n";
		return -1;
	}

	for (auto processor = processor_manager->GetFirstProcessor(); processor != nullptr;
		processor = processor_manager->GetNextProcessor())
	{
		DebugOutput(*processor);
	}

	auto processor = processor_manager->GetProcessor(L"Dummy");

	auto properties = processor->GetProperties();
	if (properties != nullptr)
	{
		for (auto property = properties->GetFirst(); property != nullptr;
			property = properties->GetNext())
		{
			wcout << property->GetName() << " " << property->GetType() << " ";
			switch (property->GetType())
			{
			case PropertyBool:
			{
				auto value_interface = reinterpret_cast<IBoolValue*>(property->GetValueInterface());
				wcout << value_interface->GetValue();
				break;
			}
			case PropertyFloat:
			{
				auto value_interface = reinterpret_cast<IFloatValue*>(property->GetValueInterface());
				wcout << value_interface->GetValue();
				break;
			}
			case PropertyInt:
			{
				auto value_interface = reinterpret_cast<IIntValue*>(property->GetValueInterface());
				wcout << value_interface->GetValue();
				break;
			}
			case PropertyString:
			{
				auto value_interface = reinterpret_cast<IStringValue*>(property->GetValueInterface());
				wcout << value_interface->GetValue();
				break;
			}
			default:
				break;
			}
			wcout << endl;
		}
	}

	vector<double> image(256 * 256);
	for (unsigned int i = 0; i < image.size(); ++i)
	{
		image[i] = 1.0;
	}

	CDataImp data(image.data(), 256, 256);

	processor->Input(L"Input", &data);

    return 0;
}

