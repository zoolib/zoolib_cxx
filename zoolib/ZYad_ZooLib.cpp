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

using std::string;
using std::vector;

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
	fIter(fVector.begin())
	{}
	

ZYadListRPos_Vector::ZYadListRPos_Vector(const std::vector<ZTValue>& iVector)
:	ZYadR_TValue(iVector),
	fVector(this->GetTValue().GetVector()),
	fIter(fVector.begin())
	{}

bool ZYadListRPos_Vector::HasChild()
	{ return fIter != fVector.end(); }

ZRef<ZYadR> ZYadListRPos_Vector::NextChild()
	{
	if (fIter != fVector.end())
		return ZYadUtil_ZooLib::sMakeYadR(*fIter++);
	return ZRef<ZYadR>();
	}

bool ZYadListRPos_Vector::IsSimple(const ZYadOptions& iOptions)
	{ return ZYadR_TValue::IsSimple(iOptions); }

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
:	ZYadR_TValue(iTuple),
	fTuple(this->GetTValue().GetTuple()),
	fIter(fTuple.begin())
	{}

bool ZYadListMapRPos_Tuple::HasChild()
	{ return fIter != fTuple.end(); }

ZRef<ZYadR> ZYadListMapRPos_Tuple::NextChild()
	{
	if (fIter != fTuple.end())
		return ZYadUtil_ZooLib::sMakeYadR(fTuple.GetValue(fIter++));
	return ZRef<ZYadR>();
	}

bool ZYadListMapRPos_Tuple::IsSimple(const ZYadOptions& iOptions)
	{ return ZYadR_TValue::IsSimple(iOptions); }

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

ZTValue ZYadUtil_ZooLib::sFromYadR(ZRef<ZYadR> iYadR)
	{
	if (!iYadR)
		return ZTValue();

	if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
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
