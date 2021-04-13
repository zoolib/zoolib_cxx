// Copyright (c) 2012-2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_ZZ_JSON.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/ChanRU_XX_Unreader.h"
#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/Chan_UTF_string.h"
#include "zoolib/PullPush_JSON.h"
#include "zoolib/PullPush_ZZ.h"
//#include "zoolib/StartOnNewThread.h"

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

// =================================================================================================
#pragma mark -

void sWrite(const ChanW_UTF& iChanW, const Val_ZZ& iVal)
	{ sPull_PPT_Push_JSON(*sChannerR_PPT(iVal), iChanW); }

void sWrite(const ChanW_UTF& iChanW, const Val_ZZ& iVal, bool iPrettyPrint)
	{
	ThreadVal_PushTextIndent tv_PushTextIndent(0);
	ThreadVal<PushTextOptions_JSON> tv_Options(iPrettyPrint);

	sWrite(iChanW, iVal);
	}

void sWrite(const ChanW_UTF& iChanW, const Val_ZZ& iVal,
	size_t iInitialIndent, const PushTextOptions_JSON& iOptions)
	{
	ThreadVal_PushTextIndent tv_PushTextIndent(iInitialIndent);
	ThreadVal<PushTextOptions_JSON> tv_Options(iOptions);

	sWrite(iChanW, iVal);
	}

// =================================================================================================
#pragma mark -

string8 sAsJSON(const Val_ZZ& iVal)
	{
	string8 result;
	sWrite(ChanW_UTF_string8(&result), iVal);
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
	Util_ZZ_JSON::sWrite(iChanW, iVal);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Map_ZZ& iMap)
	{
	Util_ZZ_JSON::sWrite(iChanW, iMap);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Seq_ZZ& iSeq)
	{
	Util_ZZ_JSON::sWrite(iChanW, iSeq);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Data_ZZ& iData)
	{
	Util_ZZ_JSON::sWrite(iChanW, iData);
	return iChanW;
	}

} // namespace Operators_ZZ_JSON

} // namespace ZooLib

// =================================================================================================
#pragma mark - pdesc

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const Val_ZZ& iVal)
	{
	Util_ZZ_JSON::sWrite(StdIO::sChanW_UTF_Err, iVal);
	StdIO::sChanW_UTF_Err << "\n";
	}

ZMACRO_pdesc(const Map_ZZ& iMap)
	{
	Util_ZZ_JSON::sWrite(StdIO::sChanW_UTF_Err, iMap);
	StdIO::sChanW_UTF_Err << "\n";
	}

ZMACRO_pdesc(const Seq_ZZ& iSeq)
	{
	Util_ZZ_JSON::sWrite(StdIO::sChanW_UTF_Err, iSeq);
	StdIO::sChanW_UTF_Err << "\n";
	}

ZMACRO_pdesc(const Data_ZZ& iData)
	{
	Util_ZZ_JSON::sWrite(StdIO::sChanW_UTF_Err, iData);
	StdIO::sChanW_UTF_Err << "\n";
	}

#endif // defined(ZMACRO_pdesc)
