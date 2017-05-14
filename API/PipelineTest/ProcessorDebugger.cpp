#include "stdafx.h"

#include "ProcessorDebugger.h"
#include "Implement/DataObject.h"

#include <iostream>

using namespace Yap;
using namespace std;

void YapDebugger::DebugPort(IPortIter& ports)
{
	for (auto port = ports.GetFirst(); port != nullptr; port = ports.GetNext())
	{
		wcout << "Name: " << port->GetId() << "\t Dimensions: " << port->GetDimensionCount()
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

bool YapDebugger::DebugProperties(IVariableIter& properties)
{
	for (auto property = properties.GetFirst(); property != nullptr;
		property = properties.GetNext())
	{
		wcout << property->GetId() << " " << property->GetType() << " ";
		switch (property->GetType())
		{
		case VariableBool:
		{
			auto variable_bool = dynamic_cast<ISimpleVariable<bool>*>(property);
			assert(variable_bool != nullptr);
			wcout << variable_bool->Get();
			break;
		}
		case VariableFloat:
		{
			auto variable_double = dynamic_cast<ISimpleVariable<double>*>(property);
			assert(variable_double != nullptr);
			wcout << variable_double->Get();
			break;
		}
		case VariableInt:
		{
			auto variable_int = dynamic_cast<ISimpleVariable<int>*>(property);
			assert(variable_int != nullptr);
			wcout << variable_int->Get();
			break;
		}
		case VariableString:
		{
			auto variable_string = dynamic_cast<ISimpleVariable<const wchar_t*>*>(property);
			assert(variable_string != nullptr);
			wcout << variable_string->Get();
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
		wcout << L"Failed to load plug-in DLL.\n";
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
