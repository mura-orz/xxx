///	@file
///	@brief		UNICODE.
///	@author		Mura.
///	@copyright	(C) 2020, Mura.

#ifndef xxx_UC_HXX_
#define xxx_UC_HXX_

#include <string>
#include <string_view>
#include <sstream>
#include <tuple>
#include <array>
#include <vector>
#include <algorithm>
#include <stdexcept>

#if defined(xxx_win32) || defined(xxx_non_utf8_mbs)
// This implementation does not use Windows.h header of VC2019
// because the header cannot be complied in strict C++ stardard mode.

#include <cstdlib>

#endif

#if ! defined(__cpp_char8_t)
#error "This implementation requires char8_t type."
#endif
#if !defined(__cpp_lib_char8_t)
#error "This implementation requires char8_t type."
#endif

//#if !defined(__STDC_UTF_16__) || !defined(__STDC_UTF_32__)
//#error "This header requires UTF-16 and UTF-32"
//#endif

namespace xxx {
namespace uc {

inline constexpr bool is_valid(char32_t ch) noexcept { return ch <= 0x0010'FFFF; }

inline constexpr bool is_u8bom(std::u8string_view const &str) noexcept
{
	using namespace std::string_view_literals;

	auto const bom = u8"\xEF\xBB\xBF"sv;
	return str == bom;
}
inline constexpr bool is_u16be(char16_t ch) noexcept { return ch == 0xFEFF; }
inline constexpr bool is_u16le(char16_t ch) noexcept { return ch == 0xFFFE; }
inline constexpr bool is_u16bom(char16_t ch) noexcept { return is_u16be(ch) || is_u16le(ch); }
inline constexpr bool is_u32be(char32_t ch) noexcept { return ch == 0xFEFF'0000; }
inline constexpr bool is_u32le(char32_t ch) noexcept { return ch == 0x0000'FEFF; }
inline constexpr bool is_u32bom(char32_t ch) noexcept { return is_u32be(ch) || is_u32le(ch); }

inline constexpr bool is_high_surrogate_pair(char16_t ch) noexcept { return is_valid(ch) && 0xFC00 <= ch && ch < 0xDBFF; }
inline constexpr bool is_low_surrogate_pair(char16_t ch) noexcept { return is_valid(ch) && 0xDC00 <= ch && ch < 0xDFFF; }
inline constexpr bool is_surrogate_pair(char16_t ch) noexcept { return is_high_surrogate_pair(ch) || is_low_surrogate_pair(ch); }
inline constexpr bool is_surrogate_pair(char32_t ch) noexcept { return is_valid(ch) && 0x0000'FFFF < ch; }

enum class u8_t
{
	E,
	L1,
	L2,
	L3,
	L4,
	T
};
inline constexpr u8_t
check_type(char8_t ch) noexcept
{
	if      ((ch & 0b1000'0000) == 0b0000'0000)	{ return u8_t::L1;	}
	else if ((ch & 0b1100'0000) == 0b1000'0000)	{ return u8_t::T;	}
	else if ((ch & 0b1110'0000) == 0b1100'0000)	{ return u8_t::L2;	}
	else if ((ch & 0b1111'0000) == 0b1110'0000)	{ return u8_t::L3;	}
	else if ((ch & 0b1111'1000) == 0b1111'0000)	{ return u8_t::L4;	}
	else										{ return u8_t::E;	}
}
inline constexpr char8_t
get_bits(char8_t ch) noexcept
{
	switch (check_type(ch))
	{
	case u8_t::L1:		return ch;
	case u8_t::T:		return ch & 0b0011'1111;
	case u8_t::L2:		return ch & 0b0001'1111;
	case u8_t::L3:		return ch & 0b0000'1111;
	case u8_t::L4:		return ch & 0b0000'0111;
	case u8_t::E:		[[fallthrough]];
	default:			return 0;
	}
}

inline constexpr std::tuple<std::size_t, char32_t>
to_u32(std::u8string_view const &str) noexcept
{
	if (str.empty())
	{
		return {0u, U'\0'};
	}
	char32_t ch;
	switch (check_type(str[0]))
	{
	case u8_t::L1:
		return {1u, str[0]};
	case u8_t::L2:
		if (str.length() < 2u || check_type(str[1]) != u8_t::T)
		{
			return {0u, U'\0'};
		}
		ch = get_bits(str[0]) * (2 << 6) + get_bits(str[1]);
		if (ch < 0x80 || 0x0800 <= ch)
		{
			return {0u, U'\0'};
		}
		return {2u, ch};
	case u8_t::L3:
		if (str.length() < 3u || check_type(str[1]) != u8_t::T || check_type(str[2]) != u8_t::T)
		{
			return {0u, U'\0'};
		}
		ch = get_bits(str[0]) * (2 << 6 * 2) + get_bits(str[1]) * (2 << 6) + get_bits(str[2]);
		if (ch < 0x0800 || 0x0001'0000 <= ch)
		{
			return {0u, U'\0'};
		}
		return {3u, ch};
	case u8_t::L4:
		if (str.length() < 4u || check_type(str[1]) != u8_t::T || check_type(str[2]) != u8_t::T || check_type(str[3]) != u8_t::T)
		{
			return {0u, U'\0'};
		}
		ch = get_bits(str[0]) * (2 << 6 * 3) + get_bits(str[1]) * (2 << 6 * 2) + get_bits(str[2]) * (2 << 6) + get_bits(str[3]);
		if (ch < 0x0001'0000 || 0x0011'0000 <= ch)
		{
			return {0u, U'\0'};
		}
		return {4u, ch};

	case u8_t::E:		[[fallthrough]];
	case u8_t::T:		[[fallthrough]];
	default:			break;
	}
	return {0u, U'\0'};
}

inline constexpr std::tuple<std::size_t, char32_t>
to_u32(std::u16string_view const &str) noexcept
{
	if (str.empty())
	{
		return {0u, U'\0'};
	}
	else if (is_high_surrogate_pair(str[0]))
	{
		if (str.size() < 2u || !is_low_surrogate_pair(str[1]))
		{
			return {0u, U'\0'};
		}
		return {2u, 0x0010'0000 + ((str[0] - 0xD800) * 0x0400) + (str[1] - 0xDC00)};
	}
	if (is_low_surrogate_pair(str[0]))
	{
		return {0u, U'\0'};
	}
	else
	{
		return {1u, str[0]};
	}
}

inline constexpr std::tuple<std::size_t, std::array<char16_t, 3u>>
to_u16(char32_t ch) noexcept
{
	if (!is_valid(ch))
	{
		return {0u, {u'\0', u'\0', u'\0'}};
	}
	else if (is_surrogate_pair(ch))
	{
		auto const high{static_cast<char16_t>((ch - 0x0010'0000) / 0x400 + 0xD800)};
		auto const low{static_cast<char16_t>((ch - 0x0010'0000) % 0x400 + 0xDC00)};
		return {2u, {high, low, u'\0'}};
	}
	else
	{
		return {1u, {static_cast<char16_t>(ch), u'\0', u'\0'}};
	}
}

inline constexpr std::tuple<std::size_t, std::array<char8_t, 5u>>
to_u8(char32_t ch) noexcept
{
	if (ch < 0x80)
	{
		return {1u, {static_cast<char8_t>(ch), u8'\0', u8'\0', u8'\0', u8'\0'}};
	}
	else if (0x80 <= ch && ch < 0x0800)
	{
		return {2u, {static_cast<char8_t>(((ch / (2 << 6 * 1)) & 0x0b0001'1111) | 0b1100'0000), static_cast<char8_t>((ch & 0x0b0011'1111) | 0b1000'0000), u8'\0', u8'\0', u8'\0'}};
	}
	else if (0x0800 <= ch && ch < 0x0001'0000)
	{
		return {3u, {static_cast<char8_t>(((ch / (2 << 6 * 2)) & 0x0b0000'1111) | 0b1110'0000), static_cast<char8_t>(((ch / (2 << 6 * 1)) & 0x0b0011'1111) | 0b1000'0000), static_cast<char8_t>((ch & 0x0b0011'1111) | 0b1000'0000), u8'\0', u8'\0'}};
	}
	else if (0x0001'0000 <= ch && ch < 0x0011'0000)
	{
		return {4u, {static_cast<char8_t>(((ch / (2 << 6 * 3)) & 0x0b0000'0111) | 0b1111'0000), static_cast<char8_t>(((ch / (2 << 6 * 2)) & 0x0b0011'1111) | 0b1000'0000), static_cast<char8_t>(((ch / (2 << 6 * 1)) & 0x0b0011'1111) | 0b1000'0000), static_cast<char8_t>((ch & 0x0b0011'1111) | 0b1000'0000), u8'\0'}};
	}
	else
	{
		return {0u, {u8'\0', u8'\0', u8'\0', u8'\0', u8'\0'}};
	}
}

// ===========================================================================

inline std::u32string
to_u32string(std::u8string_view const &str)
{
	std::vector<char32_t> buffer;
	buffer.reserve(str.length() + 1);
	for (std::size_t n = 0u; n < str.length();)
	{
		auto [length, ch] = to_u32(str.substr(n));
		if (length == 0u)
		{
			throw std::invalid_argument(__func__);
		}
		buffer.push_back(ch);
		n += length;
	}
	buffer.push_back(U'\0');
	return &buffer[0];
}
inline std::u32string
to_u32string(std::u16string_view const &str)
{
	std::vector<char32_t> buffer;
	buffer.reserve(str.length() + 1);
	for (std::size_t n = 0u; n < str.length();)
	{
		auto [length, ch] = to_u32(str.substr(n));
		if (length == 0u)
		{
			throw std::invalid_argument(__func__);
		}
		buffer.push_back(ch);
		n += length;
	}
	buffer.push_back(U'\0');
	return &buffer[0];
}
inline std::u32string
to_u32string(std::u32string_view const &str)
{
	return std::u32string{str};
}

inline std::u16string
to_u16string(std::u32string_view const &str)
{
	std::vector<char16_t> buffer;
	buffer.reserve(str.length() * 2 + 1);
	for (auto const ch : str)
	{
		auto [length, s] = to_u16(ch);
		if (length == 0u)
		{
			throw std::invalid_argument(__func__);
		}
		for (auto itr{std::cbegin(s)}, end{std::cend(s)}; itr != end && *itr != u'\0'; ++itr)
		{
			buffer.push_back(*itr);
		}
	}
	buffer.push_back(U'\0');
	return &buffer[0];
}
inline std::u16string
to_u16string(std::u8string_view const &str)
{
	return to_u16string(to_u32string(str));
}
inline std::u16string
to_u16string(std::u16string_view const &str)
{
	return std::u16string{str};
}

inline std::u8string
to_u8string(std::u32string_view const &str)
{
	std::vector<char8_t> buffer;
	buffer.reserve(str.length() * 4 + 1);
	for (auto const &ch : str)
	{
		auto [length, s] = to_u8(ch);
		if (length == 0u)
		{
			throw std::invalid_argument(__func__);
		}
		for (auto itr{std::cbegin(s)}, end{std::cend(s)}; itr != end && *itr != u8'\0'; ++itr)
		{
			buffer.push_back(*itr);
		}
	}
	buffer.push_back(u8'\0');
	return &buffer[0];
}

inline std::u8string
to_u8string(std::u8string_view const &str)
{
	return std::u8string{str};
}
inline std::u8string
to_u8string(std::u16string_view const &str)
{
	auto const s32{to_u32string(str)};
	return to_u8string(s32);
}

inline std::u32string
to_u32string(std::wstring_view const &str)
{
	if constexpr (sizeof(wchar_t) == 2u)
	{
		auto const s{to_u32string(reinterpret_cast<char16_t const *>(str.data()))};
		return s.empty() ? std::u32string() : s;
	}
	else if constexpr (sizeof(wchar_t) == 4u)
	{
		auto const s{to_u32string(reinterpret_cast<char32_t const *>(str.data()))};
		return s.empty() ? std::u32string() : s;
	}
	else
	{
		throw std::runtime_error(__func__);
	}
}
inline std::u16string
to_u16string(std::wstring_view const &str)
{
	if constexpr (sizeof(wchar_t) == 2u)
	{
		auto const s{to_u16string(reinterpret_cast<char16_t const *>(str.data()))};
		return s.empty() ? std::u16string() : s;
	}
	else if constexpr (sizeof(wchar_t) == 4u)
	{
		auto const s{to_u16string(reinterpret_cast<char32_t const *>(str.data()))};
		return s.empty() ? std::u16string() : s;
	}
	else
	{
		throw std::runtime_error(__func__);
	}
}
inline std::u8string
to_u8string(std::wstring_view const &str)
{
	if constexpr (sizeof(wchar_t) == 2u)
	{
		auto const s{to_u8string(std::u16string_view{reinterpret_cast<char16_t const *>(str.data()), str.length()})};
		return s.empty() ? std::u8string() : s;
	}
	else if constexpr (sizeof(wchar_t) == 4u)
	{
		auto const s{to_u8string(std::u32string_view{reinterpret_cast<char32_t const *>(str.data()), str.length()})};
		return s.empty() ? std::u8string() : s;
	}
	else
	{
		throw std::runtime_error(__func__);
	}
}

inline std::wstring
to_wstring(std::wstring_view const &str)
{
	return std::wstring{str};
}
inline std::wstring
to_wstring(std::u8string_view const &str)
{
	if constexpr (sizeof(wchar_t) == 2u)
	{
		auto const s{to_u16string(str)};
		return s.empty() ? std::wstring() : reinterpret_cast<wchar_t const *>(&s[0]);
	}
	else if constexpr (sizeof(wchar_t) == 4u)
	{
		auto const s{to_u32string(str)};
		return s.empty() ? std::wstring() : reinterpret_cast<wchar_t const *>(&s[0]);
	}
	else
	{
		throw std::runtime_error(__func__);
	}
}
inline std::wstring
to_wstring(std::u16string_view const &str)
{
	if constexpr (sizeof(wchar_t) == 2u)
	{
		return std::wstring{reinterpret_cast<wchar_t const *>(str.data()), str.length()};
	}
	else if constexpr (sizeof(wchar_t) == 4u)
	{
		auto const s{to_u32string(str)};
		return s.empty() ? std::wstring() : reinterpret_cast<wchar_t const *>(&s[0]);
	}
	else
	{
		throw std::runtime_error(__func__);
	}
}

inline std::wstring
to_wstring(std::u32string_view const &str)
{
	if (str.empty())
	{
		return std::wstring();
	}

	if constexpr (sizeof(wchar_t) == 2u)
	{
		auto const s{to_u16string(str)};
		return s.empty() ? std::wstring() : reinterpret_cast<wchar_t const *>(&s[0]);
	}
	else if constexpr (sizeof(wchar_t) == 4u)
	{
		return std::wstring{reinterpret_cast<wchar_t const *>(str.data()), str.length()};
	}
	else
	{
		throw std::runtime_error(__func__);
	}
}

#if defined(xxx_win32)
inline std::string
to_string(std::wstring_view const& str)
{
	if (str.empty())	return std::string();

	auto const failed	= static_cast<std::size_t>(-1);
	errno			  = 0u;
	auto		size	= std::wcstombs(nullptr, str.data(), str.size());
	if (size == failed && errno != 0u)
	{
		throw std::invalid_argument(__func__);
	}
	std::vector<char>	buffer(size + 1);
	if (size = std::wcstombs(&buffer[0], str.data(), str.size()); size == failed)
	{
		throw std::invalid_argument(__func__);
	}
	return &buffer[0];
}
inline std::string
to_string(std::u32string_view const& str)
{
	return to_string(to_wstring(str));
}
inline std::string
to_string(std::u8string_view const& str)
{
	return to_string(to_wstring(str));
}
inline std::string
to_string(std::u16string_view const& str)
{
	return to_string(to_wstring(str));
}
#else
inline std::string
to_string(std::u8string_view const& str)
{
	std::ostringstream oss;
	std::transform(std::cbegin(str), std::cend(str), std::ostreambuf_iterator<char>(oss),
		[](auto ch) { return static_cast<char>(ch); });
	return oss.str();
}
inline std::string
to_string(std::u32string_view const& str)
{
	return to_string(to_u8string(str));
}
inline std::string
to_string(std::u16string_view const& str)
{
	return to_string(to_u8string(str));
}
inline std::string
to_string(std::wstring_view const& str)
{
	return to_string(to_u8string(str));
}
#endif

} // namespace uc
} // namespace xxx

#endif //xxx_UC_HXX_
