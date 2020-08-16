// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ValPred/ValPred_GetNames.h"

#include "zoolib/Util_STL_set.h"

namespace ZooLib {

using std::set;
using std::string;

using namespace Util_STL;

// =================================================================================================
#pragma mark - ValPred, sGetNames

namespace { // anonymous

set<string> spGetNames(const ZP<ValComparand>& iComparand)
	{
	if (ZP<ValComparand_Name> asName = iComparand.DynamicCast<ValComparand_Name>())
		{
		const string& theName = asName->GetName();
		return set<string>(&theName, &theName + 1);
		}
	return set<string>();
	}

} // anonymous namespace

set<string> sGetNames(const ValPred& iValPred)
	{ return spGetNames(iValPred.GetLHS()) | spGetNames(iValPred.GetRHS()); }

} // namespace ZooLib
