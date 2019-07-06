// Copyright (c) 2019 Andrew Green
// http://www.zoolib.org

#ifndef __ZooLib_Pull_ML_h__
#define __ZooLib_Pull_ML_h__ 1
#include "zconfig.h"

#include "zoolib/ChanRU_UTF.h"
#include "zoolib/ML.h"
#include "zoolib/PullPush.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Pull_ML

namespace Pull_ML {

using ML::Attrs_t;

class Marker_Attrs
:	public virtual PullPush::Marker
	{
protected:
	Marker_Attrs(const Attrs_t& iAttrs) : fAttrs(iAttrs) {}
	Marker_Attrs() {}
	Attrs_t fAttrs;

public:
	static PPT sPPT(const Attrs_t& iAttrs);

	static bool sIs(const PPT& iPPT);
	};

// --

class Marker_Named
:	public virtual PullPush::Marker
	{
protected:
	Marker_Named(const Name& iName) : fName(iName) {}
	Name fName;

public:
	static PPT sPPT(const Name& iName);

	static bool sIs(const PPT& iPPT);
	};

// --

class TagBegin
:	public PullPush::Start_Seq
,	public Marker_Named
,	public Marker_Attrs
	{
protected:
	TagBegin(const Name& iName, const Attrs_t& iAttrs)
	:	Marker_Named(iName)
	,	Marker_Attrs(iAttrs)
		{}

	TagBegin(const Name& iName) : Marker_Named(iName) {}

public:
	static PPT sPPT(const Name& iName, const Attrs_t& iAttrs);
	static PPT sPPT(const Name& iName);

	static bool sIs(const PPT& iPPT);
	};

class TagEmpty
:	public Marker_Named
,	public Marker_Attrs
	{
protected:
	TagEmpty(const Name& iName, const Attrs_t& iAttrs)
	:	Marker_Named(iName)
	,	Marker_Attrs(iAttrs)
		{}

	TagEmpty(const Name& iName) : Marker_Named(iName) {}

public:
	static PPT sPPT(const Name& iName, const Attrs_t& iAttrs);
	static PPT sPPT(const Name& iName);

	static bool sIs(const PPT& iPPT);
	};

class TagEnd
:	public PullPush::End
,	public Marker_Named
	{
protected:
	TagEnd(const Name& iName) : Marker_Named(iName) {}

public:
	static PPT sPPT(const Name& iName);
	static bool sIs(const PPT& iPPT);
	};

// =================================================================================================
#pragma mark -

void sPush_TagBegin(const Name& iName, const ChanW_PPT& iChanW);

void sPush_TagBegin(const Name& iName, const Attrs_t& iAttrs, const ChanW_PPT& iChanW);

void sPush_TagEmpty(const Name& iName, const ChanW_PPT& iChanW);

void sPush_TagEmpty(const Name& iName, const Attrs_t& iAttrs, const ChanW_PPT& iChanW);

void sPush_TagEnd(const Name& iName, const ChanW_PPT& iChanW);

//bool sIsTagBegin(const PPT& iPPT);
//bool sIsTagEmpty(const PPT& iPPT);
//bool sIsTagEnd(const PPT& iPPT);

} // namespace Pull_ML

// =================================================================================================
#pragma mark -

bool sPull_ML_Push_PPT(const ChanRU_UTF& iChanRU,
	bool iRecognizeEntitiesInAttributeValues, const ZP<ML::Callable_Entity>& iCallable_Entity,
	const ChanW_PPT& iChanW);

} // namespace ZooLib

#endif // __ZooLib_Pull_ML_h__
