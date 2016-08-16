#include "ProcessorAgent.h"
#include <cassert>

using namespace Yap;

CProcessorAgent::CProcessorAgent(IProcessor* processor) :
	_processor(processor)
{
}


CProcessorAgent::~CProcessorAgent()
{
}

Yap::IProcessor * Yap::CProcessorAgent::Clone()
{
	assert(_processor != nullptr);
	return _processor->Clone();
}

void Yap::CProcessorAgent::Release()
{
	if (_processor != nullptr)
	{
		_processor->Release();
		_processor = nullptr;
	}
}

const wchar_t * Yap::CProcessorAgent::GetClassId()
{
	assert(_processor != nullptr);
	return _processor->GetClassId();
}

void Yap::CProcessorAgent::SetClassId(const wchar_t * id)
{
	assert(_processor != nullptr);
	_processor->SetClassId(id);
}

const wchar_t * Yap::CProcessorAgent::GetInstanceId()
{
	assert(_processor != nullptr);
	return _processor->GetInstanceId();
}

void Yap::CProcessorAgent::SetInstanceId(const wchar_t * instance_id)
{
	assert(_processor != nullptr);
	_processor->SetInstanceId(instance_id);
}

Yap::IPortEnumerator * Yap::CProcessorAgent::GetInputPortEnumerator()
{
	assert(_processor != nullptr);
	return _processor->GetInputPortEnumerator();
}

Yap::IPortEnumerator * Yap::CProcessorAgent::GetOutputPortEnumerator()
{
	assert(_processor != nullptr);
	return _processor->GetOutputPortEnumerator();
}

Yap::IPropertyEnumerator * Yap::CProcessorAgent::GetProperties()
{
	assert(_processor != nullptr);
	return _processor->GetProperties();
}

bool Yap::CProcessorAgent::LinkProperty(const wchar_t * property_id, const wchar_t * param_id)
{
	assert(_processor != nullptr);
	return _processor->LinkProperty(property_id, param_id);
}

bool Yap::CProcessorAgent::UpdateProperties(IPropertyEnumerator * params)
{
	assert(_processor != nullptr);
	return _processor->UpdateProperties(params);
}

bool Yap::CProcessorAgent::Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input)
{
	assert(_processor != nullptr);
	return _processor->Link(output, next, next_input);
}

bool Yap::CProcessorAgent::Input(const wchar_t * name, IData * data)
{
	assert(_processor != nullptr);
	return _processor->Input(name, data);
}

bool Yap::CProcessorAgent::SetInt(const wchar_t * property_name,
	int value)
{
	assert(_processor != nullptr);
	auto properties = _processor->GetProperties();
	assert(properties != nullptr);

	IProperty * property = properties->GetProperty(property_name);
	assert(property != nullptr && property->GetType() == PropertyInt);

	IIntValue * int_property = dynamic_cast<IIntValue*>(property);
	assert(int_property != nullptr);

	int_property->SetValue(value);

	return true;
}

bool Yap::CProcessorAgent::SetBool(const wchar_t * property_name, bool value)
{
	assert(_processor != nullptr);
	auto properties = _processor->GetProperties();
	assert(properties != nullptr);

	IProperty * property = properties->GetProperty(property_name);
	assert(property != nullptr && property->GetType() == PropertyBool);
	IBoolValue * bool_property = dynamic_cast<IBoolValue*>(property);
	assert(bool_property != nullptr);

	bool_property->SetValue(value);

	return true;
}

bool Yap::CProcessorAgent::SetFloat(const wchar_t * property_name, double value)
{
	assert(_processor != nullptr);
	auto properties = _processor->GetProperties();
	assert(properties != nullptr);

	IProperty * property = properties->GetProperty(property_name);
	assert(property != nullptr && property->GetType() == PropertyFloat);

	IFloatValue * float_property = dynamic_cast<IFloatValue*>(property);
	assert(float_property != nullptr);

	float_property->SetValue(value);

	return true;
}

bool Yap::CProcessorAgent::SetString(const wchar_t* property_name, 
	const wchar_t* value)
{
	assert(_processor != nullptr);
	auto properties = _processor->GetProperties();
	assert(properties != nullptr);

	IProperty * property = properties->GetProperty(property_name);
	assert(property != nullptr && property->GetType() == PropertyString);

	IStringValue * string_property = dynamic_cast<IStringValue*>(property);
	assert(string_property != nullptr);

	string_property->SetValue(value);

	return true;
}

Yap::CProcessorAgent::operator bool()
{
	return _processor != nullptr;
}

