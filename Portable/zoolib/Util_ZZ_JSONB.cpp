// Copyright (c) 2015-2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_ZZ_JSONB.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Log.h"
#include "zoolib/PullPush_JSONB.h"
#include "zoolib/PullPush_ZZ.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace Util_ZZ_JSONB {

ZQ<Val_ZZ> sQRead(const ChanR_Bin& iChanR)
	{
	PullPushPair<PPT> thePair = sMakePullPushPair<PPT>();
	ZP<Delivery<Val_ZZ>> theDelivery = sStartAsync_AsZZ(sGetClear(thePair.second));
	sPull_JSONB_Push_PPT(iChanR, null, *thePair.first);
	sDisconnectWrite(*thePair.first);

	return theDelivery->QGet();
	}

// -----

void sWrite(const ChanW_Bin& iChanW, const Val_ZZ& iVal)
	{ sPull_PPT_Push_JSONB(*sChannerR_PPT(iVal), null, iChanW); }

// -----

Data_ZZ sAsJSONB(const Val_ZZ& iVal)
	{
	Data_ZZ result;
	sWrite(ChanW_Bin_Data(&result), iVal);
	return result;
	}

} // namespace Util_ZZ_JSONB
} // namespace ZooLib

