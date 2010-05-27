/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZUnicodeCU__
#define __ZUnicodeCU__ 1
#include "zconfig.h"

#include "zoolib/ZStdInt.h" // For uint16, uint32

namespace ZooLib {

// =================================================================================================

namespace ZUnicode {

// This lets us typedef UTF16 or UTF32 from wchar_t, with
// the other being a regular unsigned integer.

template <int s> struct Types_T {};

template <> struct Types_T<4>
	{
	typedef wchar_t utf32_t;
	typedef ZStdInt::uint16 utf16_t;
	};

template <> struct Types_T<2>
	{
	typedef ZStdInt::uint32 utf32_t;
	typedef wchar_t utf16_t;
	};

} // namespace ZUnicode

// Definitions of UTF32, UTF16 and UTF8
typedef ZUnicode::Types_T<sizeof(wchar_t)>::utf32_t UTF32;
typedef ZUnicode::Types_T<sizeof(wchar_t)>::utf16_t UTF16;
typedef char UTF8;

} // namespace ZooLib

#endif // __ZUnicodeCU__
