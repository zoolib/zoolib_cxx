/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZUnicodeString__
#define __ZUnicodeString__ 1
#include "zconfig.h"

#include "zoolib/ZTypes.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN

/// A basic_string specialization that holds a sequence of UTF32 code units.
typedef std::basic_string<UTF32> string32;

/// A basic_string specialization that holds a sequence of UTF16 code units.
typedef std::basic_string<UTF16> string16;

/// A basic_string specialization that holds a sequence of UTF8 code units.
/// It is actually the same type as std::string.
typedef std::basic_string<UTF8> string8;


NAMESPACE_ZOOLIB_END

#endif // __ZUnicodeString__
