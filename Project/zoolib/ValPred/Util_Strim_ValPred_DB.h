// Copyright (c) 2010-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ValPred_Util_Strim_ValPred_DB_h__
#define __ZooLib_ValPred_Util_Strim_ValPred_DB_h__ 1
#include "zconfig.h"

#include "zoolib/ChanRU_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/ZQ.h"

#include "zoolib/ValPred/ValPred.h"

#include <set>

namespace ZooLib {
namespace Util_Strim_ValPred_DB {

// =================================================================================================
#pragma mark - Util_Strim_ValPred_DB

void sToStrim(const ValPred& iValPred, const ChanW_UTF& iStrimW);

ZQ<ValPred> sQFromStrim(const ChanRU_UTF& iChanRU);

} // namespace Util_Strim_ValPred_DB

const ChanW_UTF& operator<<(const ChanW_UTF& w, const ValPred& iValPred);

} // namespace ZooLib

#endif // __ZooLib_ValPred_Util_Strim_ValPred_DB_h__
