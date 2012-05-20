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

#include "zoolib/ZCtorDtor.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZName.h"

#include <cstring>

namespace ZooLib {

// =================================================================================================
// MARK: -

static
const char* spAsCharStar(intptr_t iIntPtr, bool iIsCounted)
	{
	if (iIsCounted)
		return sFetch_T<ZName::ZRefCountedString>(&iIntPtr)->Get()->Get().c_str();
	return (const char*)(iIntPtr);
	}

ZName::ZName()
:	fAsIntPtr(0)
,	fIsCounted(false)
	{}

ZName::ZName(const ZName& iOther)
:	fAsIntPtr(iOther.fAsIntPtr)
,	fIsCounted(iOther.fIsCounted)
	{
	if (fIsCounted)
		sFetch_T<ZRefCountedString>(&fAsIntPtr)->Retain();
	}

ZName& ZName::operator=(const ZName& iOther)
	{
	if (fIsCounted)
		{
		if (iOther.fIsCounted)
			{
			sAssignFromVoidStar_T<ZRefCountedString>(&fAsIntPtr, &iOther.fAsIntPtr);
			}
		else
			{
			sFetch_T<ZRefCountedString>(&fAsIntPtr)->Release();
			fAsIntPtr = iOther.fAsIntPtr;
			fIsCounted = false;
			}
		}
	else if (iOther.fIsCounted)
		{
		sCtorFromVoidStar_T<ZRefCountedString>(&fAsIntPtr, &iOther.fAsIntPtr);
		fIsCounted = true;
		}
	else
		{	
		fAsIntPtr = iOther.fAsIntPtr;
		}
	return *this;
	}

ZName::~ZName()
	{
	if (fIsCounted)
		sFetch_T<ZRefCountedString>(&fAsIntPtr)->Release();
	}

ZName::ZName(const char* iStatic)
:	fAsIntPtr(((intptr_t)iStatic))
,	fIsCounted(false)
	{}

ZName::ZName(const string8& iString)
:	fIsCounted(true)
	{ sCtor_T<ZRefCountedString>(&fAsIntPtr, sCountedVal(iString)); }

ZName::ZName(const ZRefCountedString& iCountedString)
	{
	if (iCountedString)
		{
		sCtor_T<ZRefCountedString>(&fAsIntPtr, iCountedString);
		fIsCounted = true;
		}
	else
		{
		fAsIntPtr = 0;
		fIsCounted = false;
		}
	}

ZName::operator string8() const
	{
	if (fIsCounted)
		return sFetch_T<ZName::ZRefCountedString>(&fAsIntPtr)->Get()->Get();
	else
		return (const char*)(fAsIntPtr);
	}

bool ZName::operator<(const ZName& iOther) const
	{ return this->Compare(iOther) < 0; }

bool ZName::operator==(const ZName& iOther) const
	{ return this->Compare(iOther) == 0; }

int ZName::Compare(const ZName& iOther) const
	{
	return std::strcmp
		(spAsCharStar(fAsIntPtr, fIsCounted),
		spAsCharStar(iOther.fAsIntPtr, iOther.fIsCounted));
	}

} // namespace ZooLib
