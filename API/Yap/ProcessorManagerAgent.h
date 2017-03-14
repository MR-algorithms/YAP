#pragma once

#ifndef ProcessorManagerAgent_h__20160813
#define ProcessorManagerAgent_h__20160813

#include "Interface/Interfaces.h"

#include <wtypes.h>

namespace Yap
{
	class ModuleAgent
	{
	public:
		ModuleAgent();

		bool Load(const wchar_t * plugin_path);
		IProcessor * Find(const wchar_t * name);
		IProcessorIter * GetIterator();
		bool Add(const wchar_t * name, IProcessor * processor);

		~ModuleAgent();

	protected:

		HINSTANCE _module;
		SmartPtr<IProcessorContainer> _manager;
	};

}
#endif // ProcessorManagerAgent_h__
