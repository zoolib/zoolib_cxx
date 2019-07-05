// Copyright (c) 2019 Andrew Green
// http://www.zoolib.org

#ifndef __ZooLib_Pull_ML_h__
#define __ZooLib_Pull_ML_h__ 1
#include "zconfig.h"

#include "zoolib/ChanRU_UTF.h"
#include "zoolib/PullPush.h"

#include <utility> // For std::pair
#include <vector>

namespace ZooLib {
namespace Pull_ML {

// =================================================================================================

typedef std::pair<Name, std::string> Attr_t;
typedef std::vector<Attr_t> Attrs_t;

struct Marker_Attrs
:	virtual PullPush::Marker
	{
	Marker_Attrs(const Attrs_t& iAttrs) : fAttrs(iAttrs) {}
	Marker_Attrs() {}
	Attrs_t fAttrs;
	};

struct Marker_Named
:	virtual PullPush::Marker
	{
	Marker_Named(const Name& iName) : fName(iName) {}
	Name fName;
	};

// --

struct TagBegin
:	PullPush::Start_Seq
,	Marker_Named
,	Marker_Attrs
	{
	TagBegin(const Name& iName, const Attrs_t& iAttrs)
	:	Marker_Named(iName)
	,	Marker_Attrs(iAttrs)
		{}

	TagBegin(const Name& iName) : Marker_Named(iName) {}
	};

struct TagEmpty
:	Marker_Named
,	Marker_Attrs
	{
	TagEmpty(const Name& iName, const Attrs_t& iAttrs)
	:	Marker_Named(iName)
	,	Marker_Attrs(iAttrs)
		{}

	TagEmpty(const Name& iName) : Marker_Named(iName) {}
	};


struct TagEnd
:	PullPush::End
,	Marker_Named
	{
	TagEnd(const Name& iName) : Marker_Named(iName) {}
	};

// =================================================================================================
#pragma mark -

void sPush_TagBegin(const Name& iName, const ChanW_PPT& iChanW);

void sPush_TagBegin(const Name& iName, const Attrs_t& iAttrs, const ChanW_PPT& iChanW);

void sPush_TagEmpty(const Name& iName, const ChanW_PPT& iChanW);

void sPush_TagEmpty(const Name& iName, const Attrs_t& iAttrs, const ChanW_PPT& iChanW);

void sPush_TagEnd(const Name& iName, const ChanW_PPT& iChanW);

bool sIsTagBegin(const PPT& iPPT);
bool sIsTagEmpty(const PPT& iPPT);
bool sIsTagEnd(const PPT& iPPT);

// =================================================================================================
#pragma mark - 

bool sPull_ML_Push_PPT(const ChanRU_UTF& iChanRU, const ChanW_PPT& iChanW);

} // namespace Pull_ML
} // namespace ZooLib

#endif // __ZooLib_Pull_ML_h__
