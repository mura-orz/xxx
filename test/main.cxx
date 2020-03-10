
#include <xxx/xxx.hxx>
#include <xxx/xexceptions.hxx>

#include <iostream>

int
main()
{
	std::cout << (xxx_version == xxx::get_version()) << std::endl;

	using namespace	xxx::exception_iostream;
		
	std::exception	e;
	std::cout	<< e << std::endl;

	try
	{
		try
		{
			try
			{
				std::string().at(1);
			}
			catch(std::exception const& e)
			{
				throw_with_nested(e);
			}
		}
		catch(std::exception const& e)
		{
			throw_with_nested(e);
		}
	}
	catch(std::exception const& ee)
	{
		std::cout	<< ee << std::endl;
	}
	return 0;
}