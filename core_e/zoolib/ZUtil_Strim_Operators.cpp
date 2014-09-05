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

#include "zoolib/ZUtil_Strim_Operators.h"

// =================================================================================================
// MARK: - ZUtil_Strim_Operators

namespace ZooLib {
namespace ZUtil_Strim_Operators {

const ZStrimW& operator<<(const ZStrimW& w, const string32& iString)
	{ return w.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& w, const UTF32* iString)
	{ return w.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& w, UTF32* iString)
	{ return w.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& w, const string16& iString)
	{ return w.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& w, const UTF16* iString)
	{ return w.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& w, UTF16* iString)
	{ return w.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& w, const string8& iString)
	{ return w.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& w, const UTF8* iString)
	{ return w.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& w, UTF8* iString)
	{ return w.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& w, const ZStrimR& r)
	{ return w.CopyAllFrom(r); }

const ZStrimW& operator<<(const ZStrimW& w, bool iVal)
	{ return w.Write(iVal ? "true" : "false"); }

const ZStrimW& operator<<(const ZStrimW& w, char iVal)
	{ return w.Writef("%c", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, unsigned char iVal)
	{ return w.Writef("%u", (unsigned int)iVal); }

const ZStrimW& operator<<(const ZStrimW& w, signed char iVal)
	{ return w.Writef("%d", int(iVal)); }

const ZStrimW& operator<<(const ZStrimW& w, __wchar_t iVal)
	{ return w.Writef("%lc", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, short iVal)
	{ return w.Writef("%d", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, unsigned short iVal)
	{ return w.Writef("%u", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, int iVal)
	{ return w.Writef("%d", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, unsigned int iVal)
	{ return w.Writef("%u", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, long iVal)
	{ return w.Writef("%ld", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, unsigned long iVal)
	{ return w.Writef("%lu", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, __int64 iVal)
	{ return w.Writef("%lld", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, __uint64 iVal)
	{ return w.Writef("%llu", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, float iVal)
	{ return w.Writef("%.9g", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, double iVal)
	{ return w.Writef("%.17g", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, long double iVal)
	{
	// This is a guess for now.
	return w.Writef("%.34Lg", iVal);
	}

const ZStrimW& operator<<(const ZStrimW& w, const void* iVal)
	{ return w.Writef("%p", iVal); }

const ZStrimW& operator<<(const ZStrimW& w, void* iVal)
	{ return w.Writef("%p", iVal); }

} // namespace ZUtil_Strim_Operators
} // namespace ZooLib
