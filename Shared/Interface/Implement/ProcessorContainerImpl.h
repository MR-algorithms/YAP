#ifndef ProcessorManagerImp_h__20160813
#define ProcessorManagerImp_h__20160813

#pragma once

#include "Interface/IProcessor.h"
#include <map>

namespace Yap
{
	class ProcessorContainerImpl :
		public IProcessorContainer, public IDynamicObject
	{
		class ProcessorIterator : public IProcessorIter
		{
			explicit ProcessorIterator(ProcessorContainerImpl& container);
			// Inherited via IIterator
			virtual IProcessor * GetFirst() override;
			virtual IProcessor * GetNext() override;

			ProcessorContainerImpl& _container;
			std::map<std::wstring, IProcessor*>::iterator _current;
			friend class ProcessorContainerImpl;
		};

	public:
		ProcessorContainerImpl();
		~ProcessorContainerImpl();

		virtual IProcessor * GetProcessor(const wchar_t * name) override;
		virtual IProcessorIter * GetIterator() override;
		virtual void DeleteThis() override;

		bool AddProcessor(IProcessor * processor);

	private:
		std::map<std::wstring, IProcessor*> _processors;
	};
}
#endif // ProcessorManagerImp_h__
