#ifndef LOGUSERIMPL_H_20170426
#define LOGUSERIMPL_H_20170426
#pragma once

#include "Interface/Interfaces.h"

#define LOG_TRACE(info, module) LogUserImpl::GetInstance().Trace(info, module)

namespace Yap
{
	class LogUserImpl : public ILogUser
	{
	public:
		~LogUserImpl();

		bool Init(const wchar_t * default_module, const wchar_t * default_log);
		virtual void SetLog(ILog * log) override;

		void Trace(const wchar_t * info, const wchar_t * module = nullptr, const wchar_t * log_name = nullptr);
		void Debug(const wchar_t * info, const wchar_t * module = nullptr, const wchar_t * log_name = nullptr);
		void Info (const wchar_t * info, const wchar_t * module = nullptr, const wchar_t * log_name = nullptr);
		void Warn (const wchar_t * info, const wchar_t * module = nullptr, const wchar_t * log_name = nullptr);
		void Error(const wchar_t * info, const wchar_t * module = nullptr, const wchar_t * log_name = nullptr);
		void Fatal(const wchar_t * info, const wchar_t * module = nullptr, const wchar_t * log_name = nullptr);

		static LogUserImpl& GetInstance();
	protected:
		LogUserImpl();

		static LogUserImpl s_instance;

		ILog * _log;
		std::wstring _module;
		std::wstring _log_name;
		bool _flush;
	};
}

#endif // LOGUSERIMPL_H_