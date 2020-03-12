///	@file
///	@brief		xxx common library.
///	@details	Utilities.
///	@pre		ISO/IEC 14882:2017
///	@author		Mura
///	@copyright	(C) 2018-, Mura. All rights reserved. (MIT License)

#ifndef xxx_HXX_
#define xxx_HXX_

#include <cstdint>

///	@brief	version of xxx.
///		e.g., 0x01020304 means version 1.2.3.4.
#define		xxx_version			0x00010000

///	@name	xxx
///	@brief	root of xxx common library.
namespace xxx {

///	@brief	Gets built version of xxx
///	@return		version of built version
std::uint32_t	get_version() noexcept;

inline std::uint32_t	get_major_version(std::uint32_t version) noexcept    { return static_cast<std::uint32_t>((version >> 24) & 0x000000FF);  }
inline std::uint32_t	get_minor_version(std::uint32_t version) noexcept    { return static_cast<std::uint32_t>((version >> 16) & 0x000000FF);  }
inline std::uint32_t	get_revision(std::uint32_t version) noexcept         { return static_cast<std::uint32_t>((version >>  8) & 0x000000FF);  }
inline std::uint32_t	get_extra_version(std::uint32_t version) noexcept    { return static_cast<std::uint32_t>((version >>  0) & 0x000000FF);  }
  
}	// namespace xxx

#endif	// xxx_HXX_
