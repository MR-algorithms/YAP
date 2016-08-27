#pragma once

namespace Yap
{
	struct IPortEnumerator;
	struct IProcessor;
	struct IPropertyEnumerator;
	
	class CProcessorDebugger
	{
	public:
		void DebugPort(IPortEnumerator& ports);
		void DebugOutput(IProcessor& processor);

		bool DebugProperties(IPropertyEnumerator * properties);

		bool DebugPlugin(const wchar_t * path);

	};
}

