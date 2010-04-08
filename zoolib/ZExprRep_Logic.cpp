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

#include "zoolib/ZExprRep_Logic.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep

ZExprRep_Logic::ZExprRep_Logic()
	{}

ZExprRep_Logic::~ZExprRep_Logic()
	{}

bool ZExprRep_Logic::Accept(ZVisitor_ExprRep& iVisitor)
	{
	if (ZVisitor_ExprRep_Logic* theVisitor =
		dynamic_cast<ZVisitor_ExprRep_Logic*>(&iVisitor))
		{
		return this->Accept(*theVisitor);
		}
	else
		{
		return ZExprRep::Accept(iVisitor);
		}
	}

bool ZExprRep_Logic::Accept(ZVisitor_ExprRep_Logic& iVisitor)
	{ return ZExprRep::Accept(iVisitor); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_True

ZExprRep_Logic_True::ZExprRep_Logic_True()
	{}

bool ZExprRep_Logic_True::Accept(ZVisitor_ExprRep_Logic& iVisitor)
	{ return iVisitor.Visit_Logic_True(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_False

ZExprRep_Logic_False::ZExprRep_Logic_False()
	{}

bool ZExprRep_Logic_False::Accept(ZVisitor_ExprRep_Logic& iVisitor)
	{ return iVisitor.Visit_Logic_False(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_Not

ZExprRep_Logic_Not::ZExprRep_Logic_Not(ZRef<ZExprRep_Logic> iOperand)
:	fOperand(iOperand)
	{}

ZExprRep_Logic_Not::~ZExprRep_Logic_Not()
	{}

bool ZExprRep_Logic_Not::Accept(ZVisitor_ExprRep_Logic& iVisitor)
	{ return iVisitor.Visit_Logic_Not(this); }

ZRef<ZExprRep_Logic> ZExprRep_Logic_Not::GetOperand()
	{ return fOperand; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_Dyadic

ZExprRep_Logic_Dyadic::ZExprRep_Logic_Dyadic(
	ZRef<ZExprRep_Logic> iLHS, ZRef<ZExprRep_Logic> iRHS)
:	fLHS(iLHS)
,	fRHS(iRHS)
	{
	ZAssert(fLHS && fRHS);
	}

ZExprRep_Logic_Dyadic::~ZExprRep_Logic_Dyadic()
	{}

ZRef<ZExprRep_Logic> ZExprRep_Logic_Dyadic::GetLHS()
	{ return fLHS; }

ZRef<ZExprRep_Logic> ZExprRep_Logic_Dyadic::GetRHS()
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_And

ZExprRep_Logic_And::ZExprRep_Logic_And(ZRef<ZExprRep_Logic> iLHS, ZRef<ZExprRep_Logic> iRHS)
:	ZExprRep_Logic_Dyadic(iLHS, iRHS)
	{}

bool ZExprRep_Logic_And::Accept(ZVisitor_ExprRep_Logic& iVisitor)
	{ return iVisitor.Visit_Logic_And(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_Or

ZExprRep_Logic_Or::ZExprRep_Logic_Or(ZRef<ZExprRep_Logic> iLHS, ZRef<ZExprRep_Logic> iRHS)
:	ZExprRep_Logic_Dyadic(iLHS, iRHS)
	{}

bool ZExprRep_Logic_Or::Accept(ZVisitor_ExprRep_Logic& iVisitor)
	{ return iVisitor.Visit_Logic_Or(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Logic

bool ZVisitor_ExprRep_Logic::Visit_Logic_True(ZRef<ZExprRep_Logic_True> iRep)
	{ return ZVisitor_ExprRep::Visit_ExprRep(iRep); }

bool ZVisitor_ExprRep_Logic::Visit_Logic_False(ZRef<ZExprRep_Logic_False> iRep)
	{ return ZVisitor_ExprRep::Visit_ExprRep(iRep); }

bool ZVisitor_ExprRep_Logic::Visit_Logic_Not(ZRef<ZExprRep_Logic_Not> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Logic> theRep = iRep->GetOperand())
		return theRep->Accept(*this);
	
	return true;
	}

bool ZVisitor_ExprRep_Logic::Visit_Logic_And(ZRef<ZExprRep_Logic_And> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Logic> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Logic> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

bool ZVisitor_ExprRep_Logic::Visit_Logic_Or(ZRef<ZExprRep_Logic_Or> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Logic> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Logic> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Operators

ZRef<ZExprRep_Logic> operator&(const ZRef<ZExprRep_Logic>& iLHS, const ZRef<ZExprRep_Logic>& iRHS)
	{
	if (iLHS)
		{
		if (iRHS)
			return new ZExprRep_Logic_And(iLHS, iRHS);
		}
	return nullref;
	}

ZRef<ZExprRep_Logic>& operator&=(ZRef<ZExprRep_Logic>& iLHS, const ZRef<ZExprRep_Logic>& iRHS)
	{
	iLHS = iLHS & iRHS;
	return iLHS;
	}

ZRef<ZExprRep_Logic> operator|(const ZRef<ZExprRep_Logic>& iLHS, const ZRef<ZExprRep_Logic>& iRHS)
	{
	if (iLHS)
		{
		if (iRHS)
			return new ZExprRep_Logic_Or(iLHS, iRHS);
		return iLHS;
		}
	return iRHS;
	}

ZRef<ZExprRep_Logic>& operator|=(ZRef<ZExprRep_Logic>& iLHS, const ZRef<ZExprRep_Logic>& iRHS)
	{
	iLHS = iLHS | iRHS;
	return iLHS;
	}

NAMESPACE_ZOOLIB_END
