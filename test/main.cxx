
#include <xxx/xxx.hxx>
#include <xxx/xexceptions.hxx>

#include <iostream>
#include <iomanip>

void
test_version()
{
	std::cout	<< "---[" << __func__ << "]---" << std::endl;

	std::cout	<< std::hex << xxx_version << std::endl << xxx::get_version() << std::endl;

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
		std::cout	<< e << std::endl;
		xxx::dump_exception(std::cout, e);
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
		std::cout	<< e << std::endl;
		xxx::dump_exception(std::cout, e);
	}
}

void
test_exception_handling()
{
	std::cout	<< "---[" << __func__ << "]---" << std::endl;
}

int
main()
{
	test_version();
	test_exception_dump();
	test_exception_handling();
}