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

#ifndef __ZooLib_Util_Chan_UTF_Operators_string_h__
#define __ZooLib_Util_Chan_UTF_Operators_string_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_UTF_string.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

// =================================================================================================
#pragma mark -
#pragma mark Util_Chan_UTF_Operators

namespace ZooLib {
namespace Util_Chan_UTF_Operators {

// These are not efficient -- syntactically and performance-wise they're very
// like the standard operator+

template <class Val_p>
string32 operator<<(const string32& iString, const Val_p& iVal)
	{
	string32 result = iString;
	ChanW_UTF_string<UTF32>(&result) << iVal;
	return result;
	}

template <class Val_p>
string16 operator<<(const string16& iString, const Val_p& iVal)
	{
	string16 result = iString;
	ChanW_UTF_string<UTF16>(&result) << iVal;
	return result;
	}

template <class Val_p>
string8 operator<<(const string8& iString, const Val_p& iVal)
	{
	string8 result = iString;
	ChanW_UTF_string<UTF8>(&result) << iVal;
	return result;
	}

} // namespace Util_Chan_UTF_Operators

#if not defined(ZMACRO_Util_Chan_UTF_Operators_Suppress) || not ZMACRO_Util_Chan_UTF_Operators_Suppress
	using Util_Chan_UTF_Operators::operator<<;
#endif

} // namespace ZooLib

#endif // __ZooLib_Util_Chan_UTF_Operators_string_h__
