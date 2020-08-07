///	@file
///	@brief		string utilities.
///	@pre		ISO/IEC 14882:2017
///	@author		Mura
///	@copyright	(C) 2018-, Mura. All rights reserved. (MIT License)

#ifndef xxx_STR_HXX_
#define xxx_STR_HXX_

#include <string>
#include <string_view>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace xxx {

///	@brief	Trims whitespaces of right side.
///	@param[in]		str		String to trim
///	@return		Trimed string
inline std::string
trim_right(std::string_view str)
{
	auto const	right_ws_itr{ std::find_if_not(std::crbegin(str), std::crend(str),
		[](auto ch) { return std::isspace(ch); }).base() };

	std::ostringstream	oss;
	std::copy(std::cbegin(str), right_ws_itr, std::ostreambuf_iterator<char>(oss));
	return oss.str();
}

///	@brief	Trims whitespaces of left side.
///	@param[in]		str		String to trim
///	@return		Trimed string
inline std::string
trim_left(std::string_view str)
{
	auto const	left_ws_itr{ std::find_if_not(std::cbegin(str), std::cend(str),
		[](auto ch) { return std::isspace(ch); }) };

	if (left_ws_itr == std::cend(str))
	{
		return std::string();
	}
	else
	{
		std::ostringstream	oss;
		std::copy(left_ws_itr, std::cend(str), std::ostreambuf_iterator<char>(oss));
		return oss.str();
	}
}

///	@brief	Trims whitespaces.
///	@param[in]		str		String to trim
///	@return		Trimed string
inline std::string
trim(std::string_view str)
{
	auto const	left_ws_itr{ std::find_if_not(std::cbegin(str), std::cend(str),
		[](auto ch) { return std::isspace(ch); }) };

	if (left_ws_itr == std::cend(str))
	{
		return std::string();
	}

	auto const	right_ws_itr{ std::find_if_not(std::crbegin(str), std::crend(str),
		[](auto ch) { return std::isspace(ch); }).base() };

	std::ostringstream	oss;
	std::copy(left_ws_itr, right_ws_itr, std::ostreambuf_iterator<char>(oss));
	return oss.str();
}

///	@brief	Casts from string to the type @p T.
///	@tparam			T		Type of returning value
///	@param[in]		str		String to cast
///	@return		Casted value
template<typename T>
inline T
lexical_cast(std::string_view str)
{
	std::istringstream	iss{ str.data() };
	T	t{};
	if ( ! (iss >> t))
	{
		throw std::invalid_argument(__func__);
	}
	return t;
}

///	@overload
template<>
inline bool
lexical_cast(std::string_view str)
{
	std::istringstream	iss{ str.data() };
	bool	t{};
	if ( ! (iss >> std::boolalpha >> t))
	{
		throw std::invalid_argument(__func__);
	}
	return t;
}

///	@overload
template<>
inline std::string
lexical_cast(std::string_view str)
{
	return str.data();
}

///	@overload
template<>
inline std::string_view
lexical_cast(std::string_view str)
{
	return str;
}

}	// namespace xxx

#endif	// xxx_STR_HXX_
