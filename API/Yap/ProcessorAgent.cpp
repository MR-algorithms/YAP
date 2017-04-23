#include "ProcessorAgent.h"

#include <cassert>
#include "Interface\Implement\VariableManager.h"

using namespace Yap;

ProcessorAgent::ProcessorAgent(IProcessor* processor) :
	_processor(YapShared(processor)),
	_variables{std::make_shared<VariableManager>(processor->GetProperties())}
{
}

ProcessorAgent::~ProcessorAgent()
{
}

const wchar_t * Yap::ProcessorAgent::GetClassId() const 
{
	assert(_processor);
	return _processor->GetClassId();
}

void Yap::ProcessorAgent::SetClassId(const wchar_t * id)
{
	assert(_processor);
	_processor->SetClassId(id);
}

const wchar_t * Yap::ProcessorAgent::GetInstanceId() const
{
	assert(_processor);
	return _processor->GetInstanceId();
}

void Yap::ProcessorAgent::SetInstanceId(const wchar_t * instance_id)
{
	assert(_processor);
	_processor->SetInstanceId(instance_id);
}

Yap::IPortContainer * Yap::ProcessorAgent::Inputs()
{
	assert(_processor);
	return _processor->Inputs();
}

Yap::IPortContainer * Yap::ProcessorAgent::Outputs()
{
	assert(_processor);
	return _processor->Outputs();
}

Yap::IPropertyContainer * Yap::ProcessorAgent::GetProperties()
{
	assert(_processor);
	return _processor->GetProperties();
}

bool Yap::ProcessorAgent::LinkProperty(const wchar_t * property_id, const wchar_t * param_id)
{
	assert(_processor);
	return _processor->LinkProperty(property_id, param_id);
}

bool Yap::ProcessorAgent::UpdateProperties(IPropertyContainer * params)
{
	assert(_processor);
	return _processor->UpdateProperties(params);
}

bool Yap::ProcessorAgent::Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input)
{
	assert(_processor);
	return _processor->Link(output, next, next_input);
}

bool Yap::ProcessorAgent::Input(const wchar_t * name, IData * data)
{
	assert(_processor);
	return _processor->Input(name, data);
}

#define HANDLE_EXCEPTION(statement) try{statement;}catch(PropertyException&){return false;} return true;

bool Yap::ProcessorAgent::SetInt(const wchar_t * property_name,
	int value)
{
	assert(_variables);
	HANDLE_EXCEPTION(_variables->Set<int>(property_name, value));
}

bool Yap::ProcessorAgent::SetBool(const wchar_t * property_name, bool value)
{
	assert(_variables);
	HANDLE_EXCEPTION(_variables->Set<bool>(property_name, value));
}

bool Yap::ProcessorAgent::SetFloat(const wchar_t * property_name, double value)
{
	assert(_variables);
	HANDLE_EXCEPTION(_variables->Set<double>(property_name, value));
}

bool Yap::ProcessorAgent::SetString(const wchar_t* property_name, 
	const wchar_t* value)
{
	assert(_variables);
	HANDLE_EXCEPTION(_variables->Set<const wchar_t*>(property_name, value));
}

Yap::ProcessorAgent::operator bool()
{
	return _processor;
}

