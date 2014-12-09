/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/Compare.h"

#include "zoolib/ValPred/ZExpr_Bool_ValPred.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZExpr_Bool_ValPred

ZExpr_Bool_ValPred::ZExpr_Bool_ValPred(const ZValPred& iValPred)
:	fValPred(iValPred)
	{}

ZExpr_Bool_ValPred::~ZExpr_Bool_ValPred()
	{}

void ZExpr_Bool_ValPred::Accept(const ZVisitor& iVisitor)
	{
	if (ZVisitor_Expr_Bool_ValPred* theVisitor = sDynNonConst<ZVisitor_Expr_Bool_ValPred>(&iVisitor))
		this->Accept_Expr_Bool_ValPred(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void ZExpr_Bool_ValPred::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Bool>& iVisitor)
	{
	if (ZVisitor_Expr_Bool_ValPred* theVisitor = sDynNonConst<ZVisitor_Expr_Bool_ValPred>(&iVisitor))
		this->Accept_Expr_Bool_ValPred(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZRef<ZExpr_Bool> ZExpr_Bool_ValPred::Self()
	{ return this; }

ZRef<ZExpr_Bool> ZExpr_Bool_ValPred::Clone()
	{ return this; }

void ZExpr_Bool_ValPred::Accept_Expr_Bool_ValPred(ZVisitor_Expr_Bool_ValPred& iVisitor)
	{ iVisitor.Visit_Expr_Bool_ValPred(this); }

const ZValPred& ZExpr_Bool_ValPred::GetValPred() const
	{ return fValPred; }

template <>
int sCompare_T(const ZExpr_Bool_ValPred& iL, const ZExpr_Bool_ValPred& iR)
	{ return sCompare_T(iL.GetValPred(), iR.GetValPred()); }

ZMACRO_CompareRegistration_T(ZExpr_Bool_ValPred)

// =================================================================================================
// MARK: - ZVisitor_Expr_Bool_ValPred

void ZVisitor_Expr_Bool_ValPred::Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred >& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
// MARK: - Operators

ZRef<ZExpr_Bool> sExpr_Bool(const ZValPred& iValPred)
	{ return new ZExpr_Bool_ValPred(iValPred); }

ZRef<ZExpr_Bool> operator~(const ZValPred& iValPred)
	{ return new ZExpr_Bool_Not(sExpr_Bool(iValPred)); }

ZRef<ZExpr_Bool> operator&(bool iBool, const ZValPred& iValPred)
	{
	if (iBool)
		return sExpr_Bool(iValPred);
	return sFalse();
	}

ZRef<ZExpr_Bool> operator&(const ZValPred& iValPred, bool iBool)
	{
	if (iBool)
		return sExpr_Bool(iValPred);
	return sFalse();
	}

ZRef<ZExpr_Bool> operator|(bool iBool, const ZValPred& iValPred)
	{
	if (iBool)
		return sTrue();
	return sExpr_Bool(iValPred);
	}

ZRef<ZExpr_Bool> operator|(const ZValPred& iValPred, bool iBool)
	{
	if (iBool)
		return sTrue();
	return sExpr_Bool(iValPred);
	}

ZRef<ZExpr_Bool> operator&(const ZValPred& iLHS, const ZValPred& iRHS)
	{ return new ZExpr_Bool_And(sExpr_Bool(iLHS), sExpr_Bool(iRHS)); }

ZRef<ZExpr_Bool> operator&(const ZValPred& iLHS, const ZRef<ZExpr_Bool>& iRHS)
	{ return new ZExpr_Bool_And(sExpr_Bool(iLHS), iRHS); }

ZRef<ZExpr_Bool> operator&(const ZRef<ZExpr_Bool>& iLHS, const ZValPred& iRHS)
	{ return new ZExpr_Bool_And(iLHS, sExpr_Bool(iRHS)); }

ZRef<ZExpr_Bool>& operator&=(ZRef<ZExpr_Bool>& ioLHS, const ZValPred& iRHS)
	{ return ioLHS = ioLHS & iRHS; }

ZRef<ZExpr_Bool> operator|(const ZValPred& iLHS, const ZValPred& iRHS)
	{ return new ZExpr_Bool_Or(sExpr_Bool(iLHS), sExpr_Bool(iRHS)); }

ZRef<ZExpr_Bool> operator|(const ZValPred& iLHS, const ZRef<ZExpr_Bool>& iRHS)
	{ return new ZExpr_Bool_Or(sExpr_Bool(iLHS), iRHS); }

ZRef<ZExpr_Bool> operator|(const ZRef<ZExpr_Bool>& iLHS, const ZValPred& iRHS)
	{ return new ZExpr_Bool_Or(iLHS, sExpr_Bool(iRHS)); }

ZRef<ZExpr_Bool>& operator|=(ZRef<ZExpr_Bool>& ioLHS, const ZValPred& iRHS)
	{ return ioLHS = ioLHS | iRHS; }

} // namespace ZooLib
