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
#include "zoolib/ZTypes.h" // For sDefault

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
:	fIntPtr(0)
,	fIsCounted(false)
	{}

ZName::ZName(const ZName& iOther)
:	fIntPtr(iOther.fIntPtr)
,	fIsCounted(iOther.fIsCounted)
	{
	if (fIsCounted)
		sFetch_T<ZRefCountedString>(&fIntPtr)->Retain();
	}

ZName& ZName::operator=(const ZName& iOther)
	{
	if (fIsCounted)
		{
		if (iOther.fIsCounted)
			{
			sAssignFromVoidStar_T<ZRefCountedString>(&fIntPtr, &iOther.fIntPtr);
			}
		else
			{
			sFetch_T<ZRefCountedString>(&fIntPtr)->Release();
			fIntPtr = iOther.fIntPtr;
			fIsCounted = false;
			}
		}
	else if (iOther.fIsCounted)
		{
		sCtorFromVoidStar_T<ZRefCountedString>(&fIntPtr, &iOther.fIntPtr);
		fIsCounted = true;
		}
	else
		{	
		fIntPtr = iOther.fIntPtr;
		}
	return *this;
	}

ZName::~ZName()
	{
	if (fIsCounted)
		sFetch_T<ZRefCountedString>(&fIntPtr)->Release();
	}

ZName::ZName(const char* iStatic)
:	fIntPtr(((intptr_t)iStatic))
,	fIsCounted(false)
	{}

ZName::ZName(const string8& iString)
:	fIsCounted(true)
	{ sCtor_T<ZRefCountedString>(&fIntPtr, sCountedVal(iString)); }

ZName::ZName(const ZRefCountedString& iCountedString)
	{
	if (iCountedString)
		{
		sCtor_T<ZRefCountedString>(&fIntPtr, iCountedString);
		fIsCounted = true;
		}
	else
		{
		fIntPtr = 0;
		fIsCounted = false;
		}
	}

ZName::operator string8() const
	{
	if (fIsCounted)
		return sFetch_T<ZName::ZRefCountedString>(&fIntPtr)->Get()->Get();
	else if (fIntPtr)
		return (const char*)(fIntPtr);
	return sDefault<string8>();
	}

bool ZName::operator<(const ZName& iOther) const
	{ return this->Compare(iOther) < 0; }

bool ZName::operator==(const ZName& iOther) const
	{ return this->Compare(iOther) == 0; }

int ZName::Compare(const ZName& iOther) const
	{
	if (const char* lhs = spAsCharStar(fIntPtr, fIsCounted))
		{
		if (const char* rhs = spAsCharStar(iOther.fIntPtr, iOther.fIsCounted))
			return std::strcmp(lhs, rhs);
		else
			return 1;
		}

	if (spAsCharStar(iOther.fIntPtr, iOther.fIsCounted))
		return -1;

	return 0;
	}

bool ZName::IsNull() const
	{ return not fIsCounted && not fIntPtr; }

void ZName::Clear()
	{
	if (fIsCounted)
		{
		sFetch_T<ZRefCountedString>(&fIntPtr)->Release();
		fIsCounted = false;
		}
	fIntPtr = 0;
	}

std::size_t ZName::Hash() const
	{
	size_t result = 0;
	if (const char* i = spAsCharStar(fIntPtr, fIsCounted))
		{
		for (size_t x = sizeof(size_t); --x && *i; ++i)
			{
			result <<= 8;
			result |= *i;
			}
		}
	return result;
	}

} // namespace ZooLib
