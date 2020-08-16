// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ValPred_ValPred_Renamed_h__
#define __ZooLib_ValPred_ValPred_Renamed_h__ 1
#include "zconfig.h"

#include "zoolib/ValPred/ValPred.h"

#include <map>

namespace ZooLib {

ZP<ValComparand> sRenamed(const std::map<std::string,std::string>& iRename,
	const ZP<ValComparand>& iVal);

ValPred sRenamed(const std::map<std::string,std::string>& iRename,
	const ValPred& iValPred);

} // namespace ZooLib

#endif // __ZooLib_ValPred_ValPred_Renamed_h__
