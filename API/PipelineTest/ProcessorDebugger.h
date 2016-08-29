#pragma once

#include "Interface/IProperties.h"
#include "Interface/IProcessor.h"

namespace Yap
{
	class CProcessorDebugger
	{
	public:
		void DebugPort(IPortIter& ports);
	
		void DebugOutput(IProcessor& processor);

		bool DebugProperties(IPropertyIter& properties);

		bool DebugPlugin(const wchar_t * path);

	};
}

