// PipelineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winbase.h>
#include <iostream>
#include "..\Interface\Interface.h"
#include "../Interface/ReconData.h"
#include <string>
#include <vector>

using namespace std;

void DebugPort(IPortEnumerator& ports)
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
		DebugPort(*in_ports);
	}
	else
	{
		wcout << "No in port found.\n";
	}

	auto out_ports = processor.GetOutputPortEnumerator();
	if (out_ports != nullptr)
	{
		wcout << L"Out port(s): \n";
		DebugPort(*out_ports);
	}
	else
	{
		wcout << "No in port found.\n";
	}

}

void DebugProperties(IPropertyEnumerator * properties)
{
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
				auto value_interface = dynamic_cast<IBoolValue*>(property);
				wcout << value_interface->GetValue();
				break;
			}
			case PropertyFloat:
			{
				auto value_interface = dynamic_cast<IFloatValue*>(property);
				wcout << value_interface->GetValue();
				break;
			}
			case PropertyInt:
			{
				auto value_interface = dynamic_cast<IIntValue*>(property);
				wcout << value_interface->GetValue();
				break;
			}
			case PropertyString:
			{
				auto value_interface = dynamic_cast<IStringValue*>(property);
				wcout << value_interface->GetValue();
				break;
			}
			default:
				break;
			}
			wcout << endl;
		}
	}

}

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
	DebugProperties(properties);
	
	vector<double> image(256 * 256);
	for (unsigned int i = 0; i < image.size(); ++i)
	{
		image[i] = 1.0;
	}
	Yap::CDimensions dimension;
	dimension(DimensionReadout, 0, 256)
		(DimensionPhaseEncoding, 0, 256);

	auto * data = new Yap::CDoubleData(image.data(), dimension, nullptr, false);

	processor->Input(L"Input", data);

	auto complex_splitter_processor = processor_manager->GetProcessor(L"ComplexSplitter");
	auto complex_splitter_properties = complex_splitter_processor->GetProperties();
	DebugProperties(complex_splitter_properties);

    return 0;
}

