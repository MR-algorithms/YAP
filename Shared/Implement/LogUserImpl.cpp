#include "LogUserImpl.h"

#include <memory>

using namespace Yap;
using namespace std;

shared_ptr<LogUserImpl> LogUserImpl::s_instance;

LogUserImpl::LogUserImpl()
{
	_log_name = L"sys.log";
	_module = L"";
	_flush = true;
}

LogUserImpl::~LogUserImpl()
{
	if (_log != nullptr)
	{
		_log->RemoveUser(this);
	}
}

LogUserImpl& LogUserImpl::GetInstance()
{
	if (!s_instance)
	{
		s_instance = shared_ptr<LogUserImpl>(new LogUserImpl);
	}
	return *s_instance;
}

bool LogUserImpl::Init(const wchar_t * default_log, const wchar_t * default_module)
{
	_module = default_module;
	_log_name = default_log;

	return true;
}

void LogUserImpl::SetLog(ILog* log)
{
	_log = log;
}

void Yap::LogUserImpl::Trace(const wchar_t * info, const wchar_t * module, const wchar_t * log_name)
{
	if (_log != nullptr)
	{
		_log->Log(module != nullptr ? module : _module.c_str(), info, LevelTrace,
			log_name != nullptr ? log_name : _log_name.c_str(), _flush);
	}
}

void Yap::LogUserImpl::Debug(const wchar_t * info, const wchar_t * module, const wchar_t * log_name)
{
	if (_log != nullptr)
	{
		_log->Log(module != nullptr ? module : _module.c_str(), info, LevelDebug,
			log_name != nullptr ? log_name : _log_name.c_str(), _flush);
	}
}

void Yap::LogUserImpl::Info(const wchar_t * info, const wchar_t * module, const wchar_t * log_name)
{
	if (_log != nullptr)
	{
		_log->Log(module != nullptr ? module : _module.c_str(), info, LevelInfo,
			log_name != nullptr ? log_name : _log_name.c_str(), _flush);
	}
}

void Yap::LogUserImpl::Warn(const wchar_t * info, const wchar_t * module, const wchar_t * log_name)
{
	if (_log != nullptr)
	{
		_log->Log(module != nullptr ? module : _module.c_str(), info, LevelWarn,
			log_name != nullptr ? log_name : _log_name.c_str(), _flush);
	}
}

void Yap::LogUserImpl::Error(const wchar_t * info, const wchar_t * module, const wchar_t * log_name)
{
	if (_log != nullptr)
	{
		_log->Log(module != nullptr ? module : _module.c_str(), info, LevelError,
			log_name != nullptr ? log_name : _log_name.c_str(), _flush);
	}
}

void Yap::LogUserImpl::Fatal(const wchar_t * info, const wchar_t * module, const wchar_t * log_name)
{
	if (_log != nullptr)
	{
		_log->Log(module != nullptr ? module : _module.c_str(), info, LevelFatal,
			log_name != nullptr ? log_name : _log_name.c_str(), _flush);
	}
}
