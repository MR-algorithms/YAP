#pragma once

#ifndef ProcessorAgent_h__20160813
#define ProcessorAgent_h__20160813

#include "Interface/Interfaces.h"

namespace Yap
{
	class VariableManager;

	class ProcessorAgent
	{
	public:
		ProcessorAgent(IProcessor * processor);
		virtual ~ProcessorAgent();

	public:
		const wchar_t * GetClassId() const;
		void SetClassId(const wchar_t * id);
		const wchar_t * GetInstanceId() const;
		void SetInstanceId(const wchar_t * instance_id);

		IPortContainer * Inputs();
		IPortContainer * Outputs();
		IPropertyContainer * GetProperties();

		bool LinkProperty(const wchar_t * property_id, const wchar_t * param_id);
		bool UpdateProperties(IPropertyContainer * params);
		bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input);
		bool Input(const wchar_t * name, IData * data);

		bool SetInt(const wchar_t * property_name, int value);
		bool SetBool(const wchar_t * property_name, bool value);
		bool SetFloat(const wchar_t * property_name, double value);
		bool SetString(const wchar_t* property_name, const wchar_t* value);

		operator bool();
	protected:
		SmartPtr<IProcessor> _processor;
		std::shared_ptr<VariableManager> _variables;
	};
}

#endif // ProcessorAgent_h__
