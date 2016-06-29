#pragma once
#include <memory>

namespace Yap
{
	class CCompositeProcessor;
	class CModuleManager;
	class CProcessorAgent;

	class CPipelineConstructor
	{
	public:
		CPipelineConstructor();
		virtual ~CPipelineConstructor();
		std::shared_ptr<CProcessorAgent> CreateProcessor(const wchar_t * class_id, const wchar_t * instance_id);

	protected:
		std::shared_ptr<CCompositeProcessor> _pipeline;
		std::shared_ptr<CModuleManager> _module_manager;
	};

}
