#pragma once
#ifndef ILog_h__
#define ILog_h__

namespace Yap
{
	enum LogLevel
	{
		LevelTrace,
		LevelDebug,
		LevelInfo,
		LevelWarn,
		LevelError,
		LevelFatal,
	};

	struct ILogUser;
	struct ILog
	{
		virtual void Log(const wchar_t * module, const wchar_t * info, LogLevel level, const wchar_t * log_name = L"", bool flush = false) = 0;
		virtual void AddUser(ILogUser * user) = 0;
		virtual void RemoveUser(ILogUser * user) = 0;
	};

	struct ILogUser
	{
		virtual void SetLog(ILog * log) = 0;
	};
}

#endif // ILog_h__