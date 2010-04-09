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

#include "zoolib/ZExpr_Logic.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr

ZExpr_Logic::ZExpr_Logic()
	{}

ZExpr_Logic::~ZExpr_Logic()
	{}

void ZExpr_Logic::Accept_Expr(ZVisitor_Expr& iVisitor)
	{
	if (ZVisitor_Expr_Logic* theVisitor =
		dynamic_cast<ZVisitor_Expr_Logic*>(&iVisitor))
		{
		this->Accept_Expr_Logic(*theVisitor);
		}
	else
		{
		ZExpr::Accept_Expr(iVisitor);
		}
	}

void ZExpr_Logic::Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor)
	{ ZExpr::Accept_Expr(iVisitor); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_True

ZExpr_Logic_True::ZExpr_Logic_True()
	{}

void ZExpr_Logic_True::Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor)
	{ iVisitor.Visit_Logic_True(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_False

ZExpr_Logic_False::ZExpr_Logic_False()
	{}

void ZExpr_Logic_False::Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor)
	{ iVisitor.Visit_Logic_False(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_Not

ZExpr_Logic_Not::ZExpr_Logic_Not(ZRef<ZExpr_Logic> iOperand)
:	fOperand(iOperand)
	{}

ZExpr_Logic_Not::~ZExpr_Logic_Not()
	{}

void ZExpr_Logic_Not::Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor)
	{ iVisitor.Visit_Logic_Not(this); }

ZRef<ZExpr_Logic> ZExpr_Logic_Not::GetOperand()
	{ return fOperand; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_Dyadic

ZExpr_Logic_Dyadic::ZExpr_Logic_Dyadic(
	ZRef<ZExpr_Logic> iLHS, ZRef<ZExpr_Logic> iRHS)
:	fLHS(iLHS)
,	fRHS(iRHS)
	{
	ZAssert(fLHS && fRHS);
	}

ZExpr_Logic_Dyadic::~ZExpr_Logic_Dyadic()
	{}

ZRef<ZExpr_Logic> ZExpr_Logic_Dyadic::GetLHS()
	{ return fLHS; }

ZRef<ZExpr_Logic> ZExpr_Logic_Dyadic::GetRHS()
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_And

ZExpr_Logic_And::ZExpr_Logic_And(ZRef<ZExpr_Logic> iLHS, ZRef<ZExpr_Logic> iRHS)
:	ZExpr_Logic_Dyadic(iLHS, iRHS)
	{}

void ZExpr_Logic_And::Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor)
	{ iVisitor.Visit_Logic_And(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_Or

ZExpr_Logic_Or::ZExpr_Logic_Or(ZRef<ZExpr_Logic> iLHS, ZRef<ZExpr_Logic> iRHS)
:	ZExpr_Logic_Dyadic(iLHS, iRHS)
	{}

void ZExpr_Logic_Or::Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor)
	{ iVisitor.Visit_Logic_Or(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic

void ZVisitor_Expr_Logic::Visit_Logic_True(ZRef<ZExpr_Logic_True> iRep)
	{ ZVisitor_Expr::Visit_Expr(iRep); }

void ZVisitor_Expr_Logic::Visit_Logic_False(ZRef<ZExpr_Logic_False> iRep)
	{ ZVisitor_Expr::Visit_Expr(iRep); }

void ZVisitor_Expr_Logic::Visit_Logic_Not(ZRef<ZExpr_Logic_Not> iRep)
	{
	if (ZRef<ZExpr_Logic> theRep = iRep->GetOperand())
		theRep->Accept(*this);
	}

void ZVisitor_Expr_Logic::Visit_Logic_And(ZRef<ZExpr_Logic_And> iRep)
	{
	if (ZRef<ZExpr_Logic> theLHS = iRep->GetLHS())
		theLHS->Accept(*this);

	if (ZRef<ZExpr_Logic> theRHS = iRep->GetRHS())
		theRHS->Accept(*this);
	}

void ZVisitor_Expr_Logic::Visit_Logic_Or(ZRef<ZExpr_Logic_Or> iRep)
	{
	if (ZRef<ZExpr_Logic> theLHS = iRep->GetLHS())
		theLHS->Accept(*this);

	if (ZRef<ZExpr_Logic> theRHS = iRep->GetRHS())
		theRHS->Accept(*this);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Operators

ZRef<ZExpr_Logic_Not> sNot(const ZRef<ZExpr_Logic>& iExpr_Logic)
	{ return new ZExpr_Logic_Not(iExpr_Logic); }

ZRef<ZExpr_Logic> sNot(const ZRef<ZExpr_Logic_Not>& iExpr_Logic_Not)
	{ return iExpr_Logic_Not->GetOperand(); }

ZRef<ZExpr_Logic> operator&(bool iBool, const ZRef<ZExpr_Logic>& iExpr_Logic)
	{
	if (iBool)
		return iExpr_Logic;
	return new ZExpr_Logic_False;
	}

ZRef<ZExpr_Logic> operator&(const ZRef<ZExpr_Logic>& iExpr_Logic, bool iBool)
	{
	if (iBool)
		return iExpr_Logic;
	return new ZExpr_Logic_False;
	}


ZRef<ZExpr_Logic> operator|(bool iBool, const ZRef<ZExpr_Logic>& iExpr_Logic)
	{
	if (iBool)
		return new ZExpr_Logic_True;
	return iExpr_Logic;
	}

ZRef<ZExpr_Logic> operator|(const ZRef<ZExpr_Logic>& iExpr_Logic, bool iBool)
	{
	if (iBool)
		return new ZExpr_Logic_True;
	return iExpr_Logic;
	}

ZRef<ZExpr_Logic> operator&(const ZRef<ZExpr_Logic>& iLHS, const ZRef<ZExpr_Logic>& iRHS)
	{
	if (iLHS)
		{
		if (iRHS)
			return new ZExpr_Logic_And(iLHS, iRHS);
		}
	return nullref;
	}

ZRef<ZExpr_Logic>& operator&=(ZRef<ZExpr_Logic>& iLHS, const ZRef<ZExpr_Logic>& iRHS)
	{
	iLHS = iLHS & iRHS;
	return iLHS;
	}

ZRef<ZExpr_Logic> operator|(const ZRef<ZExpr_Logic>& iLHS, const ZRef<ZExpr_Logic>& iRHS)
	{
	if (iLHS)
		{
		if (iRHS)
			return new ZExpr_Logic_Or(iLHS, iRHS);
		return iLHS;
		}
	return iRHS;
	}

ZRef<ZExpr_Logic>& operator|=(ZRef<ZExpr_Logic>& iLHS, const ZRef<ZExpr_Logic>& iRHS)
	{
	iLHS = iLHS | iRHS;
	return iLHS;
	}

NAMESPACE_ZOOLIB_END
