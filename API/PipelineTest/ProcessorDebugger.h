#pragma once

#include "Interface/Interfaces.h"

namespace Yap
{
	class YapDebugger
	{
	public:
		void DebugPort(IPortIter& ports);
	
		void DebugOutput(IProcessor& processor);

		bool DebugProperties(IPropertyIter& properties);

		bool DebugPlugin(const wchar_t * path);

	};
}

