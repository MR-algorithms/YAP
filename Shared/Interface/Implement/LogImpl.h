#ifndef LOGIMPL_H_20170425
#define LOGIMPL_H_20170425
#pragma once

#include "Interface/Interfaces.h"

namespace Yap
{
	class LogImpl :
		public ILog
	{
	public:
		LogImpl();
		~LogImpl();

		virtual void Log(const wchar_t * module, const wchar_t * info, LogLevel level, const wchar_t * log_name = L"", bool flush = false) override;
	};
}

#endif // LOGIMPL_H_