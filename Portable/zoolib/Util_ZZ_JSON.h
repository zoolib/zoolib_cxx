// Copyright (c) 2012-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_JSON_ZZ_h__
#define __ZooLib_Util_JSON_ZZ_h__ 1
#include "zconfig.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/PullPush_JSON.h"
#include "zoolib/Val_ZZ.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace Util_ZZ_JSON {

using namespace Util_Chan_JSON;

ZQ<Val_ZZ> sQRead(const ChanRU_UTF& iChanRU);
ZQ<Val_ZZ> sQRead(const ChanRU_UTF& iChanRU, const PullTextOptions_JSON& iRO);

void sWrite(const Val_ZZ& iVal, const ChanW_UTF& iChanW);
void sWrite(const Val_ZZ& iVal, bool iPrettyPrint, const ChanW_UTF& iChanW);
void sWrite(const Val_ZZ& iVal, size_t iInitialIndent, const PushTextOptions_JSON& iOptions, const ChanW_UTF& iChanW);

string8 sAsJSON(const Val_ZZ& iVal);

const Val_ZZ sFromJSON(const string8& iString);

} // namespace Util_ZZ_JSON

namespace Operators_ZZ_JSON {

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Val_ZZ& iVal);
const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Map_ZZ& iMap);
const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Seq_ZZ& iSeq);
const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Data_ZZ& iData);

} // namespace Operators_ZZ_JSON

} // namespace ZooLib

#endif // __ZooLib_Util_JSON_ZZ_h__
