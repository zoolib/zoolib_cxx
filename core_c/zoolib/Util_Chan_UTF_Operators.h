/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __zoolib_Util_Chan_UTF_Operators_h__
#define __zoolib_Util_Chan_UTF_Operators_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/Stringf.h"

#include <typeinfo>

// =================================================================================================
// MARK: - Util_Chan_UTF_Operators

namespace ZooLib {
namespace Util_Chan_UTF_Operators {

const ChanW_UTF& operator<<(const ChanW_UTF& w, const string32& iString);
const ChanW_UTF& operator<<(const ChanW_UTF& w, const UTF32* iString);
const ChanW_UTF& operator<<(const ChanW_UTF& w, UTF32* iString);
const ChanW_UTF& operator<<(const ChanW_UTF& w, const string16& iString);
const ChanW_UTF& operator<<(const ChanW_UTF& w, const UTF16* iString);
const ChanW_UTF& operator<<(const ChanW_UTF& w, UTF16* iString);
const ChanW_UTF& operator<<(const ChanW_UTF& w, const string8& iString);
const ChanW_UTF& operator<<(const ChanW_UTF& w, const UTF8* iString);
const ChanW_UTF& operator<<(const ChanW_UTF& w, UTF8* iString);

const ChanW_UTF& operator<<(const ChanW_UTF& w, const ChanR_UTF& r);

const ChanW_UTF& operator<<(const ChanW_UTF& w, bool iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, char iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, unsigned char iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, signed char iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, __wchar_t iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, short iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, unsigned short iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, int iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, unsigned int iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, long iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, unsigned long iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, __int64 iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, __uint64 iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, float iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, double iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, long double iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, const void* iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& w, void* iVal);

template <class P>
const ChanW_UTF& operator<<(const ChanW_UTF& w, const P* iVal)
	{ return w << "(const " << typeid(P).name() << "*)" << reinterpret_cast<const void*>(iVal); }

template <class P>
const ChanW_UTF& operator<<(const ChanW_UTF& w, P* iVal)
	{ return w << "(" << typeid(P).name() << "*)" << reinterpret_cast<void*>(iVal); }

} // namespace Util_Chan_UTF_Operators

#if not defined(ZMACRO_Util_Chan_UTF_Operators_Suppress) || not ZMACRO_Util_Chan_UTF_Operators_Suppress
	using Util_Chan_UTF_Operators::operator<<;
#endif

template <class P>
std::string sObjPtrStr(const P* iVal)
	{
	return std::string("(const ") + typeid(P).name() + "*)"
		+ sStringf("%p", reinterpret_cast<const void*>(iVal));
	}

template <class P>
std::string sObjPtrStr(P* iVal)
	{
	return std::string("(") + typeid(P).name() + "*)"
		+ sStringf("%p", reinterpret_cast<void*>(iVal));
	}

} // namespace ZooLib

#endif // __zoolib_Util_Chan_UTF_Operators_h__
