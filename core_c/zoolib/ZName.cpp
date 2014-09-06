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

#include "zoolib/CtorDtor.h"
#include "zoolib/Singleton.h" // For sDefault

#include "zoolib/ZDebug.h"
#include "zoolib/ZName.h"

#include <cstring>

namespace ZooLib {

// =================================================================================================
// MARK: -

ZName& ZName::operator=(const ZName& iOther)
	{
	if (CountedString* self = this->pGetIfCounted())
		{
		if (const CountedString* other = iOther.pGetIfCounted())
			{
			spRetain(other);
			}
		else
			{
			#if not ZCONFIG_Is64Bit
				fIsCounted = false;
			#endif
			}
		spRelease(self);
		}
	else if (const CountedString* other = iOther.pGetIfCounted())
		{
		spRetain(other);
		#if not ZCONFIG_Is64Bit
			fIsCounted = true;
		#endif
		}
	fIntPtr = iOther.fIntPtr;
	return *this;
	}

ZName::ZName(const string8& iString)
#if ZCONFIG_Is64Bit
	{
	CountedString* theCountedString = new CountedString(iString);
	spRetain(theCountedString);
	fIntPtr = ((intptr_t)theCountedString) ^ 1ULL<<63;
	}
#else
:	fIsCounted(true)
	{
	CountedString* theCountedString = new CountedString(iString);
	spRetain(theCountedString);
	fIntPtr = (intptr_t)theCountedString;
	}
#endif

ZName::ZName(const RefCountedString& iRefCountedString)
	{
	CountedString* theCountedString = iRefCountedString.Get();
	#if ZCONFIG_Is64Bit
		if (theCountedString)
			{
			spRetain(theCountedString);
			fIntPtr = ((intptr_t)theCountedString) ^ 1ULL<<63;
			}
		else
			{
			fIntPtr = 0;
			}
	#else
		if (theCountedString)
			{
			spRetain(theCountedString);
			fIntPtr = (intptr_t)theCountedString;
			fIsCounted = true;
			}
		else
			{
			fIntPtr = 0;
			fIsCounted = false;
			}
	#endif
	}

ZName::operator string8() const
	{
	#if ZCONFIG_Is64Bit
		if ((bool(fIntPtr & 1ULL<<63)) != (bool(fIntPtr & 1ULL<<62)))
			return ((const CountedString*)(fIntPtr ^ 1ULL<<63))->Get();
	#else
		if (fIsCounted)
			return ((const CountedString*)fIntPtr)->Get();
	#endif

	if (fIntPtr)
		return (const char*)(fIntPtr);

	return sDefault<string8>();
	}

ZName::operator RefCountedString() const
	{
	#if ZCONFIG_Is64Bit
		if ((bool(fIntPtr & 1ULL<<63)) != (bool(fIntPtr & 1ULL<<62)))
			return (CountedString*)(fIntPtr ^ 1ULL<<63);
	#else
		if (fIsCounted)
			return (CountedString*)fIntPtr;
	#endif

	if (fIntPtr)
		return new CountedString((const char*)fIntPtr);

	return null;
	}

int ZName::Compare(const ZName& iOther) const
	{
	if (const char* lhs = this->pAsCharStar())
		{
		if (const char* rhs = iOther.pAsCharStar())
			return std::strcmp(lhs, rhs);
		else
			return 1;
		}
	else if (iOther.pAsCharStar())
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
	#if ZCONFIG_Is64Bit
		if ((bool(fIntPtr & 1ULL<<63)) != (bool(fIntPtr & 1ULL<<62)))
			return ((const CountedString*)(fIntPtr ^ 1ULL<<63))->Get().empty();
	#else
		if (fIsCounted)
			return ((const CountedString*)(fIntPtr))->Get().empty();
	#endif

	if (fIntPtr)
		return not ((const char*)(fIntPtr))[0];

	return true;
	}

void ZName::Clear()
	{
	if (const CountedString* theCounted = this->pGetIfCounted())
		spRelease(theCounted);
	fIntPtr = 0;

	#if not ZCONFIG_Is64Bit
		fIsCounted = false;
	#endif
	}

std::size_t ZName::Hash() const
	{
	size_t result = 0;
	if (const char* i = this->pAsCharStar())
		{
		for (size_t xx = sizeof(size_t); --xx && *i; ++i)
			{
			result <<= 8;
			result |= *i;
			}
		}
	return result;
	}

const char* ZName::pAsCharStar() const
	{
	#if ZCONFIG_Is64Bit
		if ((bool(fIntPtr & 1ULL<<63)) != (bool(fIntPtr & 1ULL<<62)))
			return ((const CountedString*)(fIntPtr ^ 1ULL<<63))->Get().c_str();
	#else
		if (fIsCounted)
			return ((const CountedString*)(fIntPtr))->Get().c_str();
	#endif
	return (const char*)fIntPtr;
	}

const ZName::CountedString* ZName::pGetIfCounted() const
	{ return const_cast<ZName*>(this)->pGetIfCounted(); }

ZName::CountedString* ZName::pGetIfCounted()
	{
	#if ZCONFIG_Is64Bit
		if ((bool(fIntPtr & 1ULL<<63)) != (bool(fIntPtr & 1ULL<<62)))
			return (CountedString*)(fIntPtr ^ 1ULL<<63);
	#else
		if (fIsCounted)
			return (CountedString*)fIntPtr;
	#endif
	return nullptr;
	}


} // namespace ZooLib
