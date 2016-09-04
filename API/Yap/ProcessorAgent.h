#pragma once

#ifndef ProcessorAgent_h__20160813
#define ProcessorAgent_h__20160813

#include "../../Shared/Interface/IProcessor.h"
#include "../../Shared/Interface/IMemory.h"
#include "../../Shared/Interface/IProperty.h"
#include "../../Shared/Interface/Implement/SharedObjectImpl.h"

namespace Yap
{
	class CProcessorAgent :
		public IProcessor, public SharedObjectImpl
	{
	public:
		CProcessorAgent(IProcessor * processor);
		virtual ~CProcessorAgent();

		virtual IProcessor * Clone() override;

		virtual const wchar_t * GetClassId() override;

		virtual void SetClassId(const wchar_t * id) override;

		virtual const wchar_t * GetInstanceId() override;

		virtual void SetInstanceId(const wchar_t * instance_id) override;

		virtual IPortContainer * Inputs() override;

		virtual IPortContainer * Outputs() override;

		virtual IPropertyContainer * GetProperties() override;

		virtual bool LinkProperty(const wchar_t * property_id, const wchar_t * param_id) override;

		virtual bool UpdateProperties(IPropertyContainer * params) override;

		virtual bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input) override;

		virtual bool Input(const wchar_t * name, IData * data) override;

		bool SetInt(const wchar_t * property_name, int value);
		bool SetBool(const wchar_t * property_name, bool value);
		bool SetFloat(const wchar_t * property_name, double value);
		bool SetString(const wchar_t* property_name, const wchar_t* value);

		operator bool();
	protected:
		SmartPtr<IProcessor> _processor;
	};
}

#endif // ProcessorAgent_h__
