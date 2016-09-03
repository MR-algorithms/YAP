#ifndef ProcessorImp_h__20160813
#define ProcessorImp_h__20160813

#pragma once
#include "Interface/IProcessor.h"

#include <map>
#include <string>
#include <memory>
#include "Utilities/macros.h"
#include "interface/Implement/DataImpl.h"
#include "Interface/Implement/SharedObjectImpl.h"
#include "Interface/Implement/ContainerImpl.h"
#include "Interface/IProperty.h"
#include "Interface/IMemory.h"

namespace Yap
{
	struct Anchor
	{
		IProcessor * processor;
		std::wstring port;
		Anchor(IProcessor* processor_, const wchar_t * in_port_) : processor(processor_), port(in_port_) {}
	};

	namespace Details
	{
		class PortContainer;
	};

	class ProcessorImpl :
		public IProcessor, public SharedObjectImpl
	{
	public:
		explicit ProcessorImpl(const wchar_t * class_id);
		ProcessorImpl(const ProcessorImpl& rhs);

		virtual IPortContainer * Inputs() override;
		virtual IPortContainer * Outputs() override;

		bool Init();
		virtual bool OnInit() { return true; };
		virtual const wchar_t * GetClassId() override;
		virtual void SetClassId(const wchar_t * id) override;
		virtual const wchar_t * GetInstanceId() override;
		virtual void SetInstanceId(const wchar_t * instance_id) override;

		virtual IPropertyContainer * GetProperties() override;

		virtual bool LinkProperty(const wchar_t * property_id, const wchar_t * param_id) override;
		virtual bool UpdateProperties(IPropertyContainer * params) override;

		virtual bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input) override;

	protected:
		bool CanLink(const wchar_t * source_output_name, IProcessor * next, const wchar_t * next_input_name);
		bool OutportLinked(const wchar_t * out_port_name) const;

		bool AddInput(const wchar_t * name, unsigned int dimensions, int data_type);
		bool AddOutput(const wchar_t * name, unsigned int dimensions, int data_type);

		bool Feed(const wchar_t * name, IData * data);

		bool AddProperty(PropertyType type, const wchar_t * name, const wchar_t * description);

		void SetInt(const wchar_t * name, int value);
		int GetInt(const wchar_t * name);
		void SetFloat(const wchar_t * name, double value);
		double GetFloat(const wchar_t * name);
		void SetBool(const wchar_t * name, bool value);
		bool GetBool(const wchar_t * name);
		void SetString(const wchar_t * name, const wchar_t * value);
		const wchar_t * GetString(const wchar_t * name);

		SmartPtr<ContainerImpl<IPort>> _input;
		SmartPtr<ContainerImpl<IPort>> _output;

		SmartPtr<ContainerImpl<IProperty>> _properties;

		std::multimap<std::wstring, Anchor> _links;
		std::map<std::wstring, std::wstring> _property_links;

		std::wstring _instance_id;
		std::wstring _class_id;

		IPropertyContainer * _system_variables;

		virtual ~ProcessorImpl();
	private:
		ProcessorImpl(const ProcessorImpl&& rhs);
		const ProcessorImpl& operator = (ProcessorImpl&& rhs);
		const ProcessorImpl& operator = (const ProcessorImpl& rhs);
	};
};

#endif // ProcessorImp_h__
