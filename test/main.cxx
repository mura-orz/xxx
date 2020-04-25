
#include <xxx/xxx.hxx>
#include <xxx/exceptions.hxx>
#include <xxx/logger.hxx>

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
test_logger()
{
	std::cout << "---[" << __func__ << "]---" << std::endl;

	auto& logger = xxx::log::logger("");

	logger.oops(xxx_log, "oops");
	logger.err(xxx_log, "err");
	logger.warn(xxx_log, "warn");
	logger.notice(xxx_log, "notice");
	logger.info(xxx_log, "info");
	logger.trace(xxx_log, "trace");
	logger.debug(xxx_log, "debug");
	logger.verbose(xxx_log, "verbose");
	logger.log(xxx::log::level_t::Fatal, xxx_log, "oops");
	logger.log(xxx::log::level_t::Error, xxx_log, "err");
	logger.log(xxx::log::level_t::Warn, xxx_log, "warn");
	logger.log(xxx::log::level_t::Notice, xxx_log, "notice");
	logger.log(xxx::log::level_t::Info, xxx_log, "info");
	logger.log(xxx::log::level_t::Trace, xxx_log, "trace");
	logger.log(xxx::log::level_t::Debug, xxx_log, "debug");
	logger.log(xxx::log::level_t::Verbose, xxx_log, "verbose");

	{
		xxx::log::tracer_t	l(logger, xxx_log);
	}
	{
		xxx::log::tracer_t	l(logger, xxx_log, "arg");
	}
	{
		xxx::log::tracer_t	l(logger, xxx_log, "arg1", 2);
	}
	{
		xxx::log::tracer_t	f(logger, xxx::log::level_t::Fatal, xxx_log, "oops");
		xxx::log::tracer_t	e(logger, xxx::log::level_t::Error, xxx_log, "err");
		xxx::log::tracer_t	w(logger, xxx::log::level_t::Warn, xxx_log, "warn");
		xxx::log::tracer_t	n(logger, xxx::log::level_t::Notice, xxx_log, "notice");
		xxx::log::tracer_t	i(logger, xxx::log::level_t::Info, xxx_log, "info");
		xxx::log::tracer_t	t(logger, xxx::log::level_t::Trace, xxx_log, "trace");
		xxx::log::tracer_t	d(logger, xxx::log::level_t::Debug, xxx_log, "debug");
		xxx::log::tracer_t	v(logger, xxx::log::level_t::Verbose, xxx_log, "verbose");

		using namespace std::string_literals;
		f.set_result("1");
		e.set_result("2"s);
		w.set_result(3);
	}

	xxx::log::add_logger("tag", xxx::log::level_t::All, "test.log", "test", true);
	auto&	logger2	= xxx::log::logger("tag");
	logger2.oops(xxx_log, "oops");
	logger2.err(xxx_log, "err");
	logger2.warn(xxx_log, "warn");
	logger2.notice(xxx_log, "notice");
	logger2.info(xxx_log, "info");
	logger2.trace(xxx_log, "trace");
	logger2.debug(xxx_log, "debug");
	logger2.verbose(xxx_log, "verbose");

	{
		xxx::log::tracer_t	l(logger2, xxx_log, "arg1", 2);
		l.set_result(3);
	}
}

int
main()
{
	test_version();
	test_exception_dump();
	test_exception_handling();
	test_logger();
}
