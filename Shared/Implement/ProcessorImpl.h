#ifndef ProcessorImp_h__20160813
#define ProcessorImp_h__20160813

#pragma once
#include "Interface/Interfaces.h"

#include <map>
#include <string>
#include <memory>
#include "Utilities/macros.h"
#include "Implement/DataObject.h"
#include "Implement/ContainerImpl.h"
#include "VariableSpace.h"

namespace Yap
{
	class VariableSpace;

	struct Anchor
	{
		IProcessor * processor;
		std::wstring port;
		Anchor(IProcessor* processor_, const wchar_t * in_port_) : processor(processor_), port(in_port_) {}
	};

	class ProcessorImpl :
		public IProcessor
	{
	public:
		explicit ProcessorImpl(const wchar_t * class_id);
		ProcessorImpl(const ProcessorImpl& rhs);

		virtual IPortContainer * Inputs() override;
		virtual IPortContainer * Outputs() override;

		virtual const wchar_t * GetClassId() const override;
		virtual void SetClassId(const wchar_t * id) override;
		virtual const wchar_t * GetInstanceId() const override;
		virtual void SetInstanceId(const wchar_t * instance_id) override;

		virtual IVariableContainer * GetProperties();

		virtual bool MapProperty(const wchar_t * property_id, const wchar_t * param_id, bool input, bool output) override;
		virtual bool SetGlobalVariables(IVariableContainer * params) override;

		virtual bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input) override;

	protected:
		/// Protect destructor to prevent this object to be created on stack.
		~ProcessorImpl();

		bool CanLink(const wchar_t * source_output_name, IProcessor * next, const wchar_t * next_input_name);
		bool OutportLinked(const wchar_t * out_port_name) const;

		bool AddInput(const wchar_t * name, unsigned int dimensions, int data_type);
		bool AddOutput(const wchar_t * name, unsigned int dimensions, int data_type);

		bool Feed(const wchar_t * name, IData * data);

		template <typename T>
		bool AddProperty(const wchar_t * property_id, T value, const wchar_t * description)
		{
			if (_properties->Add(variable_type_id<T>::type, property_id, description))
			{
				_properties->Set<T>(property_id, value);
				return true;
			}

			return false;
		}

		template <typename T>
		T GetProperty(const wchar_t * property_id)
		{
			if (!_system_variables)
			{
				return _properties->Get<T>(property_id);
			}
			else
			{
				auto link = _in_property_mapping.find(property_id);
				if (link == _in_property_mapping.end())
				{
					return _properties->Get<T>(property_id);
				}
				else
				{
					return _system_variables->Get<T>(link->second.c_str());
				}
			}

			return _properties->Get<T>(property_id);
		}

		template <typename T> 
		void SetProperty(const wchar_t * property_id, T value)
		{
			if (!_system_variables)
			{
				_properties->Set<T>(property_id, value);
			}
			else
			{
				auto link = _out_property_links.find(property_id);
				if (link == _out_property_links.end())
				{
					_properties->Set<T>(property_id, value);
				}
				else
				{
					_system_variables->Set<T>(link->second, value);
				}
			}
		}

		SmartPtr<ContainerImpl<IPort>> _input;
		SmartPtr<ContainerImpl<IPort>> _output;

		std::multimap<std::wstring, Anchor> _links;
		std::map<std::wstring, std::wstring> _in_property_mapping;	 // <property_id, variable_id>
		std::map<std::wstring, std::wstring> _out_property_links; // <property_id, variable_id>

		std::wstring _instance_id;
		std::wstring _class_id;


	private:
		std::shared_ptr<VariableSpace> _properties;
		std::shared_ptr<VariableSpace> _system_variables;

		ProcessorImpl(const ProcessorImpl&& rhs);
		const ProcessorImpl& operator = (ProcessorImpl&& rhs);
		const ProcessorImpl& operator = (const ProcessorImpl& rhs);
	};
};

#endif // ProcessorImp_h__
