/* -------------------------------------------------------------------------------------------------
Copyright (c) 20012 Andrew Green
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

#ifndef __ZUtil_Strim_string_h__
#define __ZUtil_Strim_string_h__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZUtil_Strim_string

template <class Val_p>
string32 operator<<(const string32& iString, const Val_p& iVal)
	{
	string32 result = iString;
	ZStrimW_String<string32>(result) << iVal;
	return result;
	}

template <class Val_p>
string16 operator<<(const string16& iString, const Val_p& iVal)
	{
	string16 result = iString;
	ZStrimW_String<string16>(result) << iVal;
	return result;
	}

template <class Val_p>
string8 operator<<(const string8& iString, const Val_p& iVal)
	{
	string8 result = iString;
	ZStrimW_String<string8>(result) << iVal;
	return result;
	}

} // namespace ZooLib

#endif // __ZUtil_Strim_string_h__
