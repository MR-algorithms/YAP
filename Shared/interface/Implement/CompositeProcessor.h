#pragma once

#ifndef CompositeProcessor_h__20160813
#define CompositeProcessor_h__20160813

#include "ProcessorImp.h"

#include <string>
#include <memory>
#include <map>

namespace Yap
{
	class CCompositeProcessor :
		public CProcessorImp
	{
	public:
		CCompositeProcessor(const wchar_t * class_id);
		CCompositeProcessor(CCompositeProcessor& rhs);
		virtual ~CCompositeProcessor();

		virtual IProcessor * Clone() override;

		virtual bool UpdateProperties(IPropertyEnumerator * params) override;

		virtual bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input) override;
		virtual bool Input(const wchar_t * port, IData * data) override;

		bool AssignInPort(const wchar_t * port, const wchar_t * inner_processor, const wchar_t * inner_port);
		bool AssignOutPort(const wchar_t * port, const wchar_t * inner_processor, const wchar_t * inner_port);

		bool AddProcessor(IProcessor * processor);
		IProcessor * GetProcessor(const wchar_t * instance_id);
	protected:
		std::map<std::wstring, IProcessor *> _processors;
		std::map<std::wstring, Anchor> _out_ports;
		std::map<std::wstring, Anchor> _in_ports;
	};
}

#endif // CompositeProcessor_h__
