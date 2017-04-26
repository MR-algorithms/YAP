#ifndef LOGUSERIMPL_H_20170426
#define LOGUSERIMPL_H_20170426
#pragma once

#include "Interface/Interfaces.h"

namespace Yap
{
	class LogUserImpl : public ILogUser
	{
	public:
		LogUserImpl();
		~LogUserImpl();

		virtual void SetLog(ILog * log) override;

		void Trace(const wchar_t * info);
		void Debug(const wchar_t * info);
		void Info(const wchar_t * info);
		void Warn(const wchar_t * info);
		void Error(const wchar_t * info);
		void Fatal(const wchar_t * info);

		void SetModule(const wchar_t * module);
		void SetLogName(const wchar_t * log_name);
		void SetFlush(bool flush);

	protected:
		ILog * _log;
		const wchar_t * _module;
		const wchar_t * _log_name;
		bool _flush;
	};
}

#endif // LOGUSERIMPL_H_