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

// It would be nice to be able to steal the bottom bit of fIntPtr to use as fIsCounted. The
// problem is that const char* are often packed tightly and thus often have odd addresses, so
// every bit can be significant.

// =================================================================================================
// MARK: -

static
const char* spAsCharStar(intptr_t iIntPtr, bool iIsCounted)
	{
	if (iIsCounted)
		return sFetch_T<ZName::ZRefCountedString>(&iIntPtr)->Get()->Get().c_str();
	return (const char*)(iIntPtr);
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

int ZName::Compare(const ZName& iOther) const
	{
	if (const char* lhs = spAsCharStar(fIntPtr, fIsCounted))
		{
		if (const char* rhs = spAsCharStar(iOther.fIntPtr, iOther.fIsCounted))
			return std::strcmp(lhs, rhs);
		else
			return 1;
		}
	else if (spAsCharStar(iOther.fIntPtr, iOther.fIsCounted))
		{
		return -1;
		}
	else
		{
		return 0;
		}
	}

bool ZName::IsEmpty() const
	{
	if (fIsCounted)
		return sFetch_T<ZRefCountedString>(&fIntPtr)->Get()->Get().empty();
	else if (fIntPtr)
		return not ((const char*)(fIntPtr))[0];
	return true;
	}

void ZName::Clear()
	{
	if (sGetSet(fIsCounted, false))
		sFetch_T<ZRefCountedString>(&fIntPtr)->Release();
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
