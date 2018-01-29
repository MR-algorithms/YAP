#include "LogImpl.h"

#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/loggingmacros.h>
#include <memory>
#include <log4cplus/initializer.h>

using namespace std;
using namespace Yap;
using namespace log4cplus;
using namespace log4cplus::helpers;

shared_ptr<LogImpl> LogImpl::s_instance;

Yap::LogImpl::LogImpl()
{
}

Yap::LogImpl::~LogImpl()
{
	for (auto user : _users)
	{
		user->SetLog(nullptr);
	}
}

LogImpl& LogImpl::GetInstance()
{
	if (!s_instance)
	{
		Init();
	}
	return *s_instance;
}

void Yap::LogImpl::Log(const wchar_t * module,
	const wchar_t * info,
	LogLevel level,
	const wchar_t * log_name /*= L""*/,
	bool flush /*= false*/)
{
	auto iter = _loggers.find(log_name);
	if (iter == _loggers.end())
	{
		SharedAppenderPtr append(new RollingFileAppender(log_name, 1024 * 1024, 2, flush));
		append->setName(L"sysAppender");

		const wchar_t * pattern = L"%d{%y/%m/%d %H:%M:%S} [%-5p] - %m%n";
		std::unique_ptr<Layout> layout(new PatternLayout(pattern));

		append->setLayout(move(layout));

		Logger logger = Logger::getInstance(log_name);
		logger.addAppender(append);
		logger.setLogLevel(ALL_LOG_LEVEL);

		_loggers.insert(make_pair(log_name, logger));
	}

	wstring info_str(info);
	wstring module_str(module);
	wstring full_info = info_str + L" [" + module_str + L"]";

	switch (level)
	{
	case Yap::LevelTrace:
		LOG4CPLUS_TRACE(_loggers[log_name], full_info.c_str());
		break;
	case Yap::LevelDebug:
		LOG4CPLUS_DEBUG(_loggers[log_name], full_info.c_str());
		break;
	case Yap::LevelInfo:
		LOG4CPLUS_INFO(_loggers[log_name], full_info.c_str());
		break;
	case Yap::LevelWarn:
		LOG4CPLUS_WARN(_loggers[log_name], full_info.c_str());
		break;
	case Yap::LevelError:
		LOG4CPLUS_ERROR(_loggers[log_name], full_info.c_str());
		break;
	case Yap::LevelFatal:
		LOG4CPLUS_FATAL(_loggers[log_name], full_info.c_str());
		break;
	default:
		assert(0 && L"Level doesn't exist.");
		break;
	}
}

bool Yap::LogImpl::Init()
{
	try
	{
		s_instance = shared_ptr<LogImpl>(new LogImpl);
	}
	catch (bad_alloc&)
	{
		return false;
	}

	return true;
}

void Yap::LogImpl::AddUser(ILogUser * user)
{
	assert(user != nullptr);
	_users.push_back(user);
}

void Yap::LogImpl::RemoveUser(ILogUser * user)
{
	assert(user != nullptr);
	for (auto iter = _users.begin(); iter != _users.end(); ++iter)
	{
		if (*iter == user)
		{
			_users.erase(iter);
			return;
		}
	}
}
