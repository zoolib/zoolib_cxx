// Copyright (c) 2019 Andrew Green
// http://www.zoolib.org

#include "zoolib/Pull_ML.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan_UTF.h"

//#include "zoolib/Log.h"
//#include "zoolib/NameUniquifier.h" // For sName
//#include "zoolib/ParseException.h"
//#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {

using namespace ML;
using namespace PullPush;
using namespace Util_Chan;
using std::string;

// =================================================================================================
#pragma mark -Pull_ML

namespace Pull_ML {

PPT Marker_Attrs::sPPT(const Attrs_t& iAttrs)
	{ return ZP<Marker>(new Marker_Attrs(iAttrs)); }

bool Marker_Attrs::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<Marker_Attrs>())
			return true;
		}
	return false;
	}

// -------------------------------------------------------------------------------------------------

PPT Marker_Named::sPPT(const Name& iName)
	{ return ZP<Marker>(new Marker_Named(iName)); }

bool Marker_Named::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<Marker_Named>())
			return true;
		}
	return false;
	}

// -------------------------------------------------------------------------------------------------

PPT TagBegin::sPPT(const Name& iName, const Attrs_t& iAttrs)
	{ return ZP<Marker>(new TagBegin(iName, iAttrs)); }

PPT TagBegin::sPPT(const Name& iName)
	{ return ZP<Marker>(new TagBegin(iName)); }

bool TagBegin::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<TagBegin>())
			return true;
		}
	return false;
	}

// -------------------------------------------------------------------------------------------------

PPT TagEmpty::sPPT(const Name& iName, const Attrs_t& iAttrs)
	{ return ZP<Marker>(new TagEmpty(iName, iAttrs)); }

PPT TagEmpty::sPPT(const Name& iName)
	{ return ZP<Marker>(new TagEmpty(iName)); }

bool TagEmpty::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<TagEmpty>())
			return true;
		}
	return false;
	}

// -------------------------------------------------------------------------------------------------

PPT TagEnd::sPPT(const Name& iName)
	{ return ZP<Marker>(new TagEnd(iName)); }

bool TagEnd::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<TagEnd>())
			return true;
		}
	return false;
	}

// -------------------------------------------------------------------------------------------------

void sPush_TagBegin(const Name& iName, const ChanW_PPT& iChanW)
	{ sPush(TagBegin::sPPT(iName), iChanW); }

void sPush_TagBegin(const Name& iName, const Attrs_t& iAttrs, const ChanW_PPT& iChanW)
	{ sPush(TagBegin::sPPT(iName, iAttrs), iChanW); }

void sPush_TagEmpty(const Name& iName, const ChanW_PPT& iChanW)
	{ sPush(TagEmpty::sPPT(iName), iChanW); }

void sPush_TagEmpty(const Name& iName, const Attrs_t& iAttrs, const ChanW_PPT& iChanW)
	{ sPush(TagEmpty::sPPT(iName, iAttrs), iChanW); }

void sPush_TagEnd(const Name& iName, const ChanW_PPT& iChanW)
	{ sPush(TagEnd::sPPT(iName), iChanW); }

} // namespace Pull_ML

// =================================================================================================
#pragma mark -

bool sPull_ML_Push_PPT(const ChanRU_UTF& iChanRU,
	bool iRecognizeEntitiesInAttributeValues, const ZP<ML::Callable_Entity>& iCallable_Entity,
	const ChanW_PPT& iChanW)
	{
	bool readAny = false;
	for	(;;)
		{
		ZQ<UTF32> theCPQ = sQRead(iChanRU);
		if (not theCPQ)
			break;

		readAny = true;
		if (*theCPQ != '<')
			{
			PullPushPair<UTF32> thePullPushPair = sMakePullPushPair<UTF32>();
			sPush(sGetClear(thePullPushPair.second), iChanW);
			sFlush(iChanW);

			for (;;)
				{
				if (NotQ<UTF32> theCPQ = sQRead(iChanRU))
					{
					break;
					}
				else if (*theCPQ == '<')
					{
					sUnread(iChanRU, *theCPQ);
					break;
					}
				else if (*theCPQ == '&')
					{
					sEWrite(*thePullPushPair.first, sReadReference(iChanRU, iCallable_Entity));
					}
				}
			sDisconnectWrite(*thePullPushPair.first);
			continue;
			}

		sSkip_WS(iChanRU);

		theCPQ = sQRead(iChanRU);
		if (not theCPQ)
			break;

		if (*theCPQ == '/')
			{
			sSkip_WS(iChanRU);

			string tagName;
			if (not sReadIdentifier(iChanRU, tagName))
				sThrow_ParseException("Failed to read identifier while parsing an ML end tag");

			sSkip_WS(iChanRU);

			if (not sTryRead_CP('>', iChanRU))
				sThrow_ParseException("Failed to read '>' while parsing an ML end tag");

			Pull_ML::sPush_TagEnd(tagName, iChanW);
			continue;
			}

		if (*theCPQ == '?')
			{
			if (not sSkip_Until(iChanRU, "?>"))
				sThrow_ParseException("Failed to read '?>' while parsing a PI tag");
			continue;
			}

		if (*theCPQ == '!')
			{
			if (sTryRead_CP('-', iChanRU))
				{
				if (sTryRead_CP('-', iChanRU))
					{
					// A comment. Could copy this to a comment channer or something.
					if (not sSkip_Until(iChanRU, "-->"))
						sThrow_ParseException("Failed to read '-->' while parsing a comment tag");
					}
				else
					{
					// Not a proper comment -- just skip till we hit a '>'
					if (not sSkip_Until(iChanRU, ">"))
						sThrow_ParseException("Failed to read '>' while parsing a '!-' tag");
					}
				}
			else if (sRead_String("[CDATA[", iChanRU))
				{
				// CDATA
				// Can we emit the body as a binary channer?
				if (not sSkip_Until(iChanRU, "]]>"))
					sThrow_ParseException("Failed to read ']]>' while parsing a CDATA tag");
				}
			else
				{
				// An entity definition
				if (not sSkip_Until(iChanRU, ">"))
					sThrow_ParseException("Failed to read '>' while parsing an entity definition tag");
				}
			continue;
			}
		sUnread(iChanRU, *theCPQ);

		// We've read a '<' and it's not an end tag, nor a PI, comment, CDATA or entity definition.

		string tagName;
		if (not sReadIdentifier(iChanRU, tagName))
			sThrow_ParseException("Failed to read identifier while parsing an ML begin/empty tag");

		// Read any attributes
		Attrs_t attributes;
		for (;;)
			{
			sSkip_WS(iChanRU);

			string attributeName;
			attributeName.reserve(8);
			if (not sReadAttributeName(iChanRU, attributeName))
				break;

			sSkip_WS(iChanRU);

			if (not sTryRead_CP('=', iChanRU))
				{
				attributes.push_back({attributeName, string()});
				}
			else
				{
				sSkip_WS(iChanRU);
				string attributeValue;
				attributeValue.reserve(8);
				if (not sReadAttributeValue(iChanRU,
					iRecognizeEntitiesInAttributeValues, iCallable_Entity,
					attributeValue))
					{
					sThrow_ParseException("Failed to read attibute value while parsing a begin/empty tag");
					}
				attributes.push_back({attributeName, attributeValue});
				}
			}

		sSkip_WS(iChanRU);

		if (sTryRead_CP('/', iChanRU))
			{
			if (not sTryRead_CP('>', iChanRU))
				sThrow_ParseException("Failed to read '>' while parsing an empty tag");
			Pull_ML::sPush_TagEmpty(tagName, attributes, iChanW);
			continue;
			}
		else
			{
			if (not sTryRead_CP('>', iChanRU))
				sThrow_ParseException("Failed to read '>' while parsing a begin tag");
			Pull_ML::sPush_TagBegin(tagName, attributes, iChanW);
			continue;
			}
		}
	return readAny;
	}

} // namespace ZooLib
