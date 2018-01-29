#ifndef LOGIMPL_H_20170425
#define LOGIMPL_H_20170425
#pragma once

#include "Interface/Interfaces.h"

#include <map>
#include <vector>
#include <log4cplus/initializer.h>

namespace log4cplus { class Logger; }

namespace Yap
{
	class LogImpl :
		public ILog
	{
	public:
		~LogImpl();

		virtual void Log(const wchar_t * module, const wchar_t * info, LogLevel level, const wchar_t * log_name = L"", bool flush = true) override;
		virtual void AddUser(ILogUser * user) override;
		virtual void RemoveUser(ILogUser * user) override;


		static LogImpl& GetInstance();
	protected:
		LogImpl();
		static bool Init();
		static std::shared_ptr<LogImpl> s_instance;

		log4cplus::Initializer initializer;
		std::map<std::wstring, log4cplus::Logger> _loggers;
		std::vector<ILogUser*> _users;
	};
}

#endif // LOGIMPL_H_