///	@file
///	@brief		xxx common library.
///	@details	Utilities for logger.
///	@pre		ISO/IEC 14882:2017
///	@author		Mura
///	@copyright	(C) 2018-, Mura. All rights reserved. (MIT License)

#ifndef xxx_LOGGER_HXX_
#define xxx_LOGGER_HXX_

#include <filesystem>
#include <stdexcept>
#include <string>
#include <mutex>

#ifndef xxx_no_logging
#define xxx_no_logging
#endif

#if defined(xxx_no_logging)
#include <iosfwd>
#else
#include <sstream>

#endif	// xxx_no_logging

namespace xxx {

///	@brief	logger.
namespace log {

///	@brief	logging level.
enum class level_t
{
	Silent,		///< Silent (No logging)
	Fatal,		///< Fatal error
	Error,		///< Normal error
	Warn,		///< Warning
	Notice,		///< Notice
	Info,		///< Information
	Debug,		///< Debug
	Trace,		///< Trace
	Verbose,	///< Verbose
	All,		///< All (=Verbose)
};

///	@brief	Logging utility macro.
///	It can be used as parameter of the following logging methods.
#define xxx_log		__FILE__, __LINE__, __func__

#if ! defined(xxx_no_logging)

namespace impl {

//	Dumps arguments.
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

//	Dumps arguments with separation comma.
template<typename T, typename... Args>
inline void
enclose_(std::ostream& os, T const& head, Args... args)
{
	os	<< head;
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
#if defined(xxx_no_logging)
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
	template<typename... Args>	void	log(level_t, char const*, long, char const*, Args...) {}
	template<typename... Args>	void	oops(char const*, long, char const*, Args...) {}
	template<typename... Args>	void	err(char const*, long, char const*, Args...) {}
	template<typename... Args>	void	warn(char const*, long, char const*, Args...) {}
	template<typename... Args>	void	notice(char const*, long, char const*, Args...) {}
	template<typename... Args>	void	info(char const*, long, char const* function, Args...)  {}
	template<typename... Args>	void	debug(char const*, long, char const* function, Args...) {}
	template<typename... Args>	void	trace(char const*, long, char const*, Args...) {}
	template<typename... Args>	void	verbose(char const*, long, char const*, Args...) {}
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
	///	@param[in]		file		source name such as __FILE__
	///	@param[in]		line		source line such as __LINE__
	///	@param[in]		function	function name such as __func__
	///	@param[in]		args		More arguments to log
	template<typename... Args>
	void
	log(level_t level, char const* file, long line, char const* function, Args... args)
	{
		std::ostringstream	oss;
		impl::dump_(oss, args...);
		log_(level, file, line, function, oss.str().c_str());
	}
	///	@brief	Dumps log as fatal error.
	///	@tparam			Args		arguments
	///	@param[in]		file		source name such as __FILE__
	///	@param[in]		line		source line such as __LINE__
	///	@param[in]		function	function name such as __func__
	///	@param[in]		args		More arguments to log
	///	@see			xxx_log
	template<typename... Args>
	void
	oops(char const* file, long line, char const* function, Args... args)
	{
		log(level_t::Fatal, file, line, function, args...);
	}
	///	@brief	Dumps log as normal error.
	///	@tparam			Args		arguments
	///	@param[in]		file		source name such as __FILE__
	///	@param[in]		line		source line such as __LINE__
	///	@param[in]		function	function name such as __func__
	///	@param[in]		args		More arguments to log
	///	@see			xxx_log
	template<typename... Args>
	void
	err(char const* file, long line, char const* function, Args... args)
	{
		log(level_t::Error, file, line, function, args...);
	}
	///	@brief	Dumps log as warning.
	///	@tparam			Args		arguments
	///	@param[in]		file		source name such as __FILE__
	///	@param[in]		line		source line such as __LINE__
	///	@param[in]		function	function name such as __func__
	///	@param[in]		args		More arguments to log
	///	@see			xxx_log
	template<typename... Args>
	void
	warn(char const* file, long line, char const* function, Args... args)
	{
		log(level_t::Warn, file, line, function, args...);
	}
	///	@brief	Dumps log as notice.
	///	@tparam			Args		arguments
	///	@param[in]		file		source name such as __FILE__
	///	@param[in]		line		source line such as __LINE__
	///	@param[in]		function	function name such as __func__
	///	@param[in]		args		More arguments to log
	///	@see			xxx_log
	template<typename... Args>
	void
	notice(char const* file, long line, char const* function, Args... args)
	{
		log(level_t::Notice, file, line, function, args...);
	}
	///	@brief	Dumps log as information.
	///	@tparam			Args		arguments
	///	@param[in]		file		source name such as __FILE__
	///	@param[in]		line		source line such as __LINE__
	///	@param[in]		function	function name such as __func__
	///	@param[in]		args		More arguments to log
	///	@see			xxx_log
	template<typename... Args>
	void
	info(char const* file, long line, char const* function, Args... args)
	{
		log(level_t::Info, file, line, function, args...);
	}
	///	@brief	Dumps log as debug info.
	///	@tparam			Args		arguments
	///	@param[in]		file		source name such as __FILE__
	///	@param[in]		line		source line such as __LINE__
	///	@param[in]		function	function name such as __func__
	///	@param[in]		args		More arguments to log
	///	@see			xxx_log
	template<typename... Args>
	void
	debug(char const* file, long line, char const* function, Args... args)
	{
		log(level_t::Debug, file, line, function, args...);
	}
	///	@brief	Dumps log as trace.
	///	@tparam			Args		arguments
	///	@param[in]		file		source name such as __FILE__
	///	@param[in]		line		source line such as __LINE__
	///	@param[in]		function	function name such as __func__
	///	@param[in]		args		More arguments to log
	///	@see			xxx_log
	template<typename... Args>
	void
	trace(char const* file, long line, char const* function, Args... args)
	{
		log(level_t::Trace, file, line, function, args...);
	}
	///	@brief	Dumps log as verbose.
	///	@tparam			Args		arguments
	///	@param[in]		file		source name such as __FILE__
	///	@param[in]		line		source line such as __LINE__
	///	@param[in]		function	function name such as __func__
	///	@param[in]		args		More arguments to log
	///	@see			xxx_log
	template<typename... Args>
	void
	verbose(char const* file, long line, char const* function, Args... args)
	{
		log(level_t::Verbose, file, line, function, args...);
	}
public:
	///	@brief	Sets the external logger name as the following:
	///		- [xxx_win32]	dump to debugger (in debug mode)
	///		- [xxx_posix]	dump to syslog
	///	@param[in]		logger		Logger name
	void	set_logger(std::string const& logger)		{ logger_	= logger;	}
	///	@brief	Sets log file.
	///	@param[in]		path		The path of log name.
	void	set_path(std::filesystem::path const& path)	{ path_		= path;		}
	///	@brief	Sets whether dump it to standard error or not.
	///	@param[in]		on		Whether dump it to standart error or not..
	void	set_console(bool on)						{ console_	= on;		}

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
	void	log_(level_t level, char const* file, long line, char const* function, char const* message);
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
	template<typename... Args>	tracer_t(logger_t&, char const*, long, char const*, Args...) {}
	template<typename... Args>	tracer_t(logger_t&, level_t, char const*, long, char const*, Args...) {}

	template<typename T>	void	set_result(T) {}
	template<>				void	set_result(char const*) {}
	template<>				void	set_result(std::string const&) {}
};

#else	// xxx_no_logging

///	@brief	Log tracer.
class tracer_t
{
public:
	///	@brief	Dumps log at entering into the scope.
	///	@param[in]		logger		Logger to dump.
	///	@param[in]		file		Source name.
	///	@param[in]		line		Source line.
	///	@param[in]		function	Function name.
	///	@param[in]		args		More agruments.
	///	@see			xxx_log
	template<typename... Args>
	tracer_t(logger_t& logger, char const* file, long line, char const* function, Args... args) :
		tracer_t(logger, level_t::Trace, file, line, function, args...)
	{}
	///	@brief	Dumps log at entering into the scope.
	///	@param[in]		logger		Logger to dump.
	///	@param[in]		level		Logger level.
	///	@param[in]		file		Source name.
	///	@param[in]		line		Source line.
	///	@param[in]		function	Function name.
	///	@param[in]		args		More agruments.
	///	@see			xxx_log
	template<typename... Args>
	tracer_t(logger_t& logger, level_t level, char const* file, long line, char const* function, Args... args) :
		logger_{ logger },
		file_{ file == nullptr ? "" : file },
		function_{ function == nullptr ? "" : function },
		result_{},
		line_{ line },
		level_{ level }
	{
		logger_.log(level, file_.c_str(), line_, function_.c_str(), ">>>[[ ", function_, enclose(args...), " ]]");
	}
	///	@brief	Dumps log at leaving from the scope.
	~tracer_t()
	{
		logger_.log(level_, file_.c_str(), line_, function_.c_str(), "<<<[[ ", function_, enclose(result_), " ]]");
	}

	///	@brief	Sets result of the method.
	///	@param[in]		result		Result of the method.
	template<typename T>	void	set_result(T result){	std::ostringstream	oss;	oss	<< result;	result_	= oss.str();	}
	///	@overload
	template<>				void	set_result(char const* result)			{	result_	= result;	}
	///	@overload
	template<>				void	set_result(std::string const& result)	{	result_	= result;	}
private:
	logger_t&		logger_;	///< Logger.
	std::string		file_;		///< File name.
	std::string		function_;	///< Function name.
	std::string		result_;	///< Result.
	long			line_;		///< Line.
	level_t			level_;		///< Trace level.
private:
	tracer_t(tracer_t const&)						= delete;
	tracer_t const&		operator =(tracer_t const&)	= delete;
};

#endif	// xxx_no_logging

}	// namespace log
}	// namespace xxx

#endif	// xxx_LOGGER_HXX_
