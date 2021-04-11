///	@file
///	@brief		xxx common library.
///	@details	Utilities for logger.
///	@pre		ISO/IEC 14882:2017
///	@author		Mura
///	@copyright	(C) 2018-, Mura. All rights reserved. (MIT License)

#ifndef xxx_LOGGER_HXX_
#define xxx_LOGGER_HXX_

#include <filesystem>
#include <vector>
#include <stdexcept>
#include <string>
#include <optional>

#if defined(__cpp_lib_source_location) && 201907L <= __cpp_lib_source_location && __has_include(<source_location>)
// uses standard source location
#include <source_location>
#define	xxx_logpos	std::source_location::current()
namespace xxx {
namespace sl {
using std::source_location;
}	// namespace sl
}	// namespace xxx
#elif 201907L <= __cplusplus && __has_include(<experimental/source_location>)
// uses experinental implementation of source location
#include <experimental/source_location>
#define	xxx_logpos	std::experimental::source_location::current()
namespace xxx {
namespace sl {
using std::experimental::source_location;
}	// namespace sl
}	// namespace xxx
#else
// uses own implementation of source location
#include <cstdint>
namespace xxx {
namespace sl {

struct source_location
{
public:
	constexpr	auto		file_name() const noexcept { return file_; }
	constexpr	auto		function_name() const noexcept { return func_; }	
	constexpr	auto		line() const noexcept { return line_; }
	constexpr	auto		column() const noexcept { return column_; }

	constexpr	source_location(char const* file, char const* func, std::uint_least32_t line, std::uint_least32_t column=0u) noexcept : file_{ file }, func_{ func }, line_{ line }, column_ { column } {}
private:
	char const*				file_{};
	char const*				func_{};
	std::uint_least32_t		line_{};
	std::uint_least32_t		column_{};
};

}	// namespace sl
}	// namespace xxx

///	@brief	Logging utility macro.
///	It can be used as parameter of the following logging methods.
#define	xxx_logpos	::xxx::sl::source_location{__FILE__, __func__, __LINE__, 0}
#endif

#if defined(xxx_no_logging)
#include <iosfwd>
#else
#include <sstream>
#include <mutex>
#endif	// xxx_no_logging

namespace xxx {

///	@brief	logger.
namespace log {

///	@brief	logging level.
enum class level_t
{
	Silent,		///< Silent (No logging).
	Fatal,		///< Fatal error.
	Error,		///< Normal error.
	Warn,		///< Warning.
	Notice,		///< Notice.
	Info,		///< Information.
	Debug,		///< Debug.
	Trace,		///< Trace.
	Verbose,	///< Verbose.
	All,		///< All (=Verbose).
};

#if ! defined(xxx_no_logging)

namespace impl {

//	Dumps arguments.
//	@param[in,out]	os		Output stream.
//	@param[in]		head	Head of argruments.
//	@param[in]		args	Other argument(s).
template<typename T, typename... Args>
inline void
dump_(std::ostream& os, T const& head, Args... args)
{
	os	<< head;
	if constexpr (0 < sizeof...(Args))
	{
		dump_(os, args...);
	}
}
//	Dumps vector arguments.
//	@param[in,out]	os		Output stream.
//	@param[in]		head	Head of argruments.
//	@param[in]		args	Other argument(s).
template <typename T, typename... Args>
inline void
dump_(std::ostream& os, std::vector<T> const& head, Args... args)
{
	os << "[";
	for (bool first {true}; auto const& arg : head)
	{
		if (!first) os << ",";
		first = false;
		dump_(os, arg);
	}
	os << "]";
	if constexpr (0 < sizeof...(Args))
	{
		dump_(os, args...);
	}
}

//	Dumps arguments with separation comma.
//	@param[in,out]	os		Output stream.
//	@param[in]		head	Head of argruments.
//	@param[in]		args	Other argument(s).
template <typename T, typename... Args>
inline void
enclose_(std::ostream& os, T const& head, Args... args)
{
	dump_(os, head);
	if constexpr (0 < sizeof...(args))
	{
		os	<< ',';
		enclose_(os, args...);
	}
}

}	// namespace impl

#endif	// xxx_no_logging

///	@brief	Encloses arguments.
///	@tparam			Args		Arguments.
///	@param[in]		args		Arguments.
///	@return		Arguments separated with comma and enclosed by parenthesis.
template<typename... Args>
inline std::string
enclose([[maybe_unused]] Args... args)
{
#if !defined(xxx_no_logging)
	std::ostringstream	oss;
	oss << '(';
	if constexpr (0 < sizeof...(args))
	{
		impl::enclose_(oss, args...);
	}
	oss	<< ')';
	return oss.str();
#else	// xxx_no_logging
	return std::string();
#endif	// xxx_no_logging
}

#if defined(xxx_no_logging)

class logger_t
{
public:
	template<typename... Args>	void	log(level_t, sl::source_location const& pos, Args...) {}
	template<typename... Args>	void	oops(sl::source_location const& pos, Args...) {}
	template<typename... Args>	void	err(sl::source_location const& pos, Args...) {}
	template<typename... Args>	void	warn(sl::source_location const& pos, Args...) {}
	template<typename... Args>	void	notice(sl::source_location const& pos, Args...) {}
	template<typename... Args>	void	info(sl::source_location const& pos, Args...)  {}
	template<typename... Args>	void	debug(sl::source_location const& pos, Args...) {}
	template<typename... Args>	void	trace(sl::source_location const& pos, Args...) {}
	template<typename... Args>	void	verbose(sl::source_location const& pos, Args...) {}
	template<typename... Args>	void	log(Args...) {}
	template<typename... Args>	void	oops(Args...) {}
	template<typename... Args>	void	err(Args...) {}
	template<typename... Args>	void	warn(Args...) {}
	template<typename... Args>	void	notice(Args...) {}
	template<typename... Args>	void	info(Args...) {}
	template<typename... Args>	void	debug(Args...) {}
	template<typename... Args>	void	trace(Args...) {}
	template<typename... Args>	void	verbose(Args...) {}
public:
	void	set_logger(std::string const&) {}
	void	set_path(std::filesystem::path const&) {}
	void	set_console(bool) {}

	auto	logger()const noexcept	{ return std::filesystem::path();	}
	auto	path()const noexcept	{ return std::string();	}
	auto	console()const noexcept	{ return false;	}
public:
	logger_t(level_t, std::filesystem::path const&, std::string const, bool) {}
	logger_t() {}
};

#else	// xxx_no_logging

///	@brief	Logger.
class logger_t
{
public:
	///	@brief	Dumps log.
	///	@tparam			Args		arguments
	///	@param[in]		level		Logging level.
	///	@param[in]		pos			Position of source.
	///	@param[in]		args		More arguments to log
	template<typename... Args>
	void
	log(level_t level, sl::source_location const& pos, Args... args)
	{
		std::ostringstream	oss;
		impl::dump_(oss, args...);
		log_(level, pos, oss.str().c_str());
	}
	///	@overload
	void
	log(level_t level, sl::source_location const& pos, char const* message)
	{
		log_(level, pos, message);
	}
	///	@overload
	void
	log(level_t level, sl::source_location const& pos, std::string const& message)
	{
		log_(level, pos, message.c_str());
	}
	///	@brief	Dumps log as fatal error.
	///	@tparam			Args		arguments
	///	@param[in]		pos			Position of source.
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	oops(sl::source_location const& pos, Args... args)
	{
		log(level_t::Fatal, pos, args...);
	}
	///	@brief	Dumps log as normal error.
	///	@tparam			Args		arguments
	///	@param[in]		pos			Position of source.
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	err(sl::source_location const& pos, Args... args)
	{
		log(level_t::Error, pos, args...);
	}
	///	@brief	Dumps log as warning.
	///	@tparam			Args		arguments
	///	@param[in]		pos			Position of source.
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	warn(sl::source_location const& pos, Args... args)
	{
		log(level_t::Warn, pos, args...);
	}
	///	@brief	Dumps log as notice.
	///	@tparam			Args		arguments
	///	@param[in]		pos			Position of source.
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	notice(sl::source_location const& pos, Args... args)
	{
		log(level_t::Notice, pos, args...);
	}
	///	@brief	Dumps log as information.
	///	@tparam			Args		arguments
	///	@param[in]		pos			Position of source.
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	info(sl::source_location const& pos, Args... args)
	{
		log(level_t::Info, pos, args...);
	}
	///	@brief	Dumps log as debug info.
	///	@tparam			Args		arguments
	///	@param[in]		pos			Position of source.
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	debug(sl::source_location const& pos, Args... args)
	{
		log(level_t::Debug, pos, args...);
	}
	///	@brief	Dumps log as trace.
	///	@tparam			Args		arguments
	///	@param[in]		pos			Position of source.
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	trace(sl::source_location const& pos, Args... args)
	{
		log(level_t::Trace, pos, args...);
	}
	///	@brief	Dumps log as verbose.
	///	@tparam			Args		arguments
	///	@param[in]		pos			Position of source.
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	verbose(sl::source_location const& pos, Args... args)
	{
		log(level_t::Verbose, pos, args...);
	}

	///	@brief	Dumps log.
	///	@tparam			Args		arguments
	///	@param[in]		level		Logging level.
	///	@param[in]		args		More arguments to log
	template<typename... Args>
	void
	log(level_t level, Args... args)
	{
		std::ostringstream	oss;
		impl::dump_(oss, args...);
		log_(level, std::nullopt, oss.str().c_str());
	}
	///	@overload
	void
	log(level_t level, char const* message)
	{
		log_(level, std::nullopt, message);
	}
	///	@overload
	void
	log(level_t level, std::string const& message)
	{
		log_(level, std::nullopt, message.c_str());
	}
	///	@brief	Dumps log as fatal error.
	///	@tparam			Args		arguments
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	oops(Args... args)
	{
		log(level_t::Fatal, args...);
	}
	///	@brief	Dumps log as normal error.
	///	@tparam			Args		arguments
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	err(Args... args)
	{
		log(level_t::Error, args...);
	}
	///	@brief	Dumps log as warning.
	///	@tparam			Args		arguments
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	warn(Args... args)
	{
		log(level_t::Warn, args...);
	}
	///	@brief	Dumps log as notice.
	///	@tparam			Args		arguments
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	notice(Args... args)
	{
		log(level_t::Notice, args...);
	}
	///	@brief	Dumps log as information.
	///	@tparam			Args		arguments
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	info(Args... args)
	{
		log(level_t::Info, args...);
	}
	///	@brief	Dumps log as debug info.
	///	@tparam			Args		arguments
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	debug(Args... args)
	{
		log(level_t::Debug, args...);
	}
	///	@brief	Dumps log as trace.
	///	@tparam			Args		arguments
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	trace(Args... args)
	{
		log(level_t::Trace, args...);
	}
	///	@brief	Dumps log as verbose.
	///	@tparam			Args		arguments
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template<typename... Args>
	void
	verbose(Args... args)
	{
		log(level_t::Verbose, args...);
	}
public:
	///	@brief	Sets the external logger name as the following:
	///		- [xxx_win32]	dump to debugger (in debug mode)
	///		- [xxx_posix]	dump to syslog
	///	@param[in]		logger		Logger name
	void	set_logger(std::string const& logger)		{ std::lock_guard	l{ mutex_ };	logger_ = logger; }
	///	@brief	Sets log file.
	///	@param[in]		path		The path of log name.
	void	set_path(std::filesystem::path const& path)	{ std::lock_guard	l{ mutex_ };	path_		= path;		}
	///	@brief	Sets whether dump it to standard error or not.
	///	@param[in]		on		Whether dump it to standart error or not..
	void	set_console(bool on)						{ std::lock_guard	l{ mutex_ };	console_	= on;		}
	///	@brief	Sets loggihng level.
	///	@param[in]		level		Logger level.
	void	set_level(level_t level)					{ std::lock_guard	l{ mutex_ };	level_	= level;	}

	///	@brief	Gets the external logger name.
	///	@return		External logger name.
	auto const&		logger()const noexcept	{ return logger_;	}
	///	@brief	Gets the path of log file.
	///	@return		The path of log file.
	auto const&		path()const noexcept	{ return path_;		}
	///	@brief	Gets whether dump it to standard error or not.
	///	@return		If standard error is available, it returns true;
	///				otherwise, it return false.
	auto			console()const noexcept	{ return console_;	}
public:
	///	@brief	Constructor.
	///	@param[in]		level		Logger level.
	///	@param[in]		path		The path of log file.
	///	@param[in]		logger		External logger name.
	///	@param[in]		console		Whether dump it to standard error or not.
	logger_t(level_t level, std::filesystem::path const& path, std::string const&logger, bool console) :
		level_{ level }, path_{ path }, logger_{ logger }, console_{ console }, mutex_{}
	{}
	///	@brief	Constructor.
	logger_t() :
		level_{ level_t::Info }, path_{}, logger_{}, console_{ true }, mutex_{}
	{}
private:
	void	log_(level_t level, std::optional<sl::source_location> const& pos, char const* message);
private:
	level_t					level_;		///< Logger level.
	std::filesystem::path	path_;		///< The path of log file.
	std::string				logger_;	///< External logger name.
	bool					console_;	///< Whether dump it to standard error or not.
	std::mutex				mutex_;		///< Mutex.
};

#endif	// xxx_no_logging

#if defined(xxx_no_logging)

inline void			add_logger(std::string const&, level_t, std::filesystem::path const&, std::string const&, bool) {}
inline void			remove_logger(std::string const&) {}
inline logger_t		logger(std::string const&) { return logger_t();	}

#else	// xxx_no_logging

///	@brief	Adds a new logger.
///	@param[in]		tag			New tag of logger.
///	@param[in]		level		Filter level.
///	@param[in]		path		The path of log file.
///	@param[in]		logger		External logger name.
///	@param[in]		console		Whether dump it to standard error or not.
void		add_logger(std::string const& tag, level_t level, std::filesystem::path const& path, std::string const& logger, bool console);
///	@brief	Removes existing logger.
///	@param[in]		tag			Tag of logger to remove.
void		remove_logger(std::string const& tag);
///	@brief	Gets the logger.
///	@param[in]		tag			Tag of logger.
///	@return			Logger.
logger_t&	logger(std::string const& tag);

#endif	// xxx_no_logging

#if defined(xxx_no_logging)

class tracer_t
{
public:
	template<typename... Args>	tracer_t(logger_t&, sl::source_location const& pos, Args...) {}
	template<typename... Args>	tracer_t(logger_t&, level_t, sl::source_location const& pos, Args...) {}

	template<typename T>	void	set_result(T) {}
};

#else	// xxx_no_logging

///	@brief	Log tracer.
class tracer_t
{
public:
	///	@brief	Dumps log at entering into the scope.
	///	@param[in]		logger		Logger to dump.
	///	@param[in]		pos			Position of source.
	///	@param[in]		args		More agruments.
	///	@see			xxx_logpos
	template<typename... Args>
	tracer_t(logger_t& logger, sl::source_location const& pos, Args... args) :
		tracer_t(logger, level_t::Trace, pos, args...)
	{}
	///	@brief	Dumps log at entering into the scope.
	///	@param[in]		logger		Logger to dump.
	///	@param[in]		level		Logger level.
	///	@param[in]		pos			Position of source.
	///	@param[in]		args		More agruments.
	///	@see			xxx_logpos
	template<typename... Args>
	tracer_t(logger_t& logger, level_t level, sl::source_location const& pos, Args... args) :
		logger_{ logger },
		pos_{ pos },
		result_{},
		level_{ level }
	{
		logger_.log(level, pos, ">>>", enclose(args...));
	}	
	///	@brief	Dumps log at leaving from the scope.
	~tracer_t()
	{
		logger_.log(level_, pos_, "<<<", enclose(result_));
	}
	///	@brief	Dumps log as trace level.
	///	@tparam			Args		arguments
	///	@param[in]		args		More arguments to log
	///	@see			xxx_logpos
	template <typename... Args>
	void
	trace(Args... args)
	{
		logger_.log(level_, pos_, "--- ", args...);
	}
	///	@brief	Sets result of the method.
	///	@param[in]		result		Result of the method.
	template<typename T>	void	set_result(T result){	std::ostringstream	oss;	oss	<< result;	result_	= oss.str();	}
private:
	logger_t&				logger_;	///< Logger.
	sl::source_location		pos_;		///< Position of source.
	std::string				result_;	///< Result.
	level_t					level_;		///< Trace level.
private:
	tracer_t(tracer_t const&)						= delete;
	tracer_t const&		operator =(tracer_t const&)	= delete;
};

///	@overload
template<>	inline void		tracer_t::set_result(char const* result) { result_ = result; }
///	@overload
template<>	inline void		tracer_t::set_result(std::string const& result) { result_ = result; }

#endif	// xxx_no_logging

}	// namespace log
}	// namespace xxx

#endif	// xxx_LOGGER_HXX_
