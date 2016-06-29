#pragma once
#include "..\BasicRecon\ProcessorImp.h"

#include <string>
#include <memory>

namespace Yap
{
	class CProcessorAgent;

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
		bool AssignInPort(const wchar_t * port, IProcessor * inner_processor, const wchar_t * inner_port);
		bool AssignOutPort(const wchar_t * port, IProcessor * inner_processor, const wchar_t * inner_port);
	protected:
		std::map<std::wstring, std::shared_ptr<CProcessorAgent>> _processors;
		std::map<std::wstring, Anchor> _out_ports;
		std::map<std::wstring, Anchor> _in_ports;
	};
}

