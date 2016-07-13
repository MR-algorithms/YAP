#pragma once
#include "..\Interface\YapInterfaces.h"

namespace Yap
{
	class CProcessorAgent :
		public IProcessor
	{
	public:
		CProcessorAgent(IProcessor * processor);
		virtual ~CProcessorAgent();

		virtual IProcessor * Clone() override;

		virtual void Release() override;

		virtual const wchar_t * GetClassId() override;

		virtual void SetClassId(const wchar_t * id) override;

		virtual const wchar_t * GetInstanceId() override;

		virtual void SetInstanceId(const wchar_t * instance_id) override;

		virtual IPortEnumerator * GetInputPortEnumerator() override;

		virtual IPortEnumerator * GetOutputPortEnumerator() override;

		virtual IPropertyEnumerator * GetProperties() override;

		virtual bool LinkProperty(const wchar_t * property_id, const wchar_t * param_id) override;

		virtual bool UpdateProperties(IPropertyEnumerator * params) override;

		virtual bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input) override;

		virtual bool Input(const wchar_t * name, IData * data) override;

		bool SetInt(const wchar_t * property_name, int value);
		bool SetBool(const wchar_t * property_name, bool value);
		bool SetFloat(const wchar_t * property_name, double value);
		bool SetString(const wchar_t* property_name, const wchar_t* value);

	protected:
		IProcessor * _processor;
	};
}

