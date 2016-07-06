#include "stdafx.h"

#include "ProcessorDebugger.h"

#include <iostream>
#include "..\Interface\YapInterfaces.h"
#include "../Interface/DataImp.h"

using namespace Yap;
using namespace std;

void CProcessorDebugger::DebugPort(IPortEnumerator& ports)
{
	for (auto port = ports.GetFirstPort(); port != nullptr; port = ports.GetNextPort())
	{
		wcout << "Name: " << port->GetName() << "\t Dimensions: " << port->GetDimensions()
			<< "\t DataType: " << port->GetDataType() << endl;
	}
}

void CProcessorDebugger::DebugOutput(IProcessor& processor)
{
	auto processor_id = processor.GetClassId();
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

bool CProcessorDebugger::DebugProperties(IPropertyEnumerator * properties)
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

	return true;
}

bool CProcessorDebugger::DebugPlugin(const wchar_t * path)
{
	auto module = ::LoadLibrary(path);
	if (module == NULL)
	{
		wcout << L"Failed to load plugin.\n";
		return false;
	}

	auto get_processor_manager = reinterpret_cast<IProcessorManager * (*)()>
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

	for (auto processor = processor_manager->GetFirstProcessor(); processor != nullptr;
		processor = processor_manager->GetNextProcessor())
	{		
		DebugOutput(*processor);
		auto properties = processor->GetProperties();
		DebugProperties(properties);
	}
	return true;
}
