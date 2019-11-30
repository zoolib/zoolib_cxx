// Copyright (c) 2014-2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Dataspace/Daton_Val.h"

#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/ChanRU_XX_Unreader.h"
#include "zoolib/Util_ZZ_JSON.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - spAsVal (anonymous)

namespace { // anonymous

ZQ<Val_DB> spQAsVal(const Data_ZZ& iData)
	{
	try
		{
		if (ZQ<Val_ZZ> theValQ = Util_ZZ_JSON::sQRead(
			ChanRU_XX_Unreader<UTF32>(
				ChanR_UTF_Chan_Bin_UTF8(
					ChanRPos_Bin_Data<Data_ZZ>(iData)))))
			{ return theValQ->As<Val_DB>(); }
		}
	catch (...) {}
	return null;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - Daton/Val conversion.

Val_DB sAsVal(const Daton& iDaton)
	{ return spQAsVal(iDaton.GetData()).Get(); }

Daton sAsDaton(const Val_DB& iVal)
	{
	Data_ZZ theData;
	Util_ZZ_JSON::sWrite(iVal.As<Val_ZZ>(), ChanW_UTF_Chan_Bin_UTF8(ChanRWPos_Bin_Data<Data_ZZ>(&theData)));
	return theData;
	}

} // namespace Dataspace
} // namespace ZooLib
