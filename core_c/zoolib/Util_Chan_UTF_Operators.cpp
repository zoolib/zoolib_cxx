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

#include "zoolib/Util_Chan.h" // For sCopyAll
#include "zoolib/Util_Chan_UTF_Operators.h"

// =================================================================================================
// MARK: - Util_Chan_UTF_Operators

namespace ZooLib {
namespace Util_Chan_UTF_Operators {

const ChanW_UTF& operator<<(const ChanW_UTF& w, const string32& iString)
	{
	sWrite(iString, w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const UTF32* iString)
	{
	sWrite(iString, w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, UTF32* iString)
	{
	sWrite(iString, w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const string16& iString)
	{
	sWrite(iString, w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const UTF16* iString)
	{
	sWrite(iString, w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, UTF16* iString)
	{
	sWrite(iString, w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const string8& iString)
	{
	sWrite(iString, w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const UTF8* iString)
	{
	sWrite(iString, w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, UTF8* iString)
	{
	sWrite(iString, w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const ChanR_UTF& r)
	{
	Util_Chan::sCopyAll(r, w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, bool iVal)
	{
	sWrite(iVal ? "true" : "false", w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, char iVal)
	{
	sWritef(w, "%c", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, unsigned char iVal)
	{
	sWritef(w, "%u", (unsigned int)iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, signed char iVal)
	{
	sWritef(w, "%d", int(iVal));
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, __wchar_t iVal)
	{
	sWritef(w, "%lc", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, short iVal)
	{
	sWritef(w, "%d", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, unsigned short iVal)
	{
	sWritef(w, "%u", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, int iVal)
	{
	sWritef(w, "%d", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, unsigned int iVal)
	{
	sWritef(w, "%u", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, long iVal)
	{
	sWritef(w, "%ld", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, unsigned long iVal)
	{
	sWritef(w, "%lu", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, __int64 iVal)
	{
	sWritef(w, "%lld", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, __uint64 iVal)
	{
	sWritef(w, "%llu", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, float iVal)
	{
	sWritef(w, "%.9g", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, double iVal)
	{
	sWritef(w, "%.17g", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, long double iVal)
	{
	// This is a guess for now.
	sWritef(w, "%.34Lg", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const void* iVal)
	{
	sWritef(w, "%p", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, void* iVal)
	{
	sWritef(w, "%p", iVal);
	return w;
	}

} // namespace ZUtil_Strim_Operators
} // namespace ZooLib
