///	@file
///	@brief		Main entry of test.
///	@pre		ISO/IEC 14882:2017
///	@author		Mura
///	@copyright	(C) 2018-, Mura. All rights reserved.

#include <xxx/xxx.hxx>
#include <xxx/exceptions.hxx>
#include <xxx/finally.hxx>
#include <xxx/logger.hxx>
#include <xxx/str.hxx>

#include <stdexcept>
#include <iostream>
#include <iomanip>

void
test_version()
{
	std::cout	<< "---[" << __func__ << "]---" << std::endl;

	auto const	head_version	= xxx_version;
	auto const	body_version	= xxx::get_version();
	
	std::cout	<< std::hex;
	
	std::cout	<< "head:" << head_version << std::endl
				<< "body:" << body_version << std::endl;

	std::cout	<< std::dec;
	
	std::cout	<< "head ver. " << xxx::get_major_version(head_version)
				<< '.' << xxx::get_minor_version(head_version)
				<< '.' << xxx::get_revision(head_version)
				<< '.' << xxx::get_extra_version(head_version) << std::endl;
	std::cout	<< "body ver. " << xxx::get_major_version(body_version)
				<< '.' << xxx::get_minor_version(body_version)
				<< '.' << xxx::get_revision(body_version)
				<< '.' << xxx::get_extra_version(body_version) << std::endl;

	std::cout	<< (xxx_version == xxx::get_version() ? "match" : "unmatch") << std::endl;
}

void
test_exception_dump()
{
	std::cout	<< "---[" << __func__ << "]---" << std::endl;

	// To use << operator; otherwise, xxx::dump_exception() is available instead.
	using namespace	xxx::exception_iostream;

	// Plain
	{
		std::exception	e;
		xxx::dump_exception(std::cout, e);
		std::cout	<< e << std::endl;
	}

	{
		std::runtime_error	e{"test"};
		xxx::dump_exception(std::cout, e);
		std::cout	<< e << std::endl;
	}

	// Nested
	try
	{
		try
		{
			try
			{
				(void)std::string().at(1);	// Raise an exception focefully.
			}
			catch(std::exception const& e)
			{
				std::throw_with_nested(e);
			}
		}
		catch(std::exception const& e)
		{
			std::throw_with_nested(e);
		}
	}
	catch(std::exception const& e)
	{
		xxx::dump_exception(std::cout, e);
		std::cout	<< e << std::endl;
	}
}

void
test_exception_handling()
{
	std::cout	<< "---[" << __func__ << "]---" << std::endl;
	
	xxx::ignore_exceptions([]()
	{
		throw std::runtime_error(__func__);
	});
	
	xxx::ignore_exceptions([]()
	{
		throw std::runtime_error(__func__);
	},
	[](std::exception const& e)
	{
		xxx::ignore_exceptions([&e]()
		{
			std::cout << "an exception occurred:" << e.what() << std::endl;
		});
	});

	auto	e	= xxx::suppress_exceptions([]()
	{
		throw std::runtime_error("any exception");
	});
	if(e)
	{
		try
		{
			std::rethrow_exception(e);
		}
		catch(std::exception const& ee)
		{
			std::cout	<< ee.what() << std::endl;
		}
	}
	e	= xxx::suppress_exceptions([]()
	{
		;	// nothing
	});
	if( ! e)
	{
		std::cout << "no exception occurred" << std::endl;
	}
}

void
test_finally()
{
	std::cout	<< "---[" << __func__ << "]---" << std::endl;

	{
		xxx::finally_t	finally([](){ std::cout << "finally" << std::endl; });

		std::cout << "do something" << std::endl;
	}

	try
	{
		xxx::finally_t	finally([](){ std::cout << "finally" << std::endl; });

		throw std::runtime_error("an exception occurred");
	}
	catch(std::exception const& e)
	{
		std::cout << e.what() << std::endl;
	}
}

void
test_logger()
{
	std::cout << "---[" << __func__ << "]---" << std::endl;

	auto& logger = xxx::log::logger("");

	logger.oops(xxx_logpos, "oops");
	logger.err(xxx_logpos, "err");
	logger.warn(xxx_logpos, "warn");
	logger.notice(xxx_logpos, "notice");
	logger.info(xxx_logpos, "info");
	logger.trace(xxx_logpos, "trace");
	logger.debug(xxx_logpos, "debug");
	logger.verbose(xxx_logpos, "verbose");
	logger.log(xxx::log::level_t::Fatal, xxx_logpos, "oops");
	logger.log(xxx::log::level_t::Error, xxx_logpos, "err");
	logger.log(xxx::log::level_t::Warn, xxx_logpos, "warn");
	logger.log(xxx::log::level_t::Notice, xxx_logpos, "notice");
	logger.log(xxx::log::level_t::Info, xxx_logpos, "info");
	logger.log(xxx::log::level_t::Trace, xxx_logpos, "trace");
	logger.log(xxx::log::level_t::Debug, xxx_logpos, "debug");
	logger.log(xxx::log::level_t::Verbose, xxx_logpos, "verbose");

	{
		xxx::log::tracer_t	l(logger, xxx_logpos);
	}
	{
		xxx::log::tracer_t	l(logger, xxx_logpos, "arg");
	}
	{
		xxx::log::tracer_t	l(logger, xxx_logpos, "arg1", 2);
	}
	{
		xxx::log::tracer_t	f(logger, xxx::log::level_t::Fatal, xxx_logpos, "oops");
		xxx::log::tracer_t	e(logger, xxx::log::level_t::Error, xxx_logpos, "err");
		xxx::log::tracer_t	w(logger, xxx::log::level_t::Warn, xxx_logpos, "warn");
		xxx::log::tracer_t	n(logger, xxx::log::level_t::Notice, xxx_logpos, "notice");
		xxx::log::tracer_t	i(logger, xxx::log::level_t::Info, xxx_logpos, "info");
		xxx::log::tracer_t	t(logger, xxx::log::level_t::Trace, xxx_logpos, "trace");
		xxx::log::tracer_t	d(logger, xxx::log::level_t::Debug, xxx_logpos, "debug");
		xxx::log::tracer_t	v(logger, xxx::log::level_t::Verbose, xxx_logpos, "verbose");

		using namespace std::string_literals;
		f.set_result("1");
		e.set_result("2"s);
		w.set_result(3);
	}

	xxx::log::add_logger("tag", xxx::log::level_t::All, "test.log", "test", true);
	auto&	logger2	= xxx::log::logger("tag");
	logger2.oops(xxx_logpos, "oops");
	logger2.err(xxx_logpos, "err");
	logger2.warn(xxx_logpos, "warn");
	logger2.notice(xxx_logpos, "notice");
	logger2.info(xxx_logpos, "info");
	logger2.trace(xxx_logpos, "trace");
	logger2.debug(xxx_logpos, "debug");
	logger2.verbose(xxx_logpos, "verbose");

	{
		xxx::log::tracer_t	l(logger2, xxx_logpos, "arg1", 2);
		l.set_result(3);
	}
}

void
test_string()
{
	std::cout << "---[" << __func__ << "]---" << std::endl;

	{
		char const a[]{ "  \t  \r   \n    \r\n  \v   aaa    \t  \r   \n    \r\n  \v " };

		std::string rihgt_result{ "  \t  \r   \n    \r\n  \v   aaa" };
		std::string left_result{ "aaa    \t  \r   \n    \r\n  \v " };
		std::string result{ "aaa" };

		std::cout << (xxx::trim_left(a) == left_result) << std::endl;
		std::cout << (xxx::trim_right(a) == rihgt_result) << std::endl;
		std::cout << (xxx::trim(a) == result) << std::endl;
	}

	{
		try {
			xxx::lexical_cast<bool>("1");
		} catch (std::invalid_argument const&)
		{
			std::cout << "expected exception occurred" << std::endl;
		}
		try {
			xxx::lexical_cast<bool>("0");
		}
		catch (std::invalid_argument const&)
		{
			std::cout << "expected exception occurred" << std::endl;
		}

		std::cout << xxx::lexical_cast<bool>("true") << std::endl;
		std::cout << xxx::lexical_cast<bool>("false") << std::endl;

		std::cout << xxx::lexical_cast<int>("255") << std::endl;
		std::cout << xxx::lexical_cast<double>("1.234e2") << std::endl;
	}
}

int
main()
{
	test_version();
	test_exception_dump();
	test_exception_handling();
	test_finally();
	test_logger();
	test_string();
}
