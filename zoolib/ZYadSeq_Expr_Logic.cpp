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

#include "zoolib/ZExpr_Logic_ValCondition.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYadSeq_Expr_Logic.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqR_Expr_Logic

ZYadSeqR_Expr_Logic::ZYadSeqR_Expr_Logic(
	ZRef<ZYadSeqR> iYadSeqR, ZRef<ZExpr_Logic> iExpr_Logic)
:	fYadSeqR(iYadSeqR)
,	fExpr_Logic(iExpr_Logic)
	{}

ZYadSeqR_Expr_Logic::~ZYadSeqR_Expr_Logic()
	{}

ZRef<ZYadR> ZYadSeqR_Expr_Logic::ReadInc()
	{
	for (;;)
		{
		if (ZRef<ZYadR> theYadR = fYadSeqR->ReadInc())
			{
			const ZVal_Any theVal = sFromYadR(ZVal_Any(), theYadR);
			if (sMatches(fExpr_Logic, theVal))
				return sMakeYadR(theVal);
			}
		else
			{
			return ZRef<ZYadR>();
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqRPos_Expr_Logic

ZYadSeqRPos_Expr_Logic::ZYadSeqRPos_Expr_Logic(
	ZRef<ZYadSeqRPos> iYadSeqRPos, ZRef<ZExpr_Logic> iExpr_Logic)
:	fYadSeqRPos(iYadSeqRPos)
,	fExpr_Logic(iExpr_Logic)
	{}

ZYadSeqRPos_Expr_Logic::~ZYadSeqRPos_Expr_Logic()
	{}

ZRef<ZYadR> ZYadSeqRPos_Expr_Logic::ReadInc()
	{
	for (;;)
		{
		if (ZRef<ZYadR> theYadR = fYadSeqRPos->ReadInc())
			{
			const ZVal_Any theVal = sFromYadR(ZVal_Any(), theYadR);
			if (sMatches(fExpr_Logic, theVal))
				return sMakeYadR(theVal);
			}
		else
			{
			return ZRef<ZYadR>();
			}
		}
	}

ZRef<ZYadSeqRPos> ZYadSeqRPos_Expr_Logic::Clone()
	{ return new ZYadSeqRPos_Expr_Logic(fYadSeqRPos->Clone(), fExpr_Logic); }

uint64 ZYadSeqRPos_Expr_Logic::GetPosition()
	{ return fYadSeqRPos->GetPosition(); }

void ZYadSeqRPos_Expr_Logic::SetPosition(uint64 iPosition)
	{ fYadSeqRPos->SetPosition(iPosition); }

uint64 ZYadSeqRPos_Expr_Logic::GetSize()
	{ return fYadSeqRPos->GetSize(); }

NAMESPACE_ZOOLIB_END
