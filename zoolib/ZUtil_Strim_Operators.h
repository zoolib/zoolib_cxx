/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZUtil_Strim_Operators_h__
#define __ZUtil_Strim_Operators_h__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"

#include <typeinfo>

// =================================================================================================
// MARK: - ZUtil_Strim_Operators

namespace ZooLib {
namespace ZUtil_Strim_Operators {

const ZStrimW& operator<<(const ZStrimW& w, const string32& iString);
const ZStrimW& operator<<(const ZStrimW& w, const UTF32* iString);
const ZStrimW& operator<<(const ZStrimW& w, UTF32* iString);
const ZStrimW& operator<<(const ZStrimW& w, const string16& iString);
const ZStrimW& operator<<(const ZStrimW& w, const UTF16* iString);
const ZStrimW& operator<<(const ZStrimW& w, UTF16* iString);
const ZStrimW& operator<<(const ZStrimW& w, const string8& iString);
const ZStrimW& operator<<(const ZStrimW& w, const UTF8* iString);
const ZStrimW& operator<<(const ZStrimW& w, UTF8* iString);

const ZStrimW& operator<<(const ZStrimW& w, const ZStrimR& r);

const ZStrimW& operator<<(const ZStrimW& w, bool iVal);
const ZStrimW& operator<<(const ZStrimW& w, char iVal);
const ZStrimW& operator<<(const ZStrimW& w, unsigned char iVal);
const ZStrimW& operator<<(const ZStrimW& w, signed char iVal);
const ZStrimW& operator<<(const ZStrimW& w, __wchar_t iVal);
const ZStrimW& operator<<(const ZStrimW& w, short iVal);
const ZStrimW& operator<<(const ZStrimW& w, unsigned short iVal);
const ZStrimW& operator<<(const ZStrimW& w, int iVal);
const ZStrimW& operator<<(const ZStrimW& w, unsigned int iVal);
const ZStrimW& operator<<(const ZStrimW& w, long iVal);
const ZStrimW& operator<<(const ZStrimW& w, unsigned long iVal);
const ZStrimW& operator<<(const ZStrimW& w, __int64 iVal);
const ZStrimW& operator<<(const ZStrimW& w, __uint64 iVal);
const ZStrimW& operator<<(const ZStrimW& w, float iVal);
const ZStrimW& operator<<(const ZStrimW& w, double iVal);
const ZStrimW& operator<<(const ZStrimW& w, long double iVal);
const ZStrimW& operator<<(const ZStrimW& w, const void* iVal);
const ZStrimW& operator<<(const ZStrimW& w, void* iVal);

template <class P>
const ZStrimW& operator<<(const ZStrimW& w, const P* iVal)
	{ return w << "(const " << typeid(P).name() << "*)" << reinterpret_cast<const void*>(iVal); }

template <class P>
const ZStrimW& operator<<(const ZStrimW& w, P* iVal)
	{ return w << "(" << typeid(P).name() << "*)" << reinterpret_cast<void*>(iVal); }

} // namespace ZUtil_Strim_Operators

#if not defined(ZMACRO_Util_Strim_Operators_Suppress) || not ZMACRO_Util_Strim_Operators_Suppress
	using ZUtil_Strim_Operators::operator<<;
#endif

} // namespace ZooLib

#endif // __ZUtil_Strim_Operators_h__
