// Copyright (c) 2012-2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_ZZ_JSON.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/ChanRU_XX_Unreader.h"
#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/Chan_UTF_string.h"
#include "zoolib/Promise.h"
#include "zoolib/PullPush_JSON.h"
#include "zoolib/PullPush_ZZ.h"
#include "zoolib/StartOnNewThread.h"

#include "zoolib/pdesc.h"
#if defined(ZMACRO_pdesc)
	#include "zoolib/StdIO.h"
	#include "zoolib/Util_Chan_UTF_Operators.h"
#endif

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace Util_ZZ_JSON {

ZQ<Val_ZZ> sQRead(const ChanRU_UTF& iChanRU, const PullTextOptions_JSON& iRO)
	{
	PullPushPair<PPT> thePair = sMakePullPushPair<PPT>();
	ZP<Delivery<Val_ZZ>> theDelivery = sStartAsync_AsZZ(sGetClear(thePair.second));
	sPull_JSON_Push_PPT(iChanRU, iRO, *thePair.first);
	sDisconnectWrite(*thePair.first);

	return theDelivery->QGet();
	}

ZQ<Val_ZZ> sQRead(const ChanRU_UTF& iChanRU)
	{ return sQRead(iChanRU, sPullTextOptions_Extended()); }

// -----

void sWrite(const Val_ZZ& iVal, const ChanW_UTF& iChanW)
	{ sWrite(iVal, false, iChanW); }

void sWrite(const Val_ZZ& iVal, bool iPrettyPrint, const ChanW_UTF& iChanW)
	{ sWrite(iVal, 0, PushTextOptions_JSON(iPrettyPrint), iChanW); }

static void spFromAny_Push_PPT(const Val_ZZ& iVal, const ZP<ChannerWCon_PPT>& iChannerWCon)
	{
	sFromZZ_Push_PPT(iVal, *iChannerWCon);
	sDisconnectWrite(*iChannerWCon);
	}

void sWrite(const Val_ZZ& iVal, size_t iInitialIndent, const PushTextOptions_JSON& iOptions, const ChanW_UTF& iChanW)
	{
	PullPushPair<PPT> thePair = sMakePullPushPair<PPT>();
	sStartOnNewThread(sBindR(sCallable(spFromAny_Push_PPT), iVal, sGetClear(thePair.first)));
	sPull_PPT_Push_JSON(*thePair.second, iInitialIndent, iOptions, iChanW);
	}

string8 sAsJSON(const Val_ZZ& iVal)
	{
	string8 result;
	sWrite(iVal, ChanW_UTF_string8(&result));
	return result;
	}

const Val_ZZ sFromJSON(const string8& iString)
	{ return sQRead(ChanRU_UTF_string8(iString)).Get(); }

} // namespace Util_ZZ_JSON

// =================================================================================================
#pragma mark -

namespace Operators_ZZ_JSON {

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Val_ZZ& iVal)
	{
	Util_ZZ_JSON::sWrite(iVal, iChanW);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Map_ZZ& iMap)
	{
	Util_ZZ_JSON::sWrite(iMap, iChanW);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Seq_ZZ& iSeq)
	{
	Util_ZZ_JSON::sWrite(iSeq, iChanW);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Data_ZZ& iData)
	{
	Util_ZZ_JSON::sWrite(iData, iChanW);
	return iChanW;
	}

} // namespace Operators_ZZ_JSON

} // namespace ZooLib

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const Val_ZZ& iVal)
	{
	Util_ZZ_JSON::sWrite(iVal, StdIO::sChan_UTF_Err);
	StdIO::sChan_UTF_Err << "\n";
	}

ZMACRO_pdesc(const Map_ZZ& iMap)
	{
	Util_ZZ_JSON::sWrite(iMap, StdIO::sChan_UTF_Err);
	StdIO::sChan_UTF_Err << "\n";
	}

ZMACRO_pdesc(const Seq_ZZ& iSeq)
	{
	Util_ZZ_JSON::sWrite(iSeq, StdIO::sChan_UTF_Err);
	StdIO::sChan_UTF_Err << "\n";
	}

ZMACRO_pdesc(const Data_ZZ& iData)
	{
	Util_ZZ_JSON::sWrite(iData, StdIO::sChan_UTF_Err);
	StdIO::sChan_UTF_Err << "\n";
	}

#endif // defined(ZMACRO_pdesc)
