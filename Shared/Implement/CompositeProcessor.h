#pragma once

#ifndef CompositeProcessor_h__20160813
#define CompositeProcessor_h__20160813

#include "ProcessorImpl.h"

#include <string>
#include <memory>
#include <map>
#include <set>

namespace Yap
{
	class CompositeProcessor :
		public ProcessorImpl  
	{
		IMPLEMENT_LOCK_RELEASE
	public:
		explicit CompositeProcessor(const wchar_t * class_id);
		CompositeProcessor(const CompositeProcessor& rhs);

		virtual IProcessor * Clone() const override;

		virtual bool SetGlobalVariables(IVariableContainer * params) override;

		virtual bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input) override;
		virtual bool Input(const wchar_t * port, IData * data) override;
		virtual bool ProcessTimer() override;

		bool MapInput(const wchar_t * port, const wchar_t * inner_processor, const wchar_t * inner_port);
		bool MapOutput(const wchar_t * port, const wchar_t * inner_processor, const wchar_t * inner_port);

		bool AddProcessor(IProcessor * processor);
		IProcessor * Find(const wchar_t * instance_id);
	protected:
        virtual ~CompositeProcessor();

		std::map<std::wstring, SmartPtr<IProcessor>> _processors;
		std::map<std::wstring, Anchor> _output;
		std::map<std::wstring, Anchor> _inputs;

	};

	typedef CompositeProcessor Pipeline;
}

#endif // CompositeProcessor_h__
