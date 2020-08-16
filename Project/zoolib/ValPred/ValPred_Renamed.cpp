// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ValPred/ValPred_Renamed.h"

#include "zoolib/Util_STL_map.h"
#include "zoolib/ZQ.h"

namespace ZooLib {

using std::map;
using std::string;

using namespace Util_STL;

ZP<ValComparand> sRenamed(const map<string,string>& iRename,
	const ZP<ValComparand>& iVal)
	{
	if (ZP<ValComparand_Name> asName = iVal.DynamicCast<ValComparand_Name>())
		{
		if (ZQ<string> theQ = sQGet(iRename, asName->GetName()))
			return new ValComparand_Name(*theQ);
		}
	return iVal;
	}

ValPred sRenamed(const map<string,string>& iRename, const ValPred& iValPred)
	{
	return ValPred(
		sRenamed(iRename, iValPred.GetLHS()),
		iValPred.GetComparator(),
		sRenamed(iRename, iValPred.GetRHS()));
	}

} // namespace ZooLib
