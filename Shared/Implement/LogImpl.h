#ifndef LOGIMPL_H_20170425
#define LOGIMPL_H_20170425
#pragma once

#include "Interface/Interfaces.h"

#include <map>

namespace log4cplus { class Logger; }

namespace Yap
{
	class LogImpl :
		public ILog
	{
	public:
		~LogImpl();

		virtual void Log(const wchar_t * module, const wchar_t * info, LogLevel level, const wchar_t * log_name = L"", bool flush = true) override;
		static LogImpl& GetInstance();

	protected:
		LogImpl();
		static bool Init();
		static std::shared_ptr<LogImpl> s_instance;

		std::map<std::wstring, log4cplus::Logger> _loggers;
	};
}

#endif // LOGIMPL_H_