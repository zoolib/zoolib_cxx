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

#include "zoolib/ZYad.h"
#include "zoolib/ZMemoryBlock.h"

using std::min;
using std::string;
using std::vector;

/*
YAD is Yet Another Data. It provides a suite of facilities for accessing data that looks like
ZooLib ZTuple suite -- CFDictionary, NSDictionary, PList, XMLRPC, JSON, Javascript types etc.

The idea is that there are a Map-like and List-like entities in many APIs, and that abstracting
access to them allows code to be applied to any of them.
*/

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
#pragma mark * ZYad

ZYad::ZYad()
	{}

ZTValue ZYad::GetTValue()
	{
	ZTValue result;
	this->GetTValue(result);
	return result;
	}

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
#pragma mark * ZYadPrimR

bool ZYadPrimR::HasChild()
	{ return false; }

ZRef<ZYadR> ZYadPrimR::NextChild()
	{ return ZRef<ZYadR>(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawR

ZRef<ZYad> ZYadRawR::ReadYad()
	{
	ZMemoryBlock theMB;
	ZStreamRWPos_MemoryBlock(theMB).CopyAllFrom(this->GetStreamR());
	return new ZYad_TValue(theMB);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR

ZRef<ZYad> ZYadListR::ReadYad()
	{
	vector<ZTValue> theVector;
	while (this->HasChild())
		{
		if (ZRef<ZYadR> theChild = this->NextChild())
			{
			if (ZRef<ZYad> theYad = theChild->ReadYad())
				theVector.push_back(theYad->GetTValue());
			}
		}			
	return new ZYad_TValue(theVector);	
	}
	
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
#pragma mark * ZYadMapR

ZRef<ZYad> ZYadMapR::ReadYad()
	{
	ZTuple theTuple;
	while (this->HasChild())
		{
		string theName = this->Name();
		if (ZRef<ZYadR> theChild = this->NextChild())
			{
			if (ZRef<ZYad> theYad = theChild->ReadYad())
				theTuple.SetValue(theName, theYad->GetTValue());
			}
		}
	return new ZYad_TValue(theTuple);
	}

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
#pragma mark * ZYadListMapR

ZRef<ZYad> ZYadListMapR::ReadYad()
	{
	// Assume our list-ish implementation is more efficient
	return ZYadListR::ReadYad();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapRPos

bool ZYadListMapRPos::IsSimple(const ZYadOptions& iOptions)
	{
	// Assume our list-ish implementation is more efficient
	return ZYadListRPos::ReadYad();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_TValue

ZYad_TValue::ZYad_TValue(const ZTValue& iTV)
:	fTV(iTV)
	{}

ZYad_TValue::ZYad_TValue(ZType iType, const ZStreamR& iStreamR)
:	fTV(iType, iStreamR)
	{}

ZYad_TValue::~ZYad_TValue()
	{}

bool ZYad_TValue::GetTValue(ZTValue& oTV)
	{
	oTV = fTV;
	return true;
	}

// =================================================================================================
static void sTest(const vector<ZTValue>& iVector, const ZTuple& iTuple, ZRef<ZYadR> iYadR)
	{
	ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR);
	ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR);
	ZRef<ZYadPrimR> theYadPrimR = ZRefDynamicCast<ZYadPrimR>(iYadR);

	ZRef<ZYadListMapR> theYadListMapR = ZRefDynamicCast<ZYadListMapR>(iYadR);
	ZRef<ZYadListRPos> theYadListRPos = ZRefDynamicCast<ZYadListRPos>(iYadR);
	ZRef<ZYadMapRPos> theYadMapRPos = ZRefDynamicCast<ZYadMapRPos>(iYadR);
	ZRef<ZYadRawR> theYadRawR = ZRefDynamicCast<ZYadRawR>(iYadR);

	ZRef<ZYadListMapRPos> theYadListMapRPos = ZRefDynamicCast<ZYadListMapRPos>(iYadR);
	}