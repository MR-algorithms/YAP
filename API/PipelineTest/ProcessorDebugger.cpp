#include "stdafx.h"

#include "ProcessorDebugger.h"

#include <iostream>

#include "Interface/Implement/DataObject.h"
#include "Interface/IProcessor.h"
#include "Interface/IProperty.h"

using namespace Yap;
using namespace std;

void YapDebugger::DebugPort(IPortIter& ports)
{
	for (auto port = ports.GetFirst(); port != nullptr; port = ports.GetNext())
	{
		wcout << "Name: " << port->GetName() << "\t Dimensions: " << port->GetDimensionCount()
			<< "\t int: " << port->GetDataType() << endl;
	}
}

void YapDebugger::DebugOutput(IProcessor& processor)
{
	auto processor_id = processor.GetClassId();
	wcout << L"Processor: " << wstring(processor_id) << endl;
	auto inputs = processor.Inputs();
	if (inputs != nullptr)
	{
		wcout << L"In port(s): \n";
		auto iter = YapDynamic(inputs->GetIterator());
		if (iter)
		{
			DebugPort(*iter);
		}
	}
	else
	{
		wcout << "No in port found.\n";
	}

	auto outputs = processor.Outputs();
	if (outputs != nullptr)
	{
		wcout << L"Out port(s): \n";
		auto iter = YapDynamic(outputs->GetIterator());
		if (iter)
		{
			DebugPort(*iter);
		}
	}
	else
	{
		wcout << "No in port found.\n";
	}
}

bool YapDebugger::DebugProperties(IPropertyIter& properties)
{
	for (auto property = properties.GetFirst(); property != nullptr;
		property = properties.GetNext())
	{
		wcout << property->GetName() << " " << property->GetType() << " ";
		switch (property->GetType())
		{
		case PropertyBool:
		{
			auto value_interface = dynamic_cast<IBoolean*>(property);
			wcout << value_interface->GetBool();
			break;
		}
		case PropertyFloat:
		{
			auto value_interface = dynamic_cast<IDouble*>(property);
			wcout << value_interface->GetDouble();
			break;
		}
		case PropertyInt:
		{
			auto value_interface = dynamic_cast<IInt*>(property);
			wcout << value_interface->GetInt();
			break;
		}
		case PropertyString:
		{
			auto value_interface = dynamic_cast<IString*>(property);
			wcout << value_interface->GetString();
			break;
		}
		default:
			break;
		}
		wcout << endl;
	}

	return true;
}

bool YapDebugger::DebugPlugin(const wchar_t * path)
{
	auto module = ::LoadLibrary(path);
	if (module == NULL)
	{
		wcout << L"Failed to load plugin.\n";
		return false;
	}

	auto get_processor_manager = reinterpret_cast<IProcessorContainer * (*)()>
		(::GetProcAddress(module, "GetProcessorManager"));

	if (get_processor_manager == nullptr)
	{
		wcout << L"Failed to find GetProcessorManager().\n";
		return false;
	}

	auto processor_manager = get_processor_manager();
	if (processor_manager == nullptr)
	{
		wcout << L"Failed to get IProcessorManager.\n";
		return false;
	}

	auto processor_iter = YapDynamic(processor_manager->GetIterator());
	for (auto processor = processor_iter->GetFirst(); processor != nullptr;
		processor = processor_iter->GetNext())
	{	
		DebugOutput(*processor);
		auto properties = processor->GetProperties();
		auto property_iter = YapDynamic(properties->GetIterator());
	
		assert(property_iter);

		DebugProperties(*property_iter);
	}
	return true;
}
