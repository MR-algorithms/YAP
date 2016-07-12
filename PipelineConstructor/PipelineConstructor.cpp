#include "stdafx.h"
#include "PipelineConstructor.h"
#include "ModuleManager.h"
#include "ProcessorAgent.h"
#include "CompositeProcessor.h"

using namespace Yap;
using namespace std;

CConstructError::CConstructError() :
	_error_number(ConstructErrorSuccess)
{
}

CConstructError::CConstructError(unsigned int line_number,
	int error_number,
	const wstring& error_message)
	: _error_number(error_number),
	_error_message(error_message),
	_line_number(line_number)
{
}

unsigned int CConstructError::GetLineNumber() const
{
	return _line_number;
}

int CConstructError::GetErrorNumber() const
{
	return _error_number;
}

wstring CConstructError::GetErrorMessage() const
{
	return _error_message;
}

CPipelineConstructor::CPipelineConstructor()
{
	_module_manager = std::shared_ptr<CModuleManager>(new CModuleManager);

}

CPipelineConstructor::~CPipelineConstructor()
{
}

void CPipelineConstructor::Reset(bool reset_modules)
{
	_pipeline = shared_ptr<CCompositeProcessor>(new CCompositeProcessor(L"__PIPELINE"));
	_pipeline->Init();

	if (reset_modules)
	{
		_module_manager->Reset();
	}
}

bool Yap::CPipelineConstructor::LoadModule(const wchar_t * module_name)
{
	wstring plugin_path = L".";
	if (plugin_path[plugin_path.length() - 1] != L'\\')
	{
		plugin_path += L'\\';
	}
	plugin_path += module_name;

	return _module_manager->LoadModule(plugin_path.c_str());
}

void Yap::CPipelineConstructor::SetPluginFolder(const wchar_t * path)
{
	_plugin_folder = path;
}

const wchar_t * Yap::CPipelineConstructor::GetPluginFolder() const
{
	return _plugin_folder.c_str();
}

std::shared_ptr<CProcessorAgent> Yap::CPipelineConstructor::CreateProcessor(const wchar_t * class_id,
	const wchar_t * instance_id)
{
	assert(_pipeline);
	auto processor = _module_manager->CreateProcessor(class_id, instance_id);
	if (processor == nullptr)
	{
		throw CConstructError(0, ConstructErrorCreateProcessor, L"Failed to create processor instance.");
	}

	try
	{
		shared_ptr<CProcessorAgent> processor_agent(new CProcessorAgent(processor));
		_pipeline->AddProcessor(processor_agent);

		return processor_agent;
	}
	catch (bad_alloc&)
	{
		throw CConstructError(0, ConstructErrorOutOfMemory, L"Out of memory.");
	}
}

bool CPipelineConstructor::Link(const wchar_t * source, const wchar_t * dest)
{
	return Link(source, nullptr, dest, nullptr);
}

bool Yap::CPipelineConstructor::Link(const wchar_t * source, 
	const wchar_t * source_port, 
	const wchar_t * dest, 
	const wchar_t * dest_port)
{
	auto source_processor = _pipeline->GetProcessor(source);
	if (!source_processor)
	{
		throw CConstructError(0, ConstructErrorProcessorNotFound, L"Processor not found.");
	}

	auto dest_processor = _pipeline->GetProcessor(dest);
	if (!dest_processor)
	{
		throw CConstructError(0, ConstructErrorProcessorNotFound, L"Processor not found.");
	}

	if (source_port == nullptr)
	{
		source_port = L"Output";
	}
	if (dest_port == nullptr)
	{
		dest_port = L"Input";
	}

	if (!source_processor->Link(source_port, dest_processor.get(), dest_port))
	{
		throw CConstructError(0, ConstructErrorAddLink, L"Failed to add link.");
	}

	return true;
}

bool Yap::CPipelineConstructor::AssignPipelineInPort(const wchar_t * pipeline_port, 
	const wchar_t * inner_processor, 
	const wchar_t * inner_port)
{
	assert(_pipeline);
	return _pipeline->AssignInPort(pipeline_port, inner_processor, inner_port);
}

bool Yap::CPipelineConstructor::AssignPipelineOutPort(const wchar_t * pipeline_port, 
	const wchar_t * inner_processor, 
	const wchar_t * inner_port)
{
	assert(_pipeline);
	return _pipeline->AssignOutPort(pipeline_port, inner_processor, inner_port);
}

std::shared_ptr<CCompositeProcessor> Yap::CPipelineConstructor::GetPipeline()
{
	return _pipeline;
}

bool CPipelineConstructor::SetProperty(const wchar_t * processor_id,
	const wchar_t * property_id,
	const wchar_t * value)
{
	assert(_pipeline != nullptr);

	auto processor = _pipeline->GetProcessor(processor_id);
	if (!processor)
	{
		auto output = wstring(L"Processor not found£º") + processor_id;
		throw CConstructError(0, ConstructErrorProcessorNotFound, output.c_str());
	}

	auto properties = processor->GetProperties();
	if (properties == nullptr)
	{
		throw CConstructError(0, ConstructErrorPropertiesNotFound, L"No property enumerator found in processor.");
	}

	auto property = properties->GetProperty(property_id);
	if (property == nullptr)
	{
		wostringstream output;
		output << wstring(L"Property not found: ") << property_id;
		throw CConstructError(0, ConstructErrorPropertyNotFound, output.str());
	}

	auto property_type = property->GetType();
	bool result = false;
	switch (property_type)
	{
	case PropertyBool:
	{
		if (wcscmp(value, L"true") == 0)
		{
			result = processor->SetBool(property_id, true);
		}
		else if (wcscmp(value, L"false") == 0)
		{
			result = processor->SetBool(property_id, false);
		}
	}
	break;
	case PropertyInt:
	{
		result = processor->SetInt(property_id, _wtoi(value));
	}
	break;
	case PropertyFloat:
	{
		result = processor->SetFloat(property_id, _wtof(value));
	}
	break;
	case PropertyString:
	{
		wstring str(value);
		if (str.empty())
		{
			auto output = wstring(L"No string value specified for property£º") + property_id;
			throw CConstructError(0, ConstructErrorPropertyValueNotString, output.c_str());
		}

		result = processor->SetString(property_id, value);
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
		throw CConstructError(0, ConstructErrorPropertySet, output.c_str());
	}

	return result;
}

bool CPipelineConstructor::InstanceIdExists(const wchar_t * id)
{
	return _pipeline->GetProcessor(id) != nullptr;
}

bool CPipelineConstructor::LinkProperty(const wchar_t * processor_id,
	const wchar_t * property_id,
	const wchar_t * param_id)
{
	assert(_pipeline);

	auto processor = _pipeline->GetProcessor(processor_id);
	if (!processor)
	{
		auto output = wstring(L"failed to find processor:") + processor_id;
		throw CConstructError(0, ConstructErrorProcessorNotFound, output.c_str());
	}

	if (!processor->LinkProperty(property_id, param_id))
	{
		wostringstream output;
		output << L"Fail to link property to system variable. Property: " << property_id
			<< L". System variable: " << param_id;
		throw CConstructError(0, ConstructErrorPropertyLink, output.str());
	}

	return true;
}
