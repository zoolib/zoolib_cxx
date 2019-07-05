// Copyright (c) 2019 Andrew Green
// http://www.zoolib.org

#include "zoolib/Pull_ML.h"

#include "zoolib/Log.h"
#include "zoolib/NameUniquifier.h" // For sName
#include "zoolib/ParseException.h"
#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace Pull_ML {

using namespace PullPush;
using std::string;

// =================================================================================================
#pragma mark -

void sPush_TagBegin(const Name& iName, const ChanW_PPT& iChanW)
	{ sPush_Marker(new TagBegin(iName), iChanW); }

void sPush_TagBegin(const Name& iName, const Attrs_t& iAttrs, const ChanW_PPT& iChanW)
	{ sPush_Marker(new TagBegin(iName, iAttrs), iChanW); }

void sPush_TagEmpty(const Name& iName, const ChanW_PPT& iChanW)
	{ sPush_Marker(new TagEmpty(iName), iChanW); }

void sPush_TagEmpty(const Name& iName, const Attrs_t& iAttrs, const ChanW_PPT& iChanW)
	{ sPush_Marker(new TagEmpty(iName, iAttrs), iChanW); }

void sPush_TagEnd(const Name& iName, const ChanW_PPT& iChanW)
	{ sPush_Marker(new TagEnd(iName), iChanW); }

// =================================================================================================
#pragma mark -

bool sPull_ML_Push_PPT(const ChanRU_UTF& iChanRU, const ChanW_PPT& iChanW)
	{
	return false;
	}

} // namespace Pull_ML
} // namespace ZooLib
