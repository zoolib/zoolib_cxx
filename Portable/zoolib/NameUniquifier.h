// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_NameUniquifier_h__
#define __ZooLib_NameUniquifier_h__ 1
#include "zconfig.h"

#include "zoolib/Name.h"
#include "zoolib/ThreadVal.h"
#include "zoolib/Uniquifier.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - CountedStringUniquifier

typedef CountedVal<string8> CountedString;

typedef ZP<CountedString> ZP_CountedString;

struct Compare_ZP_CountedString
	{ bool operator()(const ZP_CountedString& ll, const ZP_CountedString& rr) const; };

typedef Uniquifier<ZP_CountedString,Compare_ZP_CountedString> CountedStringUniquifier;

// =================================================================================================
#pragma mark - ThreadVal_NameUniquifier

typedef ThreadVal<CountedStringUniquifier, struct Tag_NameUniquifier> ThreadVal_NameUniquifier;

// =================================================================================================
#pragma mark - sName

inline Name sName(const char* iConstCharStar)
	{ return Name(iConstCharStar); }

Name sName(const string8& iString);

inline Name sName(const Name& iName)
	{ return iName; }

inline Name sName(const ZP_CountedString& iCountedString);

} // namespace ZooLib

#endif // __ZooLib_NameUniquifier_h__
