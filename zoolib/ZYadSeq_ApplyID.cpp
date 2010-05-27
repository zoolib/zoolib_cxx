/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYadSeq_ApplyID.h"

namespace ZooLib {
// FIXME. Could return a ZYadMapR with a constant for the ID, and the actual yad for the value.
// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqR_Expr_Logic

ZYadSeqR_ApplyID::ZYadSeqR_ApplyID(
	ZRef<ZYadSeqR> iYadSeqR, const std::string& iIDName, const std::string& iValName)
:	fYadSeqR(iYadSeqR)
,	fIDName(iIDName)
,	fValName(iValName)
,	fNextID(1)
	{}

ZYadSeqR_ApplyID::~ZYadSeqR_ApplyID()
	{}

ZRef<ZYadR> ZYadSeqR_ApplyID::ReadInc()
	{
	if (ZRef<ZYadR> theYadR = fYadSeqR->ReadInc())
		{
		ZMap_Any theMap;
		theMap.Set(fIDName, ++fNextID);
		theMap.Set(fValName, sFromYadR(ZVal_Any(), theYadR));
		return sMakeYadR(theMap);
		}

	return ZRef<ZYadR>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqRPos_ApplyID

ZYadSeqRPos_ApplyID::ZYadSeqRPos_ApplyID(
	ZRef<ZYadSeqRPos> iYadSeqRPos, const std::string& iIDName, const std::string& iValName)
:	fYadSeqRPos(iYadSeqRPos)
,	fIDName(iIDName)
,	fValName(iValName)
	{}

ZYadSeqRPos_ApplyID::~ZYadSeqRPos_ApplyID()
	{}

ZRef<ZYadR> ZYadSeqRPos_ApplyID::ReadInc()
	{
	const uint64 thePosition = fYadSeqRPos->GetPosition();
	if (ZRef<ZYadR> theYadR = fYadSeqRPos->ReadInc())
		{
		ZMap_Any theMap;
		theMap.Set(fIDName, thePosition);
		theMap.Set(fValName, sFromYadR(ZVal_Any(), theYadR));
		return sMakeYadR(theMap);
		}

	return ZRef<ZYadR>();
	}

//##ZRef<ZYadSeqRPos> ZYadSeqRPos_ApplyID::Clone()
//##	{ return new ZYadSeqRPos_ApplyID(fYadSeqRPos->Clone(), fIDName, fValName); }

uint64 ZYadSeqRPos_ApplyID::GetPosition()
	{ return fYadSeqRPos->GetPosition(); }

void ZYadSeqRPos_ApplyID::SetPosition(uint64 iPosition)
	{ fYadSeqRPos->SetPosition(iPosition); }

uint64 ZYadSeqRPos_ApplyID::GetSize()
	{ return fYadSeqRPos->GetSize(); }

} // namespace ZooLib
