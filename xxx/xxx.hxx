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
#define		xxx_version			(static_cast<std::uint32_t>(0x00010000))

///	@name	xxx
///	@brief	root of xxx common library.
namespace xxx {

///	@brief	Gets built version of xxx
///	@return		version of built version
std::uint32_t	get_version() noexcept;

///	@brief	Gets major version of the @p version.
///		If backward compatibility is broken, this version is imcremented.
///	@param[in]	version		The library versions, xxx_version macro or result of xxx_version()
///	@return		Major version of the @p version.
inline std::uint32_t	get_major_version(std::uint32_t version) noexcept    { return static_cast<std::uint32_t>((version >> 24) & 0x000000FF);  }
///	@brief	Gets minor version of the @p version.
///		If new feature with backward compatibility is added, this version is imcremented.
///		If the major version increases, this version is reset to zero.
///	@param[in]	version		The library versions, xxx_version macro or result of xxx_version()
///	@return		Minor version of the @p version.
inline std::uint32_t	get_minor_version(std::uint32_t version) noexcept    { return static_cast<std::uint32_t>((version >> 16) & 0x000000FF);  }
///	@brief	Gets revision of the @p version.
///		If bug-fixes or trival improvements are added, this version is imcremented.
///		If the minor version increases or resets, this version is reset to zero, too.
///	@param[in]	version		The library versions, xxx_version macro or result of xxx_version()
///	@return		Revision of the @p version.
inline std::uint32_t	get_revision(std::uint32_t version) noexcept         { return static_cast<std::uint32_t>((version >>  8) & 0x000000FF);  }
///	@brief	Gets extra version of the @p version.
///		This version is reserved for especial usage.
///	@param[in]	version		The library versions, xxx_version macro or result of xxx_version()
///	@return		Extra version of the @p version.
inline std::uint32_t	get_extra_version(std::uint32_t version) noexcept    { return static_cast<std::uint32_t>((version >>  0) & 0x000000FF);  }
  
}	// namespace xxx

#endif	// xxx_HXX_
