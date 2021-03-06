// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pull_ML_h__
#define __ZooLib_Pull_ML_h__ 1
#include "zconfig.h"

#include "zoolib/ML.h"
#include "zoolib/PullPush.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Pull_ML

namespace Pull_ML {

using ML::Attrs_t;

// =================================================================================================
#pragma mark - Marker_Attrs

class Marker_Attrs
:	public virtual PullPush::Marker
	{
protected:
	Marker_Attrs(const Attrs_t& iAttrs) : fAttrs(iAttrs) {}
	Marker_Attrs() {}

	Attrs_t fAttrs;

public:
	const Attrs_t& GetAttrs_t() { return fAttrs; }

	static PPT sPPT(const Attrs_t& iAttrs);

	static bool sIs(const PPT& iPPT);
	};

// =================================================================================================
#pragma mark - Marker_Named

class Marker_Named
:	public virtual PullPush::Marker
	{
protected:
	Marker_Named(const Name& iName) : fName(iName) {}

	const Name fName;

public:
	const Name& GetName() { return fName; }

	static PPT sPPT(const Name& iName);

	static bool sIs(const PPT& iPPT);
	};

// =================================================================================================
#pragma mark - TagBegin

class TagBegin
:	public PullPush::Start_Seq
,	public Marker_Attrs
,	public Marker_Named
	{
protected:
	TagBegin(const Name& iName, const Attrs_t& iAttrs)
	:	Marker_Attrs(iAttrs)
	,	Marker_Named(iName)
		{}

	TagBegin(const Name& iName) : Marker_Named(iName) {}

public:
	static PPT sPPT(const Name& iName, const Attrs_t& iAttrs);
	static PPT sPPT(const Name& iName);

	static bool sIs(const PPT& iPPT);
	static ZP<TagBegin> sAs(const PPT& iPPT);
	};

// =================================================================================================
#pragma mark - TagEmpty

class TagEmpty
:	public Marker_Attrs
,	public Marker_Named
	{
protected:
	TagEmpty(const Name& iName, const Attrs_t& iAttrs)
	:	Marker_Attrs(iAttrs)
	,	Marker_Named(iName)
		{}

	TagEmpty(const Name& iName) : Marker_Named(iName) {}

public:
	static PPT sPPT(const Name& iName, const Attrs_t& iAttrs);
	static PPT sPPT(const Name& iName);

	static bool sIs(const PPT& iPPT);
	static ZP<TagEmpty> sAs(const PPT& iPPT);
	};

// =================================================================================================
#pragma mark - TagEnd

class TagEnd
:	public PullPush::End
,	public Marker_Named
	{
protected:
	TagEnd(const Name& iName) : Marker_Named(iName) {}

public:
	static PPT sPPT(const Name& iName);
	static bool sIs(const PPT& iPPT);
	static ZP<TagEnd> sAs(const PPT& iPPT);
	};

// =================================================================================================
#pragma mark -

void sPush_TagBegin(const Name& iName, const ChanW_PPT& iChanW);

void sPush_TagBegin(const Name& iName, const Attrs_t& iAttrs, const ChanW_PPT& iChanW);

void sPush_TagEmpty(const Name& iName, const ChanW_PPT& iChanW);

void sPush_TagEmpty(const Name& iName, const Attrs_t& iAttrs, const ChanW_PPT& iChanW);

void sPush_TagEnd(const Name& iName, const ChanW_PPT& iChanW);

// =================================================================================================
#pragma mark -

ZQ<PPT> sQSkipText_Read(const ChanR_PPT& iChanR);

PPT sESkipText_Read(const ChanR_PPT& iChanR);

void sESkipText_ReadEnd(const ChanR_PPT& iChanR, const Name& iTagName);

bool sIsText(const PPT& iPPT);

ZQ<std::string> sQAsString(const PPT& iPPT);

} // namespace Pull_ML

// =================================================================================================
#pragma mark -

bool sPull_ML_Push_PPT(const ChanRU_UTF& iChanRU,
	bool iRecognizeEntitiesInAttributeValues, const ZP<ML::Callable_Entity>& iCallable_Entity,
	const ChanW_PPT& iChanW);

} // namespace ZooLib

#endif // __ZooLib_Pull_ML_h__
