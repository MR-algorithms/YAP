#ifndef ProcessorManagerAgent_h__20160813
#define ProcessorManagerAgent_h__20160813

#pragma once
#include "Interface\YapInterfaces.h"
#include <wtypes.h>

namespace Yap
{
	class CProcessorManagerAgent :
		public IProcessorManager
	{
	public:
		CProcessorManagerAgent();

		bool Load(const wchar_t * plugin_path);

		virtual IProcessor * GetFirstProcessor() override;

		virtual IProcessor * GetNextProcessor() override;

		virtual IProcessor * GetProcessor(const wchar_t * name) override;

		virtual void Release() override;
		~CProcessorManagerAgent();

	protected:
		HINSTANCE _module;
		IProcessorManager * _manager;
	};

}
#endif // ProcessorManagerAgent_h__
