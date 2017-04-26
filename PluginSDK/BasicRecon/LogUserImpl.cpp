#include "stdafx.h"
#include "LogUserImpl.h"

using namespace Yap;
LogUserImpl::LogUserImpl()
{
	_module = nullptr;
	_log_name = L"sys.log";
	_flush = true;
}


LogUserImpl::~LogUserImpl()
{
}

void Yap::LogUserImpl::SetLog(ILog * log)
{
	_log = log;
}

void Yap::LogUserImpl::Trace(const wchar_t * info)
{
	assert(_log != nullptr && L"must SetLog!");
	_log->Log(_module, info, LevelTrace, _log_name, _flush);
}

void Yap::LogUserImpl::Debug(const wchar_t * info)
{
	assert(_log != nullptr && L"must SetLog!");
	_log->Log(_module, info, LevelDebug, _log_name, _flush);
}

void Yap::LogUserImpl::Info(const wchar_t * info)
{
	assert(_log != nullptr && L"must SetLog!");
	_log->Log(_module, info, LevelInfo, _log_name, _flush);
}

void Yap::LogUserImpl::Warn(const wchar_t * info)
{
	assert(_log != nullptr && L"must SetLog!");
	_log->Log(_module, info, LevelWarn, _log_name, _flush);
}

void Yap::LogUserImpl::Error(const wchar_t * info)
{
	assert(_log != nullptr && L"must SetLog!");
	_log->Log(_module, info, LevelError, _log_name, _flush);
}

void Yap::LogUserImpl::Fatal(const wchar_t * info)
{
	assert(_log != nullptr && L"must SetLog!");
	_log->Log(_module, info, LevelFatal, _log_name, _flush);
}

void Yap::LogUserImpl::SetModule(const wchar_t * module)
{
	_module = module;
}

void Yap::LogUserImpl::SetLogName(const wchar_t * log_name)
{
	_log_name = log_name;
}

void Yap::LogUserImpl::SetFlush(bool flush)
{
	_flush = flush;
}
