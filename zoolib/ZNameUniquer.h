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

#ifndef __ZNameUniquer_h__
#define __ZNameUniquer_h__ 1
#include "zconfig.h"

#include "zoolib/ZName.h"
#include "zoolib/ZThreadVal.h"
#include "zoolib/ZUniquer.h"

namespace ZooLib {

// =================================================================================================
// MARK: - NameUniquer

typedef ZCountedVal<string8> ZCountedString;
typedef ZRef<ZCountedString> ZRefCountedString;

struct Compare_RefCountedString
	{ bool operator()(const ZRefCountedString& l, const ZRefCountedString& r); };

typedef ZUniquer<ZRefCountedString,Compare_RefCountedString> ZCountedStringUniquer;

typedef ZThreadVal<ZCountedStringUniquer, struct Tag_NameUniquer> ZThreadVal_NameUniquer;

// =================================================================================================
// MARK: - sName

inline ZName sName(const char* iConstCharStar)
	{ return ZName(iConstCharStar); }

ZName sName(const string8& iString);

ZName sName(const ZRefCountedString& iCountedString);

} // namespace ZooLib

#endif // __ZNameUniquer_h__
