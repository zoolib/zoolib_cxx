// Copyright (c) 2015-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_JSONB_ZZ_h__
#define __ZooLib_Util_JSONB_ZZ_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/Val_ZZ.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace Util_ZZ_JSONB {

ZQ<Val_ZZ> sQRead(const ChanR_Bin& iChanR);

void sWrite(const ChanW_Bin& iChanW, const Val_ZZ& iVal);

Data_ZZ sAsJSONB(const Val_ZZ& iVal);

} // namespace Util_ZZ_JSONB
} // namespace ZooLib

#endif // __ZooLib_Util_JSONB_ZZ_h__
