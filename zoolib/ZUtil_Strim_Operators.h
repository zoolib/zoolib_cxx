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

#ifndef __ZUtil_Strim_Operators__
#define __ZUtil_Strim_Operators__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"

#include <typeinfo>

namespace ZooLib {
namespace ZUtil_Strim_Operators {

const ZStrimW& operator<<(const ZStrimW& s, const string32& iString);
const ZStrimW& operator<<(const ZStrimW& s, const UTF32* iString);
const ZStrimW& operator<<(const ZStrimW& s, UTF32* iString);
const ZStrimW& operator<<(const ZStrimW& s, const string16& iString);
const ZStrimW& operator<<(const ZStrimW& s, const UTF16* iString);
const ZStrimW& operator<<(const ZStrimW& s, UTF16* iString);
const ZStrimW& operator<<(const ZStrimW& s, const string8& iString);
const ZStrimW& operator<<(const ZStrimW& s, const UTF8* iString);
const ZStrimW& operator<<(const ZStrimW& s, UTF8* iString);

const ZStrimW& operator<<(const ZStrimW& s, const ZStrimR& r);

const ZStrimW& operator<<(const ZStrimW& s, bool iVal);
const ZStrimW& operator<<(const ZStrimW& s, char iVal);
const ZStrimW& operator<<(const ZStrimW& s, unsigned char iVal);
const ZStrimW& operator<<(const ZStrimW& s, signed char iVal);
const ZStrimW& operator<<(const ZStrimW& s, wchar_t iVal);
const ZStrimW& operator<<(const ZStrimW& s, short iVal);
const ZStrimW& operator<<(const ZStrimW& s, unsigned short iVal);
const ZStrimW& operator<<(const ZStrimW& s, int iVal);
const ZStrimW& operator<<(const ZStrimW& s, unsigned int iVal);
const ZStrimW& operator<<(const ZStrimW& s, long iVal);
const ZStrimW& operator<<(const ZStrimW& s, unsigned long iVal);
const ZStrimW& operator<<(const ZStrimW& s, int64 iVal);
const ZStrimW& operator<<(const ZStrimW& s, uint64 iVal);
const ZStrimW& operator<<(const ZStrimW& s, float iVal);
const ZStrimW& operator<<(const ZStrimW& s, double iVal);
const ZStrimW& operator<<(const ZStrimW& s, const void* iVal);
const ZStrimW& operator<<(const ZStrimW& s, void* iVal);

template <class P>
const ZStrimW& operator<<(const ZStrimW& s, const P* iVal)
	{ return s << "(" << typeid(P).name() << "*)" << static_cast<void*>(iVal); }

template <class P>
const ZStrimW& operator<<(const ZStrimW& s, P* iVal)
	{ return s << "(" << typeid(P).name() << "*)" << static_cast<void*>(iVal); }

} // namespace ZUtil_Strim_Operators

using ZUtil_Strim_Operators::operator<<;

} // namespace ZooLib

#endif // __ZUtil_Strim_Operators__
