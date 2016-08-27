#ifndef ProcessorImp_h__20160813
#define ProcessorImp_h__20160813

#pragma once
#include "Interface/IProcessor.h"

#include <map>
#include <string>
#include <memory>
#include "Utilities/macros.h"
#include "interface/Implement/DataImp.h"
#include "Interface/IProperties.h"
#include "Interface/IMemory.h"

namespace Yap
{
	class CPropertyEnumeratorImp;

	class CPort : public IPort
	{
	public:
		CPort(const wchar_t * name, unsigned int dimensions, int data_type);
		virtual const wchar_t * GetName() override;
		virtual unsigned int GetDimensions() override;
		virtual int GetDataType() override;

	protected:
		std::wstring _name;
		unsigned int _dimensions;
		int _data_type;

	};

	class CPortEnumerator : public IPortEnumerator
	{
	public:
		bool AddPort(const wchar_t * name, unsigned int dimensions, int data_type);

		virtual IPort * GetFirstPort() override;
		virtual IPort * GetNextPort() override;

		virtual IPort * GetPort(const wchar_t * name) override;

	protected:
		std::map<std::wstring, std::shared_ptr<IPort>> _ports;
		std::map<std::wstring, std::shared_ptr<IPort>>::iterator _current_port;
	};

	struct Anchor
	{
		IProcessor * processor;
		std::wstring port;
		Anchor(IProcessor* processor_, const wchar_t * in_port_) : processor(processor_), port(in_port_) {}
	};


	struct PropertyException
	{
		enum Type
		{
			PropertyNotFound,
			TypeNotMatch,
		};
		std::wstring property_name;
		Type type;
		PropertyException(const wchar_t * name, Type type_) : property_name(name), type(type_) {}
	};

	class CProcessorImp :
		public IProcessor, public IDynamicObject
	{
	public:
		explicit CProcessorImp(const wchar_t * class_id);
		CProcessorImp(const CProcessorImp& rhs);

		/// 释放当前Processor的资源。
		virtual void Delete() override;

		virtual IPortEnumerator * GetInputPortEnumerator() override;
		virtual IPortEnumerator * GetOutputPortEnumerator() override;

		bool Init();
		virtual bool OnInit() { return true; };
		virtual const wchar_t * GetClassId() override;
		virtual void SetClassId(const wchar_t * id) override;
		virtual const wchar_t * GetInstanceId() override;
		virtual void SetInstanceId(const wchar_t * instance_id) override;

		virtual IPropertyEnumerator * GetProperties() override;

		virtual bool LinkProperty(const wchar_t * property_id, const wchar_t * param_id) override;
		virtual bool UpdateProperties(IPropertyEnumerator * params) override;

		virtual bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input) override;

	protected:
		bool CanLink(const wchar_t * source_output_name, IProcessor * next, const wchar_t * next_input_name);
		bool OutportLinked(const wchar_t * out_port_name) const;

		bool AddInputPort(const wchar_t * name, unsigned int dimensions, int data_type);
		bool AddOutputPort(const wchar_t * name, unsigned int dimensions, int data_type);

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

		CPortEnumerator _input_ports;
		CPortEnumerator _output_ports;

		std::shared_ptr<CPropertyEnumeratorImp> _properties;

		std::multimap<std::wstring, Anchor> _links;
		std::map<std::wstring, std::wstring> _property_links;

		std::wstring _instance_id;
		std::wstring _class_id;
		IPropertyEnumerator * _system_variables;

		virtual ~CProcessorImp();
	private:
		CProcessorImp(const CProcessorImp&& rhs);
		const CProcessorImp& operator = (CProcessorImp&& rhs);
		const CProcessorImp& operator = (const CProcessorImp& rhs);
	};
};

#endif // ProcessorImp_h__
