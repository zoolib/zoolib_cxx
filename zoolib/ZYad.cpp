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

#include <stdio.h> // For printf

NAMESPACE_ZOOLIB_BEGIN

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

void ZYadListRPos::SetPosition(size_t iPosition)
	{
	printf("ZYadListRPos::SetPosition\n");
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

void ZYadMapRPos::SetPosition(const string& iName)
	{
	printf("ZYadMapRPos::SetPosition\n");
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapRPos

bool ZYadListMapRPos::IsSimple(const ZYadOptions& iOptions)
	{
	// Assume our list-ish implementation is more efficient
	return ZYadListRPos::IsSimple(iOptions);
	}

void ZYadListMapRPos::SetPosition(size_t iPosition)
	{ ZYadListRPos::SetPosition(iPosition); }

void ZYadListMapRPos::SetPosition(const string& iName)
	{ ZYadMapRPos::SetPosition(iName); }

NAMESPACE_ZOOLIB_END
