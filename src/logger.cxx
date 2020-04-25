///	@file
///	@brief		xxx common library.
///	@details	Utilities.
///	@pre		ISO/IEC 14882:2017
///	@author		Mura
///	@copyright	(C) 2018-, Mura. All rights reserved. (MIT License)

#include <xxx/logger.hxx>
#include <xxx/exceptions.hxx>
#include <xxx/xxx.hxx>

#include <unordered_map>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

#if defined(xxx_win32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define STRICT
#include <Windows.h>
#elif defined(xxx_posix)
#include <syslog.h>
#else
#error "No platform is specified."
#endif

//	@name	xxx
namespace xxx {
namespace log {

void
logger_t::log_(level_t level, char const* file, long line, char const* function, char const* message)
{
	validate_argument(level != level_t::Silent && level != level_t::All);
	validate_argument(file != nullptr && function != nullptr);

	if(static_cast<int>(level_) < static_cast<int>(level))
	{
		return;
	}

	char const*	Lv[]	= { " [S] ", " [F] ", " [E] ", " [W] ", " [N] ", " [I] ", " [D] ", " [T] ", " [V] ", " [A] " };

	std::ostringstream	oss;

	std::lock_guard		lock(mutex_);

	{
		auto const		now	= std::chrono::system_clock::now();
		auto const		t	= std::chrono::system_clock::to_time_t(now);
		oss	<< std::put_time(std::gmtime(&t), "%FT%T%z")
			<< Lv[static_cast<int>(level)]
			<< (message == nullptr ? "" : message)
			<< " - {{" << file << ':' << line << ' ' << function << "}}";
	}
	auto const	str		= oss.str();

	if(console_)
	{
		ignore_exceptions([&str, level]()
		{
			using namespace std::string_literals;
				
			std::string		begin;
			switch(level)
			{
			case level_t::Fatal:	begin	= "\x1b[37m\x1b[41m"s;	break;	// Red (reversed)
			case level_t::Error:	begin	= "\x1b[31m"s;			break;	// Red
			case level_t::Warn:		begin	= "\x1b[33m"s;			break;	// Yellow
			case level_t::Notice:	begin	= "\x1b[32m"s;			break;	// Green
			case level_t::Info:		begin	= "\x1b[37m"s;			break;	// White
			case level_t::Debug:	begin	= "\x1b[35m"s;			break;	// Magenta
			case level_t::Trace:	begin	= "\x1b[34m"s;			break;	// Blue
			case level_t::Verbose:	begin	= "\x1b[36m"s;			break;	// Cyan
			default:				begin	= "\x1b[0m"s;			break;
			}

			auto const	end		= "\x1b[0m"s;

			std::clog	<< begin << str << end << std::endl;
		});
	}
	if( ! path_.empty())
	{
		ignore_exceptions([&str, this]()
		{
			std::ofstream	ofs(path_, std::ios::out | std::ios::app);
			if(ofs.is_open())
			{
				ofs << str << std::endl;
			}
		});
	}
	if( ! logger_.empty())
	{
		ignore_exceptions([&str, level, this]()
		{
#if defined(xxx_win32)
			::OutputDebugStringA(("[" + logger_ + "] " + str + "\r\n").c_str());
#elif defined(xxx_posix)
			int		lv;
			switch (level)
			{
			case level_t::Fatal:	lv = LOG_CRIT;		break;
			case level_t::Error:	lv = LOG_ERR;		break;
			case level_t::Warn:		lv = LOG_WARNING;	break;
			case level_t::Notice:	lv = LOG_NOTICE;	break;
			case level_t::Info:		lv = LOG_INFO;		break;
			case level_t::Debug:	lv = LOG_DEBUG;		break;
			case level_t::Trace:	lv = LOG_DEBUG;		break;
			case level_t::Verbose:	lv = LOG_DEBUG;		break;
			default:				lv = LOG_CRIT;		break;
			}
			::openlog(logger_.c_str(), LOG_PID, LOG_USER);
			::syslog(lv, "%s", str.c_str());
#else
			// unsupported platform.
#endif
		});
	}
}

std::once_flag												logger_once_s;
std::mutex													loggers_mutex_s;
std::unordered_map<std::string, std::unique_ptr<logger_t>>	loggers_s;

void
add_logger(std::string const& tag, level_t level, std::filesystem::path const& path, std::string const& logger, bool console)
{
	validate_argument( ! tag.empty());

	std::lock_guard		lock(loggers_mutex_s);

	auto	itr	= loggers_s.find(tag);
	validate_argument(itr == std::end(loggers_s));

	loggers_s[tag]	= std::make_unique<logger_t>(level, path, logger, console);
}

void
remove_logger(std::string const& tag)
{
	validate_argument( ! tag.empty());

	std::lock_guard		lock(loggers_mutex_s);

	auto	itr	= loggers_s.find(tag);
	validate_argument(itr != std::end(loggers_s));
	loggers_s.erase(itr);
}

logger_t& logger(std::string const& tag)
{
	std::call_once(logger_once_s, []()
		{
			std::lock_guard		lock(loggers_mutex_s);
			loggers_s[""]	= std::make_unique<logger_t>();
		});

	std::lock_guard		lock(loggers_mutex_s);

	auto itr	= loggers_s.find(tag);
	validate_argument(itr != std::end(loggers_s));
	return *itr->second;
}

}	// namespace log
}	// namespace xxx

