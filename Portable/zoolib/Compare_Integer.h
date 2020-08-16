// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compare_Integer_h__
#define __ZooLib_Compare_Integer_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"
#include "zoolib/Compat_MSVCStaticLib.h"

ZMACRO_MSVCStaticLib_Reference(Compare_Integer)

namespace ZooLib {

template <>
inline int sCompare_T(const bool& iL, const bool& iR)
	{ return int(iL) - int(iR); }

// -----

template <>
inline int sCompare_T(const char& iL, const char& iR)
	{ return iL - iR; }

template <>
inline int sCompare_T(const unsigned char& iL, const unsigned char& iR)
	{ return int(iL) - int(iR); }

template <>
inline int sCompare_T(const signed char& iL, const signed char& iR)
	{ return iL - iR; }

// -----

template <>
inline int sCompare_T(const __wchar_t& iL, const __wchar_t& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

// -----

template <>
inline int sCompare_T(const short& iL, const short& iR)
	{ return iL - iR; }

template <>
inline int sCompare_T(const unsigned short& iL, const unsigned short& iR)
	{ return int(iL) - int(iR); }

// -----

template <>
inline int sCompare_T(const int& iL, const int& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

template <>
inline int sCompare_T(const unsigned int& iL, const unsigned int& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

// -----

template <>
inline int sCompare_T(const long& iL, const long& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

template <>
inline int sCompare_T(const unsigned long& iL, const unsigned long& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

// -----

template <>
inline int sCompare_T(const __int64& iL, const __int64& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

template <>
inline int sCompare_T(const __uint64& iL, const __uint64& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

// -----

#if ZCONFIG_CPP >= 2011

template <>
inline int sCompare_T(const char16_t& iL, const char16_t& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

template <>
inline int sCompare_T(const char32_t& iL, const char32_t& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

#endif // ZCONFIG_CPP >= 2011

} // namespace ZooLib

#endif // __ZooLib_Compare_Integer_h__
