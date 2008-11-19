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

#include "zoolib/ZYad_ZooLib.h"

using std::string;
using std::vector;

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
#pragma mark * ZYadPrimR_TValue

ZYadPrimR_TValue::ZYadPrimR_TValue(const ZTValue& iTV)
:	fTValue(iTV)
	{}

ZRef<ZYad> ZYadPrimR_TValue::ReadYad()
	{ return new ZYad_TValue(fTValue); }

bool ZYadPrimR_TValue::IsSimple(const ZYadOptions& iOptions)
	{ return sIsSimple(iOptions, fTValue); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawRPos_MemoryBlock

ZYadRawRPos_MemoryBlock::ZYadRawRPos_MemoryBlock(const ZMemoryBlock& iMB)
:	ZStreamerRPos_MemoryBlock(iMB),
	fMB(iMB)
	{}

ZRef<ZYad> ZYadRawRPos_MemoryBlock::ReadYad()
	{ return new ZYad_TValue(fMB); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_Vector

ZYadListRPos_Vector::ZYadListRPos_Vector(const std::vector<ZTValue>& iVector)
:	fVector(iVector),
	fIter(fVector.begin())
	{}

bool ZYadListRPos_Vector::HasChild()
	{ return fIter != fVector.end(); }

ZRef<ZYad> ZYadListRPos_Vector::ReadYad()
	{
	if (fIter != fVector.end())
		return new ZYad_TValue(*fIter++);
	return ZRef<ZYad>();
	}

ZRef<ZYadR> ZYadListRPos_Vector::NextChild()
	{
	if (fIter != fVector.end())
		return ZYadUtil_ZooLib::sMakeYadR(*fIter++);
	return ZRef<ZYadR>();
	}

size_t ZYadListRPos_Vector::GetPosition()
	{ return fIter - fVector.begin(); }

size_t ZYadListRPos_Vector::GetSize()
	{ return fVector.size(); }

void ZYadListRPos_Vector::SetPosition(size_t iPosition)
	{ fIter = fVector.begin() + iPosition; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapRPos_Tuple

ZYadListMapRPos_Tuple::ZYadListMapRPos_Tuple(const ZTuple& iTuple)
:	fTuple(iTuple),
	fIter(fTuple.begin())
	{}

bool ZYadListMapRPos_Tuple::HasChild()
	{ return fIter != fTuple.end(); }

ZRef<ZYad> ZYadListMapRPos_Tuple::ReadYad()
	{
	if (fIter != fTuple.end())
		return new ZYad_TValue(fTuple.GetValue(fIter++));
	return ZRef<ZYad>();
	}

ZRef<ZYadR> ZYadListMapRPos_Tuple::NextChild()
	{
	if (fIter != fTuple.end())
		return ZYadUtil_ZooLib::sMakeYadR(fTuple.GetValue(fIter++));
	return ZRef<ZYadR>();
	}

size_t ZYadListMapRPos_Tuple::GetPosition()
	{ return fIter - fTuple.begin(); }

size_t ZYadListMapRPos_Tuple::GetSize()
	{ return fTuple.Count(); }

void ZYadListMapRPos_Tuple::SetPosition(size_t iPosition)
	{ fIter = fTuple.begin() + iPosition; }

std::string ZYadListMapRPos_Tuple::Name()
	{ return fTuple.NameOf(fIter).AsString(); }

void ZYadListMapRPos_Tuple::SetPosition(const std::string& iName)
	{ fIter = fTuple.IteratorOf(iName); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadUtil_ZooLib

ZRef<ZYadR> ZYadUtil_ZooLib::sMakeYadR(const ZTValue& iTV)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Vector: return new ZYadListRPos_Vector(iTV.GetVector());
		case eZType_Tuple: return new ZYadListMapRPos_Tuple(iTV.GetTuple());
		case eZType_Raw: return new ZYadRawRPos_MemoryBlock(iTV.GetRaw());
		}

	return new ZYadPrimR_TValue(iTV);
	}
