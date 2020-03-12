///	@file
///	@brief		xxx common library.
///	@details	Utilities for exceptions.
///	@pre		ISO/IEC 14882:2017
///	@author		Mura
///	@copyright	(C) 2018-, Mura. All rights reserved. (MIT License)

#ifndef xxx_XEXCEPTIONS_HXX_
#define xxx_XEXCEPTIONS_HXX_

#include <exception>

#include <iostream>
#include <string>
#include <typeinfo>

namespace xxx {

///	@name	exception handling
///	@{
	
///	@brief	Ignores any exception.
///	@tparam		P	function object of procedure
///	@param[in]		procedure	Procedure
template<typename P>
inline void
ignore_exceptions(P const& procedure) noexcept
{
	try
	{
		procedure();
	}
	catch(...)
	{}
}

///	@copydoc	ignore_exceptions()
///	@tparam		H	function object of exception handler
///	@param[in]		handler		Exception handler,
///								which requires only one std::exception_ptr parameter.
template<typename P, typename H>
inline void
ignore_exceptions(P const& procedure, H const& handler) noexcept
{
	try
	{
		procedure();
	}
	catch(...)
	{
		try
		{
			handler(std::current_exception());
		}
		catch(...)
		{}
	}
}

///	@brief	Suppresses any exception.
///	@tparam		P	function object of procedure
///	@param[in]		procedure	Procedure
///	@return		If an exception occurred, it returns it as std::exception_ptr;
///				otherwise, it returns nullptr as std::exception_ptr.
template<typename P>
inline std::exception_ptr
suppress_exceptions(P const& procedure) noexcept
{
	try
	{
		procedure();
		return nullptr;
	}
	catch(...)
	{
		return std::current_exception();
	}
}

///	@}

///	@name	nested exception
///	@{

namespace impl {

inline static void
dump_exception_(std::ostream& os, std::exception const& e, unsigned nested) noexcept
{
	ignore_exceptions([&os, &e, nested]()
	{
		// The nested_exception type would not be simple.
		auto const	p		= dynamic_cast<std::nested_exception const*>(&e);
		auto const	name	= p == nullptr ? typeid(e).name() : "std::nested_exception";

		if(0u == nested)
		{
			os		<< e.what() << " (" << name << ")" << std::endl;
		}
		os		<< " [" << nested << "] " << e.what() << " (" << name << ")" << std::endl;
	});

	try
	{
		std::rethrow_if_nested(e);
	}
	catch(std::exception const& ee)
	{
		dump_exception_(os, ee, nested + 1u);
	}
}

}	// namespace impl

///	@brief	Dumps exceptions.
///	@param[in,out]	os		Output stream
///	@param[in]		e		Exception to dump
inline void
dump_exception(std::ostream& os, std::exception const& e) noexcept
{
	impl::dump_exception_(os, e, 0u);
}

///	@name	dump exception to iostream.
namespace exception_iostream {

///	@brief	Dumps exceptions.
///	@param[in,out]	os		Output stream
///	@param[in]		e		Exception to dump
///	@return		It returns the @p os output stream.
///	@pre	Using the exception_iostream namespace is required.
///			e.g., @code
///				using namespace xxx::exception_iostream;
///			@endcode
inline std::ostream&
operator <<(std::ostream& os, std::exception const& e) noexcept
{
	dump_exception(os, e);
	return os;
}

}	// namespace exception_iostream
	
///	@}

}	// namespace xxx

#endif	// xxx_XEXCEPTIONS_HXX_
