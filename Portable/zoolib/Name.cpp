// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Name.h"

#include "zoolib/CtorDtor.h"
#include "zoolib/Default.h"

#include "zoolib/ZDebug.h"

#include <cstring>

namespace ZooLib {

#if ZMACRO_NameUsesString

#else // ZMACRO_NameUsesString

// =================================================================================================
#pragma mark -

Name& Name::operator=(const Name& iOther)
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

Name::Name(const string8& iString)
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

Name::Name(const ZPCountedString& iZPCountedString)
	{
	CountedString* theCountedString = iZPCountedString.Get();
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

Name::operator string8() const
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

Name::operator ZPCountedString() const
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

int Name::Compare(const Name& iOther) const
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

bool Name::IsEmpty() const
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

void Name::Clear()
	{
	if (const CountedString* theCounted = this->pGetIfCounted())
		spRelease(theCounted);
	fIntPtr = 0;

	#if not ZCONFIG_Is64Bit
		fIsCounted = false;
	#endif
	}

size_t Name::Hash() const
	{
	if (const CountedString* theCountedString = this->pGetIfCounted())
		{ return std::hash<string8>()(theCountedString->Get()); }

	const char* thePtr = (const char*)fIntPtr;

	#if defined(_LIBCPP_STRING)
		return std::__do_string_hash(thePtr, thePtr + strlen(thePtr));
	#elif defined(_BASIC_STRING_H)
		return std::_Hash_impl::hash(thePtr, strlen(thePtr));
	#else
		ZUnimplemented();
	#endif
	}

const char* Name::pAsCharStar() const
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

const Name::CountedString* Name::pGetIfCounted() const
	{ return const_cast<Name*>(this)->pGetIfCounted(); }

Name::CountedString* Name::pGetIfCounted()
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

#endif // ZMACRO_NameUsesString

} // namespace ZooLib
