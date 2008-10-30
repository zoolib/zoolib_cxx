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

#include "zoolib/ZYAD_ZooLib.h"
#include "zoolib/ZMemoryBlock.h"

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD_ZooLib

ZYAD_ZooLib::ZYAD_ZooLib(const ZTValue& iTV)
:	fTV(iTV)
	{}

ZYAD_ZooLib::~ZYAD_ZooLib()
	{}

bool ZYAD_ZooLib::GetTValue(ZTValue& oTV)
	{
	oTV = fTV;
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep_ZooLib definition

ZYADReaderRep_ZooLib::ZYADReaderRep_ZooLib(const ZTValue& iTV)
:	fTV(iTV)
	{}

bool ZYADReaderRep_ZooLib::HasValue()
	{ return true; }

ZType ZYADReaderRep_ZooLib::Type()
	{ return fTV.TypeOf(); }

ZRef<ZMapReaderRep> ZYADReaderRep_ZooLib::ReadMap()
	{
	if (fTV.TypeOf() == eZType_Tuple)
		return new ZMapReaderRep_ZooLib(fTV.GetTuple());
	return ZRef<ZMapReaderRep>();
	}

ZRef<ZListReaderRep> ZYADReaderRep_ZooLib::ReadList()
	{
	if (fTV.TypeOf() == eZType_Vector)
		return new ZListReaderRep_ZooLib(fTV.GetVector());
	return ZRef<ZListReaderRep>();
	}

ZRef<ZStreamerR> ZYADReaderRep_ZooLib::ReadRaw()
	{
	if (fTV.TypeOf() == eZType_Raw)
		return new ZStreamerRPos_T<ZStreamRPos_MemoryBlock>(fTV.GetRaw());
	return ZRef<ZStreamerR>();
	}

ZRef<ZYAD> ZYADReaderRep_ZooLib::ReadYAD()
	{
	return new ZYAD_ZooLib(fTV);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep_ZooLib definition

ZMapReaderRep_ZooLib::ZMapReaderRep_ZooLib(const ZTuple& iTuple)
:	fTuple(iTuple),
	fIter(fTuple.begin())
	{}

bool ZMapReaderRep_ZooLib::HasValue()
	{ return fIter != fTuple.end(); }

string ZMapReaderRep_ZooLib::Name()
	{
	if (fIter != fTuple.end())
		return fTuple.NameOf(fIter).AsString();
	return string();
	}

ZRef<ZYADReaderRep> ZMapReaderRep_ZooLib::Read()
	{
	if (fIter != fTuple.end())
		return new ZYADReaderRep_ZooLib(fTuple.GetValue(fIter++));

	return ZRef<ZYADReaderRep>();
	}

void ZMapReaderRep_ZooLib::Skip()
	{
	if (fIter != fTuple.end())
		++fIter;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep_ZooLib definition

ZListReaderRep_ZooLib::ZListReaderRep_ZooLib(const vector<ZTValue>& iVector)
:	fVector(iVector),
	fIter(fVector.begin())
	{}

bool ZListReaderRep_ZooLib::HasValue()
	{ return fIter != fVector.end(); }

size_t ZListReaderRep_ZooLib::Index() const
	{ return fIter - fVector.begin(); }

ZRef<ZYADReaderRep> ZListReaderRep_ZooLib::Read()
	{
	if (fIter != fVector.end())
		return new ZYADReaderRep_ZooLib(*fIter++);

	return ZRef<ZYADReaderRep>();
	}

void ZListReaderRep_ZooLib::Skip()
	{
	if (fIter != fVector.end())
		++fIter;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Test code

ZTValue sReadValue(ZYADReader iYADReader);

static ZTuple sReadTuple(ZMapReader iMapReader)
	{
	ZTuple result;
	while (iMapReader)
		{
		string theName = iMapReader.Name();
		ZTValue theValue = ZYADUtil_ZooLib::sFromReader(iMapReader.Read());
		result.SetValue(theName, theValue);
		}
	return result;
	}

static ZTValue sReadVector(ZListReader iListReader)
	{
	ZTValue result;
	vector<ZTValue>& theVector = result.SetMutableVector();
	while (iListReader)
		theVector.push_back(ZYADUtil_ZooLib::sFromReader(iListReader.Read()));
	return result;
	}

bool ZYADUtil_ZooLib::sFromReader(ZYADReader iYADReader, ZTValue& oTV)
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
		ZRef<ZYAD> theYAD = iYADReader.ReadYAD();
		oTV = theYAD->GetTValue();
		}
	return true;
	}

ZTValue ZYADUtil_ZooLib::sFromReader(ZYADReader iYADReader)
	{
	ZTValue theTV;
	if (sFromReader(iYADReader, theTV))
		return theTV;
	return ZTValue();
	}
