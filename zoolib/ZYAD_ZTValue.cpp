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

#include "zoolib/ZYAD_ZTValue.h"
#include "zoolib/ZMemoryBlock.h"

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static bool sIsComplexString(const ZYADOptions& iOptions, const string& iString)
	{
	if (iOptions.fStringLineLength && iString.size() > iOptions.fStringLineLength)
		{
		// We have a non-zero desired line length, and the string contains
		// more characters than that. This doesn't account for increases in width
		// due to escaping etc.
		return true;
		}

	if (iOptions.fBreakStrings)
		{
		// We've been asked to break strings at line ending characters,
		// which means (here and in ZStrimW_Escapify) LF and CR. Strictly
		// speaking we should use ZUnicode::sIsEOL().
		if (string::npos != iString.find_first_of("\n\r"))
			return true;
		}

	return false;
	}

static bool sIsComplex(const ZYADOptions& iOptions, const ZTValue& iTV)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Raw:
			{
			size_t theSize;
			iTV.GetRawAttributes(nil, &theSize);
			return theSize > iOptions.fRawChunkSize;
			}
		case eZType_Vector:
			{
			const vector<ZTValue>& theVector = iTV.GetVector();
			if (theVector.empty())
				return false;

			if (theVector.size() == 1)
				return sIsComplex(iOptions, theVector.at(0));

			return true;
			}
		case eZType_Tuple:
			{
			const ZTuple& theTuple = iTV.GetTuple();
			if (theTuple.Empty())
				return false;

			if (theTuple.Count() == 1)
				return sIsComplex(iOptions, theTuple.GetValue(theTuple.begin()));

			return true;
			}
		case eZType_String:
			{
			return sIsComplexString(iOptions, iTV.GetString());
			}
		default:
			{
			return false;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep_ZTValue definition

ZYADReaderRep_ZTValue::ZYADReaderRep_ZTValue(const ZTValue& iTV)
:	fTV(iTV),
	fHasValue(true)
	{}

bool ZYADReaderRep_ZTValue::HasValue()
	{ return fHasValue; }

ZType ZYADReaderRep_ZTValue::Type()
	{ return fTV.TypeOf(); }

ZRef<ZMapReaderRep> ZYADReaderRep_ZTValue::ReadMap()
	{
	if (fHasValue && fTV.TypeOf() == eZType_Tuple)
		{
		fHasValue = false;
		return new ZMapReaderRep_ZTuple(fTV.GetTuple());
		}
	return ZRef<ZMapReaderRep>();
	}

ZRef<ZListReaderRep> ZYADReaderRep_ZTValue::ReadList()
	{
	if (fHasValue && fTV.TypeOf() == eZType_Vector)
		{
		fHasValue = false;
		return new ZListReaderRep_ZVector(fTV.GetVector());
		}
	return ZRef<ZListReaderRep>();
	}

ZRef<ZStreamerR> ZYADReaderRep_ZTValue::ReadRaw()
	{
	if (fHasValue && fTV.TypeOf() == eZType_Raw)
		{
		fHasValue = false;
		return new ZStreamerRPos_T<ZStreamRPos_MemoryBlock>(fTV.GetRaw());
		}
	return ZRef<ZStreamerR>();
	}

ZRef<ZYAD> ZYADReaderRep_ZTValue::ReadYAD()
	{
	if (fHasValue)
		{
		fHasValue = false;
		return new ZYAD_ZTValue(fTV);
		}
	return ZRef<ZYAD>();
	}

void ZYADReaderRep_ZTValue::Skip()
	{ fHasValue = false; }

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep_ZTuple definition

ZMapReaderRep_ZTuple::ZMapReaderRep_ZTuple(const ZTuple& iTuple)
:	fTuple(iTuple),
	fIter(fTuple.begin())
	{}

bool ZMapReaderRep_ZTuple::HasValue()
	{ return fIter != fTuple.end(); }

string ZMapReaderRep_ZTuple::Name()
	{
	if (fIter != fTuple.end())
		return fTuple.NameOf(fIter).AsString();
	return string();
	}

ZRef<ZYADReaderRep> ZMapReaderRep_ZTuple::Read()
	{
	if (fIter != fTuple.end())
		return new ZYADReaderRep_ZTValue(fTuple.GetValue(fIter++));

	return ZRef<ZYADReaderRep>();
	}

void ZMapReaderRep_ZTuple::Skip()
	{
	if (fIter != fTuple.end())
		++fIter;
	}

bool ZMapReaderRep_ZTuple::IsSimple(const ZYADOptions& iOptions)
	{ return !sIsComplex(iOptions, fTuple); }

bool ZMapReaderRep_ZTuple::CanReadAtName()
	{ return true; }

ZRef<ZYADReaderRep> ZMapReaderRep_ZTuple::ReadAtName(const string& iName)
	{
	ZTuple::const_iterator i = fTuple.IteratorOf(iName);
	if (i != fTuple.end())
		return new ZYADReaderRep_ZTValue(fTuple.GetValue(i));

	return ZRef<ZYADReaderRep>();
	}

bool ZMapReaderRep_ZTuple::CanReadAtIndex()
	{ return true; }

size_t ZMapReaderRep_ZTuple::Count()
	{ return fTuple.Count(); }

ZRef<ZYADReaderRep> ZMapReaderRep_ZTuple::ReadAtIndex(size_t iIndex)
	{
	if (iIndex < fTuple.Count())
		return new ZYADReaderRep_ZTValue(fTuple.GetValue(fTuple.begin() + iIndex));

	return ZRef<ZYADReaderRep>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep_ZVector definition

ZListReaderRep_ZVector::ZListReaderRep_ZVector(const vector<ZTValue>& iVector)
:	fVector(iVector),
	fIter(fVector.begin())
	{}

bool ZListReaderRep_ZVector::HasValue()
	{ return fIter != fVector.end(); }

size_t ZListReaderRep_ZVector::Index()
	{ return fIter - fVector.begin(); }

ZRef<ZYADReaderRep> ZListReaderRep_ZVector::Read()
	{
	if (fIter != fVector.end())
		return new ZYADReaderRep_ZTValue(*fIter++);

	return ZRef<ZYADReaderRep>();
	}

void ZListReaderRep_ZVector::Skip()
	{
	if (fIter != fVector.end())
		++fIter;
	}

bool ZListReaderRep_ZVector::IsSimple(const ZYADOptions& iOptions)
	{ return !sIsComplex(iOptions, fVector); }

bool ZListReaderRep_ZVector::CanReadAtIndex()
	{ return true; }

size_t ZListReaderRep_ZVector::Count()
	{ return fVector.size(); }

ZRef<ZYADReaderRep> ZListReaderRep_ZVector::ReadAtIndex(size_t iIndex)
	{
	if (iIndex < fVector.size())
		return new ZYADReaderRep_ZTValue(fVector.at(iIndex));

	return ZRef<ZYADReaderRep>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYADUtil_ZTValue

static ZTuple sReadTuple(ZMapReader iMapReader)
	{
	ZTuple result;
	while (iMapReader)
		{
		string theName = iMapReader.Name();
		ZTValue theValue = ZYADUtil_ZTValue::sFromReader(iMapReader.Read());
		result.SetValue(theName, theValue);
		}
	return result;
	}

static ZTValue sReadVector(ZListReader iListReader)
	{
	ZTValue result;
	vector<ZTValue>& theVector = result.SetMutableVector();
	while (iListReader)
		theVector.push_back(ZYADUtil_ZTValue::sFromReader(iListReader.Read()));
	return result;
	}

bool ZYADUtil_ZTValue::sFromReader(ZYADReader iYADReader, ZTValue& oTV)
	{
	if (!iYADReader)
		return false;

	if (iYADReader.IsMap())
		{
		oTV = sReadTuple(iYADReader.ReadMap());
		}
	else if (iYADReader.IsList())
		{
		oTV = sReadVector(iYADReader.ReadList());
		}
	else if (iYADReader.IsRaw())
		{
		ZMemoryBlock theMB;
		ZStreamRWPos_MemoryBlock(theMB).CopyAllFrom(iYADReader.ReadRaw()->GetStreamR());
		oTV = theMB;
		}
	else
		{
		oTV = iYADReader.ReadYAD()->GetTValue();
		}
	return true;
	}

ZTValue ZYADUtil_ZTValue::sFromReader(ZYADReader iYADReader)
	{
	ZTValue theTV;
	if (sFromReader(iYADReader, theTV))
		return theTV;
	return ZTValue();
	}
