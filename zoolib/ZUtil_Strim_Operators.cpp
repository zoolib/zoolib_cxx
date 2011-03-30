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

namespace ZooLib {
namespace ZUtil_Strim_Operators {

const ZStrimW& operator<<(const ZStrimW& s, const string32& iString)
	{ return s.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& s, const UTF32* iString)
	{ return s.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& s, UTF32* iString)
	{ return s.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& s, const string16& iString)
	{ return s.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& s, const UTF16* iString)
	{ return s.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& s, UTF16* iString)
	{ return s.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& s, const string8& iString)
	{ return s.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& s, const UTF8* iString)
	{ return s.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& s, UTF8* iString)
	{ return s.Write(iString); }

const ZStrimW& operator<<(const ZStrimW& s, const ZStrimR& r)
	{ return s.CopyAllFrom(r); }

const ZStrimW& operator<<(const ZStrimW& s, bool iVal)
	{ return s.Write(iVal ? "true" : "false"); }
	
const ZStrimW& operator<<(const ZStrimW& s, char iVal)
	{ return s.Writef("%c", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, unsigned char iVal)
	{ return s.Writef("%uc", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, signed char iVal)
	{ return s.Writef("%c", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, wchar_t iVal)
	{ return s.Writef("%lc", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, short iVal)
	{ return s.Writef("%d", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, unsigned short iVal)
	{ return s.Writef("%u", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, int iVal)
	{ return s.Writef("%d", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, unsigned int iVal)
	{ return s.Writef("%u", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, long iVal)
	{ return s.Writef("%ld", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, unsigned long iVal)
	{ return s.Writef("%lu", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, int64 iVal)
	{ return s.Writef("%lld", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, uint64 iVal)
	{ return s.Writef("%llu", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, float iVal)
	{ return s.Writef("%.9g", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, double iVal)
	{ return s.Writef("%.17g", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, const void* iVal)
	{ return s.Writef("%p", iVal); }

const ZStrimW& operator<<(const ZStrimW& s, void* iVal)
	{ return s.Writef("%p", iVal); }

} // namespace ZUtil_Strim_Operators
} // namespace ZooLib
