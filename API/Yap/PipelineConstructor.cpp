#include "PipelineConstructor.h"
#include "ModuleManager.h"
#include "ProcessorAgent.h"
#include "Implement/CompositeProcessor.h"
#include "Implement/LogImpl.h"

using namespace Yap;
using namespace std;

ConstructError::ConstructError() :
	_error_number(ConstructErrorSuccess)
{
}

ConstructError::ConstructError(unsigned int line_number,
	int error_number,
	const wstring& error_message)
	: _error_number(error_number),
	_error_message(error_message),
	_line_number(line_number)
{
}

unsigned int ConstructError::GetLineNumber() const
{
	return _line_number;
}

int ConstructError::GetErrorNumber() const
{
	return _error_number;
}

wstring ConstructError::GetErrorMessage() const
{
	return _error_message;
}

PipelineConstructor::PipelineConstructor()
{
	_module_manager = std::shared_ptr<ModuleManager>(new ModuleManager);

}

PipelineConstructor::~PipelineConstructor()
{
}

void PipelineConstructor::Reset(bool reset_modules)
{
    _pipeline = YapShared(new Pipeline(L"__PIPELINE"));

	if (reset_modules)
    {
		_module_manager->Reset();
    }
}

bool Yap::PipelineConstructor::LoadModule(const wchar_t * module_name)
{
	wstring plugin_path = L".";
	if (plugin_path[plugin_path.length() - 1] != L'\\')
	{
		plugin_path += L'\\';
	}
	plugin_path += module_name;

	return _module_manager->LoadModule(plugin_path.c_str());
}

void Yap::PipelineConstructor::SetPluginFolder(const wchar_t * path)
{
	_plugin_folder = path;
}

const wchar_t * Yap::PipelineConstructor::GetPluginFolder() const
{
	return _plugin_folder.c_str();
}

IProcessor * Yap::PipelineConstructor::CreateProcessor(const wchar_t * class_id,
	const wchar_t * instance_id)
{
	assert(_pipeline);
	auto processor = _module_manager->CreateProcessor(class_id, instance_id);
	if (processor == nullptr)
	{

		throw ConstructError(0, ConstructErrorCreateProcessor, L"Failed to create processor instance.");
	}

	try
	{
		_pipeline->AddProcessor(processor);

		return processor;
	}
	catch (bad_alloc&)
	{
		throw ConstructError(0, ConstructErrorOutOfMemory, L"Out of memory.");
	}
}

bool PipelineConstructor::Link(const wchar_t * source, const wchar_t * dest)
{
	return Link(source, nullptr, dest, nullptr);
}

bool Yap::PipelineConstructor::Link(const wchar_t * source, 
	const wchar_t * source_port, 
	const wchar_t * dest, 
	const wchar_t * dest_port)
{
	auto source_processor = _pipeline->Find(source);
	if (!source_processor)
	{
		wstring message(L"Source processor not found: ");
		throw ConstructError(0, ConstructErrorProcessorNotFound, message + source);
	}

	auto dest_processor = _pipeline->Find(dest);
	if (!dest_processor)
	{
		wstring message(L"Destination processor not found.");
		throw ConstructError(0, ConstructErrorProcessorNotFound, message + dest);
	}

	if (source_port == nullptr)
	{
		source_port = L"Output";
	}
	if (dest_port == nullptr)
	{
		dest_port = L"Input";
	}

	if (!source_processor->Link(source_port, dest_processor, dest_port))
	{
		wstring message(L"Failed to add link. Source: ");
		message = message + source + L"." + source_port + L" Dest: " + dest + L"." + dest_port;
		throw ConstructError(0, ConstructErrorAddLink, message);
	}

	return true;
}

bool Yap::PipelineConstructor::MapInput(const wchar_t * pipeline_port, 
	const wchar_t * inner_processor, 
	const wchar_t * inner_port)
{
	assert(_pipeline);
	return _pipeline->MapInput(pipeline_port, inner_processor, inner_port);
}

bool Yap::PipelineConstructor::MapOutput(const wchar_t * pipeline_port, 
	const wchar_t * inner_processor, 
	const wchar_t * inner_port)
{
	assert(_pipeline);
	return _pipeline->MapOutput(pipeline_port, inner_processor, inner_port);
}

Yap::SmartPtr<Pipeline> Yap::PipelineConstructor::GetPipeline()
{
    if (_pipeline->_modules.empty())
    {
        for (auto module : _module_manager->_modules)
        {
            _pipeline->AddModule(module.second);
        }
    }
	return _pipeline;
}

bool PipelineConstructor::SetProperty(const wchar_t * processor_id,
	const wchar_t * property_id,
	const wchar_t * value)
{
    assert(_pipeline.get() != nullptr);

	ProcessorAgent processor(_pipeline->Find(processor_id));

	if (!processor)
	{
        auto output = wstring(L"Processor not found��") + processor_id;
		throw ConstructError(0, ConstructErrorProcessorNotFound, output.c_str());
	}

	auto properties = processor.GetProperties();
	if (properties == nullptr)
	{
		throw ConstructError(0, ConstructErrorPropertiesNotFound, L"No property enumerator found in processor.");
	}

	auto property = properties->Find(property_id);
	if (property == nullptr)
	{
		wostringstream output;
		output << wstring(L"Property not found: ") << property_id;
		throw ConstructError(0, ConstructErrorPropertyNotFound, output.str());
	}

	auto property_type = property->GetType();
	bool result = false;
	switch (property_type)
	{
	case VariableBool:
	{
		if (wcscmp(value, L"true") == 0)
		{
			result = processor.SetBool(property_id, true);
		}
		else if (wcscmp(value, L"false") == 0)
		{
			result = processor.SetBool(property_id, false);
		}
	}
	break;
	case VariableInt:
	{
		result = processor.SetInt(property_id, _wtoi(value));
	}
	break;
	case VariableFloat:
	{
		result = processor.SetFloat(property_id, _wtof(value));
	}
	break;
	case VariableString:
	{
		result = processor.SetString(property_id, value);
	}
	break;
	default:
		return false;
	}

	if (!result)
	{
		auto output = wstring(L"Failed to set property value,"
			" please check to see if the property is of matching type: ") 
			+ property_id;
		throw ConstructError(0, ConstructErrorPropertySet, output.c_str());
	}

	return result;
}

bool PipelineConstructor::InstanceIdExists(const wchar_t * id)
{
	return _pipeline->Find(id) != nullptr;
}

bool PipelineConstructor::MapProperty(const wchar_t * processor_id,
	const wchar_t * property_id,
	const wchar_t * variable_id, 
	bool input, bool output)
{
	assert(_pipeline);

	auto processor = _pipeline->Find(processor_id);
	if (!processor)
	{
		auto output = wstring(L"failed to find processor:") + processor_id;
		throw ConstructError(0, ConstructErrorProcessorNotFound, output.c_str());
	}

	if (!processor->MapProperty(property_id, variable_id, input, output))
	{
		wostringstream output;
		output << L"Fail to link property to system variable. Property: " << property_id
			<< L". System variable: " << variable_id;
		throw ConstructError(0, ConstructErrorPropertyLink, output.str());
	}

	return true;
}
