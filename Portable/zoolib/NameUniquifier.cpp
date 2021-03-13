// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/NameUniquifier.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Compare_ZP_CountedString

bool Compare_ZP_CountedString::operator()(const ZP_CountedString& ll, const ZP_CountedString& rr) const
	{
	const string8& ls = ll->Get();
	const string8& rs = rr->Get();
	return ls < rs;
//	return l->Get() < r->Get();
	}

// =================================================================================================
#pragma mark - sName

Name sName(const string8& iString)
	{ return sName(sCountedVal<string8>(iString)); }

Name sName(const ZP_CountedString& iCountedString)
	{
	if (ThreadVal_NameUniquifier::Type_t* theUniquifier = ThreadVal_NameUniquifier::sPMut())
		{
		ZP_CountedString theCS = theUniquifier->Get(iCountedString);
		int theCount = theCS->RefCount();
		return Name(theCS);
		}
	return Name(iCountedString);
	}

} // namespace ZooLib
