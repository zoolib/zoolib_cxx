// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_CountedString_h__
#define __ZooLib_CountedString_h__ 1
#include "zconfig.h"

#include "zoolib/CountedVal.h"
#include "zoolib/UnicodeString.h" // For string8

namespace ZooLib {

// =================================================================================================
#pragma mark -

typedef CountedVal<string8> CountedString;

typedef ZP<CountedString> ZP_CountedString;

struct Compare_ZP_CountedString
	{
	bool operator()(const ZP_CountedString& ll, const ZP_CountedString& rr) const
		{ return ll->Get() < rr->Get(); }
	};

} // namespace ZooLib

#endif // __ZooLib_CountedString_h__
