// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Chan_UTF_Operators_h__
#define __ZooLib_Util_Chan_UTF_Operators_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_UTF.h"

#include <typeinfo>

// =================================================================================================
#pragma mark - Util_Chan_UTF_Operators

namespace ZooLib {
namespace Util_Chan_UTF_Operators {

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const string32& iString);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, const UTF32* iString);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, UTF32* iString);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, const string16& iString);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, const UTF16* iString);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, UTF16* iString);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, const string8& iString);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, const UTF8* iString);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, UTF8* iString);

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const ChanR_UTF& r);

const ChanW_UTF& operator<<(const ChanW_UTF& ww, bool iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, char iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, unsigned char iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, signed char iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, __wchar_t iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, short iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, unsigned short iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, int iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, unsigned int iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, long iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, unsigned long iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, __int64 iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, __uint64 iVal);

#if ZCONFIG_CPP >= 2011
	const ChanW_UTF& operator<<(const ChanW_UTF& ww, char16_t iVal);
	const ChanW_UTF& operator<<(const ChanW_UTF& ww, char32_t iVal);
#endif

const ChanW_UTF& operator<<(const ChanW_UTF& ww, float iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, double iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, long double iVal);

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const void* iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& ww, void* iVal);

template <class P>
const ChanW_UTF& operator<<(const ChanW_UTF& ww, const P* iVal)
	{ return ww << "(const " << typeid(P).name() << "*)" << reinterpret_cast<const void*>(iVal); }

template <class P>
const ChanW_UTF& operator<<(const ChanW_UTF& ww, P* iVal)
	{ return ww << "(" << typeid(P).name() << "*)" << reinterpret_cast<void*>(iVal); }

} // namespace Util_Chan_UTF_Operators

#if not defined(ZMACRO_Util_Chan_UTF_Operators_Suppress) || not ZMACRO_Util_Chan_UTF_Operators_Suppress
	using Util_Chan_UTF_Operators::operator<<;
#endif

//template <class P>
//std::string sObjPtrStr(const P* iVal)
//	{
//	return std::string("(const ") + typeid(P).name() + "*)"
//		+ sStringf("%p", reinterpret_cast<const void*>(iVal));
//	}
//
//template <class P>
//std::string sObjPtrStr(P* iVal)
//	{
//	return std::string("(") + typeid(P).name() + "*)"
//		+ sStringf("%p", reinterpret_cast<void*>(iVal));
//	}

} // namespace ZooLib

#endif // __ZooLib_Util_Chan_UTF_Operators_h__
