#include "LogImpl.h"

#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/loggingmacros.h>
#include <memory>

using namespace std;
using namespace Yap;
using namespace log4cplus;
using namespace log4cplus::helpers;

Yap::LogImpl::LogImpl()
{

}

Yap::LogImpl::~LogImpl()
{

}

void Yap::LogImpl::Log(const wchar_t * module, const wchar_t * info, LogLevel level, const wchar_t * log_name /*= L""*/, bool flush /*= false*/)
{
	SharedAppenderPtr append(new FileAppender(log_name, 16, flush));
	append->setName(L"sysAppender");

	const wchar_t * pattern = L"%d{%m/%d/%y  %H:%M:%S} [%-5p] - %m [%l]%n";
	std::unique_ptr<Layout> layout(new PatternLayout(pattern));

	append->setLayout(move(layout));

	Logger logger = Logger::getInstance(L"sys");
	logger.addAppender(append);

	logger.setLogLevel(ALL_LOG_LEVEL);
	
	switch (level)
	{
	case Yap::LevelTrace:
		LOG4CPLUS_TRACE(logger, info);
		break;
	case Yap::LevelDebug:
		LOG4CPLUS_DEBUG(logger, info);
		break;
	case Yap::LevelInfo:
		LOG4CPLUS_INFO(logger, info);
		break;
	case Yap::LevelWarn:
		LOG4CPLUS_WARN(logger, info);
		break;
	case Yap::LevelError:
		LOG4CPLUS_ERROR(logger, info);
		break;
	case Yap::LevelFatal:
		LOG4CPLUS_FATAL(logger, info);
		break;
	default:
		assert(L"Level doesn't exist.");
		break;
	}
}
