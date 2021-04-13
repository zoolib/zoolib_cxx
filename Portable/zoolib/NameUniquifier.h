// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_NameUniquifier_h__
#define __ZooLib_NameUniquifier_h__ 1
#include "zconfig.h"

#include "zoolib/CountedStringUniquifier.h"
#include "zoolib/Name.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - NameUniquifier

typedef CountedStringUniquifier NameUniquifier;

typedef ThreadVal_CountedStringUniquifier ThreadVal_NameUniquifier;

// =================================================================================================
#pragma mark - sName

inline Name sName(const Name& iName)
	{ return iName; }

inline Name sName(const ZP_CountedString& iCountedString)
	{ return Name(sCountedString(iCountedString)); }

inline Name sName(const string8& iString)
	{ return Name(sCountedString(iString)); }

inline Name sName(const char* iConstCharStar)
	{ return Name(iConstCharStar); }

} // namespace ZooLib

#endif // __ZooLib_NameUniquifier_h__
