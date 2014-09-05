/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZUnicodeString8_h__
#define __ZUnicodeString8_h__ 1
#include "zconfig.h"

#include "zoolib/ZUnicodeCU8.h"

#include <string>

namespace ZooLib {

/** A basic_string specialization that holds a sequence of UTF8 code units.
It is almost certainly same type as std::string. */
typedef std::basic_string<UTF8> string8;

template <class Char_p>
bool sIsEmpty(const std::basic_string<Char_p>& iString)
	{ return iString.empty(); }

template <class Char_p>
bool sNotEmpty(const std::basic_string<Char_p>& iString)
	{ return not sIsEmpty(iString); }

} // namespace ZooLib

#endif // __ZUnicodeString8_h__
