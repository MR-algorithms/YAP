#pragma once

#ifndef ProcessorManagerAgent_h__20160813
#define ProcessorManagerAgent_h__20160813

#include "Interface/IProcessor.h"
#include "Interface/IMemory.h"

#include <wtypes.h>

namespace Yap
{
	class ModuleAgent :
		public IProcessorContainer
	{
	public:
		ModuleAgent();

		bool Load(const wchar_t * plugin_path);

		virtual IProcessor * GetProcessor(const wchar_t * name) override;
		virtual IProcessorIter * GetIterator() override;

		~ModuleAgent();

	protected:

		HINSTANCE _module;
		std::shared_ptr<IProcessorContainer> _manager;
	};

}
#endif // ProcessorManagerAgent_h__
