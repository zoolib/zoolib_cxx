// Copyright (c) 2012-21 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_CountedStringUniquifier_h__
#define __ZooLib_CountedStringUniquifier_h__ 1
#include "zconfig.h"

#include "zoolib/CountedString.h"
#include "zoolib/ThreadVal.h"
#include "zoolib/Uniquifier.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - CountedStringUniquifier

typedef Uniquifier<ZP_CountedString,Compare_ZP_CountedString> CountedStringUniquifier;

// =================================================================================================
#pragma mark - ThreadVal_CountedStringUniquifier

typedef ThreadVal<CountedStringUniquifier, struct Tag_CountedStringUniquifier>
	ThreadVal_CountedStringUniquifier;

// =================================================================================================
#pragma mark - sCountedString

inline ZP_CountedString sCountedString(const ZP_CountedString& iCountedString)
	{
	if (ThreadVal_CountedStringUniquifier::Type_t* theUniquifier =
		ThreadVal_CountedStringUniquifier::sPMut())
		{
		return theUniquifier->Get(iCountedString);
		}
	return iCountedString;
	}

inline ZP_CountedString sCountedString(const string8& iString)
	{ return sCountedString(new CountedString(iString)); }

} // namespace ZooLib

#endif // __ZooLib_CountedStringUniquifier_h__
