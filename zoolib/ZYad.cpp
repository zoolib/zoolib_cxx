/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#include "zoolib/ZFactoryChain.h"
#include "zoolib/ZYad.h"
#include "zoolib/ZMemoryBlock.h"

using std::min;
using std::string;
using std::vector;

ZOOLIB_FACTORYCHAIN_HEAD(ZTValue, ZRef<ZYadR>);

/*
YAD is Yet Another Data. It provides a suite of facilities for accessing data that looks like
ZooLib ZTuple suite -- CFDictionary, NSDictionary, PList, XMLRPC, JSON, Javascript types etc.

The idea is that there are a Map-like and List-like entities in many APIs, and that abstracting
access to them allows code to be applied to any of them.
*/

// =================================================================================================
#pragma mark -
#pragma mark * Factory

namespace ZANONYMOUS {

class Maker0
:	public ZFactoryChain_T<ZTValue, ZRef<ZYadR> >
	{
public:
	Maker0()
	:	ZFactoryChain_T<Result_t, Param_t>(true)
		{}

	virtual bool Make(Result_t& oResult, Param_t iParam)
		{
		if (ZRef<ZYadR_TValue> theYadR = ZRefDynamicCast<ZYadR_TValue>(iParam))
			{
			oResult = theYadR->GetTValue();
			return true;
			}
		return false;
		}	
	} sMaker0;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static bool sIsSimpleString(const ZYadOptions& iOptions, const string& iString)
	{
	if (iOptions.fStringLineLength && iString.size() > iOptions.fStringLineLength)
		{
		// We have a non-zero desired line length, and the string contains
		// more characters than that. This doesn't account for increases in width
		// due to escaping etc.
		return false;
		}

	if (iOptions.fBreakStrings)
		{
		// We've been asked to break strings at line ending characters,
		// which means (here and in ZStrimW_Escapify) LF and CR. Strictly
		// speaking we should use ZUnicode::sIsEOL().
		if (string::npos != iString.find_first_of("\n\r"))
			return false;
		}

	return true;
	}

static bool sIsSimple(const ZYadOptions& iOptions, const ZTValue& iTV)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Raw:
			{
			size_t theSize;
			iTV.GetRawAttributes(nil, &theSize);
			return theSize <= iOptions.fRawChunkSize;
			}
		case eZType_Vector:
			{
			const vector<ZTValue>& theVector = iTV.GetVector();
			if (theVector.empty())
				return true;

			if (theVector.size() == 1)
				return sIsSimple(iOptions, theVector.at(0));

			return false;
			}
		case eZType_Tuple:
			{
			const ZTuple& theTuple = iTV.GetTuple();
			if (theTuple.Empty())
				return true;

			if (theTuple.Count() == 1)
				return sIsSimple(iOptions, theTuple.GetValue(theTuple.begin()));

			return false;
			}
		case eZType_String:
			{
			return sIsSimpleString(iOptions, iTV.GetString());
			}
		default:
			{
			return true;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadOptions

ZYadOptions::ZYadOptions(bool iDoIndentation)
:	fRawChunkSize(16),
	fRawByteSeparator(" "),
	fRawAsASCII(iDoIndentation),
	fBreakStrings(true),
	fStringLineLength(80),
	fIDsHaveDecimalVersionComment(iDoIndentation),
	fTimesHaveUserLegibleComment(true)
	{
	if (iDoIndentation)
		{
		fEOLString = "\n";
		fIndentString = "  ";
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException

ZYadParseException::ZYadParseException(const string& iWhat)
:	runtime_error(iWhat)
	{}

ZYadParseException::ZYadParseException(const char* iWhat)
:	runtime_error(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR

ZYadR::ZYadR()
	{}

void ZYadR::Skip()
	{
	if (this->HasChild())
		this->NextChild();
	}

void ZYadR::SkipAll()
	{
	while (this->HasChild())
		this->NextChild();
	}

bool ZYadR::IsSimple(const ZYadOptions& iOptions)
	{ return false; }

	
// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_TValue

ZYadR_TValue::ZYadR_TValue(ZType iType, const ZStreamR& iStreamR)
:	fTValue(iType, iStreamR)
	{}

ZYadR_TValue::ZYadR_TValue(const ZTValue& iTV)
:	fTValue(iTV)
	{}

ZTValue ZYadR_TValue::GetTValue()
	{ return fTValue; }

bool ZYadR_TValue::IsSimple(const ZYadOptions& iOptions)
	{ return sIsSimple(iOptions, fTValue); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR

bool ZYadPrimR::HasChild()
	{ return false; }

ZRef<ZYadR> ZYadPrimR::NextChild()
	{ return ZRef<ZYadR>(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_TValue

ZYadPrimR_TValue::ZYadPrimR_TValue(ZType iType, const ZStreamR& iStreamR)
:	ZYadR_TValue(iType, iStreamR)
	{}

ZYadPrimR_TValue::ZYadPrimR_TValue(const ZTValue& iTV)
:	ZYadR_TValue(iTV)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos

bool ZYadListRPos::IsSimple(const ZYadOptions& iOptions)
	{
	size_t theSize = this->GetSize();
	if (theSize == 0)
		return true;

	size_t thePosition = this->GetPosition();
	if (thePosition == theSize - 1)
		{
		ZRef<ZYadR> theYadR = this->NextChild();
		this->SetPosition(thePosition);
		return theYadR->IsSimple(iOptions);
		}

	return false;
	}

void ZYadListRPos::Skip()
	{
	size_t theSize = this->GetSize();
	size_t thePosition = this->GetPosition();
	this->SetPosition(min(theSize, thePosition + 1));
	}

void ZYadListRPos::SkipAll()
	{ this->SetPosition(this->GetSize()); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos

bool ZYadMapRPos::IsSimple(const ZYadOptions& iOptions)
	{
	if (!this->HasChild())
		return true;

	string thePosition = this->Name();
	ZRef<ZYadR> theYadR = this->NextChild();
	this->SetPosition(thePosition);

	if (!this->HasChild())
		{
		// We've exhausted ouselves, so we had just one entry.
		return theYadR->IsSimple(iOptions);
		}

	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapRPos

bool ZYadListMapRPos::IsSimple(const ZYadOptions& iOptions)
	{
	// Assume our list-ish implementation is more efficient
	return ZYadListRPos::IsSimple(iOptions);
	}

// =================================================================================================
static void sTest(const vector<ZTValue>& iVector, const ZTuple& iTuple, ZRef<ZYadR> iYadR)
	{
	ZFactoryChain_T<ZTValue, ZRef<ZYadR> >::sMake(ZRef<ZYadR>());

	ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR);
	ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR);
	ZRef<ZYadPrimR> theYadPrimR = ZRefDynamicCast<ZYadPrimR>(iYadR);

	ZRef<ZYadListMapR> theYadListMapR = ZRefDynamicCast<ZYadListMapR>(iYadR);
	ZRef<ZYadListRPos> theYadListRPos = ZRefDynamicCast<ZYadListRPos>(iYadR);
	ZRef<ZYadMapRPos> theYadMapRPos = ZRefDynamicCast<ZYadMapRPos>(iYadR);
	ZRef<ZYadRawR> theYadRawR = ZRefDynamicCast<ZYadRawR>(iYadR);

	ZRef<ZYadListMapRPos> theYadListMapRPos = ZRefDynamicCast<ZYadListMapRPos>(iYadR);
	}
