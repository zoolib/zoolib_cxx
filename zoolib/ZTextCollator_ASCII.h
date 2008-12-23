/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTextCollator_ASCII__
#define __ZTextCollator_ASCII__ 1
#include "zconfig.h"

#include "zoolib/ZTextCollator.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZTextCollatorRep_ASCII

class ZTextCollatorRep_ASCII : public ZTextCollatorRep_NativeUTF8
	{
public:
	ZTextCollatorRep_ASCII(int iStrength);
	virtual ~ZTextCollatorRep_ASCII();

// From ZTextCollatorRep via ZTextCollatorRep_NativeUTF8
	virtual ZRef<ZTextCollatorRep> Clone();
	virtual ZRef<ZTextCollatorRep> SafeCopy();

	using ZTextCollatorRep::Equals;
	using ZTextCollatorRep::Compare;
	using ZTextCollatorRep::Contains;

	virtual bool Equals(const string8& iLeft, const string8& iRight);
	virtual bool Equals(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength);

	virtual int Compare(const string8& iLeft, const string8& iRight);
	virtual int Compare(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength);

	virtual bool Contains(const string8& iPattern, const string8& iTarget);
	virtual bool Contains(const UTF8* iPattern, size_t iPatternLength,
		const UTF8* iTarget, size_t iTargetLength);

private:
	int fStrength;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZTextCollator_ASCII__
