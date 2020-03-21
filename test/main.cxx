
#include <xxx/xxx.hxx>
#include <xxx/xexceptions.hxx>

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
				std::string().at(1);	// Raise an exception focefully.
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
		xxx::ignore_exceptions([]()
		{
			std::cout << "an exception occurred" << std::endl;
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

int
main()
{
	test_version();
	test_exception_dump();
	test_exception_handling();
}