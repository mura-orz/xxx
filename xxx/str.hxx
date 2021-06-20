///	@file
///	@brief		string utilities.
///	@pre		ISO/IEC 14882:2017
///	@author		Mura
///	@copyright	(C) 2018-, Mura. All rights reserved. (MIT License)

#ifndef xxx_STR_HXX_
#define xxx_STR_HXX_

#include <string>
#include <string_view>
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace xxx {

///	@brief	Trims whitespaces of right side.
/// @notice	It depends on global locale.
///	@param[in]		str		String to trim
///	@return		Trimed string
inline std::string
trim_right(std::string_view str)
{
	using std::cbegin, std::crbegin, std::crend;

	if (auto const	right_ws_itr{ std::find_if_not(crbegin(str), crend(str),
		[](auto ch) { return std::isspace(ch); }).base() };
		right_ws_itr != cend(str))
	{
		return std::string(cbegin(str), right_ws_itr);
	}
	return std::string();
}

///	@brief	Trims whitespaces of left side.
/// @notice	It depends on global locale.
///	@param[in]		str		String to trim
///	@return		Trimed string
inline std::string
trim_left(std::string_view str)
{
	using std::cbegin, std::cend;

	if (auto const	left_ws_itr{ std::find_if_not(cbegin(str), cend(str),
		[](auto ch) { return std::isspace(ch); }) };
		left_ws_itr != cend(str))
	{
		return std::string(left_ws_itr, cend(str));
	}
	return std::string();
}

///	@brief	Trims whitespaces.
/// @notice	It depends on global locale.
///	@param[in]		str		String to trim
///	@return		Trimed string
inline std::string
trim(std::string_view str)
{
	using std::cbegin, std::cend, std::crbegin, std::crend;

	auto const	left_ws_itr{ std::find_if_not(cbegin(str), cend(str),
		[](auto ch) { return std::isspace(ch); }) };

	if (left_ws_itr == cend(str))
	{
		return std::string();
	}

	auto const	right_ws_itr{ std::find_if_not(crbegin(str), crend(str),
		[](auto ch) { return std::isspace(ch); }).base() };

	return std::string(left_ws_itr, right_ws_itr);
}

///	@brief	Casts from string to the type @p T.
///	@tparam			T		Type of returning value
///	@param[in]		str		String to cast
///	@return		Casted value
template<typename T>
inline T
lexical_cast(std::string const& str)
{
	std::istringstream	iss{ str };
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
lexical_cast(std::string const& str)
{
	std::istringstream	iss{ str };
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
lexical_cast(std::string const& str)
{
	return str;
}

}	// namespace xxx

#endif	// xxx_STR_HXX_
