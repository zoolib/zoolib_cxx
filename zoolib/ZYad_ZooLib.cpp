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
#include "zoolib/ZYad_ZooLib.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;
using std::vector;

ZOOLIB_FACTORYCHAIN_HEAD(ZTValue, ZRef<ZYadR>);

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
#pragma mark * ZYadR_TValue

ZYadR_TValue::ZYadR_TValue(ZType iType, const ZStreamR& iStreamR)
:	fTValue(iType, iStreamR)
	{}

ZYadR_TValue::ZYadR_TValue(const ZTValue& iTV)
:	fTValue(iTV)
	{}

const ZTValue& ZYadR_TValue::GetTValue()
	{ return fTValue; }

bool ZYadR_TValue::IsSimple(const ZYadOptions& iOptions)
	{ return sIsSimple(iOptions, fTValue); }

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
#pragma mark * ZYadRawRPos_MemoryBlock

ZYadRawRPos_MemoryBlock::ZYadRawRPos_MemoryBlock(const ZMemoryBlock& iMB)
:	ZYadR_TValue(iMB),
	ZStreamerRPos_MemoryBlock(iMB)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_Vector

ZYadListRPos_Vector::ZYadListRPos_Vector(const ZTValue& iTV)
:	ZYadR_TValue(iTV),
	fVector(this->GetTValue().GetVector()),
	fPosition(0)
	{}

ZYadListRPos_Vector::ZYadListRPos_Vector(const std::vector<ZTValue>& iVector)
:	ZYadR_TValue(iVector),
	fVector(this->GetTValue().GetVector()),
	fPosition(0)
	{}

ZYadListRPos_Vector::ZYadListRPos_Vector(const std::vector<ZTValue>& iVector, uint64 iPosition)
:	ZYadR_TValue(iVector),
	fVector(this->GetTValue().GetVector()),
	fPosition(iPosition)
	{}

bool ZYadListRPos_Vector::HasChild()
	{ return fPosition < fVector.size(); }

ZRef<ZYadR> ZYadListRPos_Vector::NextChild()
	{
	if (fPosition < fVector.size())
		return ZYad_ZooLib::sMakeYadR(fVector[fPosition++]);
	return ZRef<ZYadR>();
	}

bool ZYadListRPos_Vector::IsSimple(const ZYadOptions& iOptions)
	{ return ZYadR_TValue::IsSimple(iOptions); }

uint64 ZYadListRPos_Vector::GetPosition()
	{ return fPosition; }

uint64 ZYadListRPos_Vector::GetSize()
	{ return fVector.size(); }

void ZYadListRPos_Vector::SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

ZRef<ZYadListRPos> ZYadListRPos_Vector::Clone()
	{ return new ZYadListRPos_Vector(fVector, fPosition); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_Tuple

ZYadMapRPos_Tuple::ZYadMapRPos_Tuple(const ZTuple& iTuple)
:	ZYadR_TValue(iTuple),
	fTuple(this->GetTValue().GetTuple()),
	fIter(fTuple.begin())
	{}

ZYadMapRPos_Tuple::ZYadMapRPos_Tuple(const ZTuple& iTuple, const ZTuple::const_iterator& iIter)
:	ZYadR_TValue(iTuple),
	fTuple(this->GetTValue().GetTuple()),
	fIter(iIter)
	{}

bool ZYadMapRPos_Tuple::HasChild()
	{ return fIter < fTuple.end(); }

ZRef<ZYadR> ZYadMapRPos_Tuple::NextChild()
	{
	if (fIter < fTuple.end())
		return ZYad_ZooLib::sMakeYadR(fTuple.GetValue(fIter++));
	return ZRef<ZYadR>();
	}

bool ZYadMapRPos_Tuple::IsSimple(const ZYadOptions& iOptions)
	{ return ZYadR_TValue::IsSimple(iOptions); }

std::string ZYadMapRPos_Tuple::Name()
	{
	if (fIter < fTuple.end())
		return fTuple.NameOf(fIter).AsString();
	return string();
	}

void ZYadMapRPos_Tuple::SetPosition(const std::string& iName)
	{ fIter = fTuple.IteratorOf(iName); }

ZRef<ZYadMapRPos> ZYadMapRPos_Tuple::Clone()
	{ return new ZYadMapRPos_Tuple(fTuple, fIter); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_ZooLib

ZRef<ZYadR> ZYad_ZooLib::sMakeYadR(const ZTValue& iTV)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Vector: return new ZYadListRPos_Vector(iTV.GetVector());
		case eZType_Tuple: return new ZYadMapRPos_Tuple(iTV.GetTuple());
		case eZType_Raw: return new ZYadRawRPos_MemoryBlock(iTV.GetRaw());
		}

	return new ZYadPrimR_TValue(iTV);
	}

ZTValue ZYad_ZooLib::sFromYadR(ZRef<ZYadR> iYadR)
	{
	if (!iYadR)
		{
		return ZTValue();
		}
	else if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		ZTuple theTuple;
		while (theYadMapR->HasChild())
			{
			string theName = theYadMapR->Name();
			if (ZRef<ZYadR> theChild = theYadMapR->NextChild())
				{
				ZTValue theTV = sFromYadR(theChild);
				theTuple.SetValue(theName, theTV);
				}
			}
		return theTuple;
		}
	else if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		vector<ZTValue> theVector;
		while (theYadListR->HasChild())
			{
			if (ZRef<ZYadR> theChild = theYadListR->NextChild())
				{
				ZTValue theTV = sFromYadR(theChild);
				theVector.push_back(theTV);
				}
			}			
		return theVector;	
		}
	else if (ZRef<ZYadRawR> theYadRawR = ZRefDynamicCast<ZYadRawR>(iYadR))
		{
		ZMemoryBlock theMB;
		ZStreamRWPos_MemoryBlock(theMB).CopyAllFrom(theYadRawR->GetStreamR());
		return theMB;
		}
	else
		{
		return ZFactoryChain_T<ZTValue, ZRef<ZYadR> >::sMake(iYadR);
		}
	}

NAMESPACE_ZOOLIB_END
