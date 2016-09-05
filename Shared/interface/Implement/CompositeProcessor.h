#pragma once

#ifndef CompositeProcessor_h__20160813
#define CompositeProcessor_h__20160813

#include "ProcessorImpl.h"

#include <string>
#include <memory>
#include <map>

namespace Yap
{
	class CompositeProcessor :
		public ProcessorImpl
	{
	public:
		CompositeProcessor(const wchar_t * class_id);
		CompositeProcessor(CompositeProcessor& rhs);
		virtual ~CompositeProcessor();

		virtual IProcessor * Clone() override;

		virtual bool UpdateProperties(IPropertyContainer * params) override;

		virtual bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input) override;
		virtual bool Input(const wchar_t * port, IData * data) override;

		bool AddInputMapping(const wchar_t * port, const wchar_t * inner_processor, const wchar_t * inner_port);
		bool AddOutputMapping(const wchar_t * port, const wchar_t * inner_processor, const wchar_t * inner_port);

		bool AddProcessor(IProcessor * processor);
		IProcessor * Find(const wchar_t * instance_id);
	protected:
		std::map<std::wstring, SmartPtr<IProcessor>> _processors;
		std::map<std::wstring, Anchor> _output;
		std::map<std::wstring, Anchor> _inputs;

		// Inherited via ProcessorImpl
		virtual bool OnInit() override;
	};
}

#endif // CompositeProcessor_h__
