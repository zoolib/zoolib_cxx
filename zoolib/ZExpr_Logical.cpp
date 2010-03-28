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

#include "zoolib/ZExpr_Logical.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep

ZExprRep_Logical::ZExprRep_Logical()
	{}

ZExprRep_Logical::~ZExprRep_Logical()
	{}

bool ZExprRep_Logical::Accept(ZVisitor_ExprRep& iVisitor)
	{
	if (ZVisitor_ExprRep_Logical* theVisitor =
		dynamic_cast<ZVisitor_ExprRep_Logical*>(&iVisitor))
		{
		return this->Accept(*theVisitor);
		}
	else
		{
		return ZExprRep::Accept(iVisitor);
		}
	}

bool ZExprRep_Logical::Accept(ZVisitor_ExprRep_Logical& iVisitor)
	{ return ZExprRep::Accept(iVisitor); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logical_True

ZExprRep_Logical_True::ZExprRep_Logical_True()
	{}

bool ZExprRep_Logical_True::Accept(ZVisitor_ExprRep_Logical& iVisitor)
	{ return iVisitor.Visit_Logical_True(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logical_False

ZExprRep_Logical_False::ZExprRep_Logical_False()
	{}

bool ZExprRep_Logical_False::Accept(ZVisitor_ExprRep_Logical& iVisitor)
	{ return iVisitor.Visit_Logical_False(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logical_Not

ZExprRep_Logical_Not::ZExprRep_Logical_Not(ZRef<ZExprRep_Logical> iOperand)
:	fOperand(iOperand)
	{}

ZExprRep_Logical_Not::~ZExprRep_Logical_Not()
	{}

bool ZExprRep_Logical_Not::Accept(ZVisitor_ExprRep_Logical& iVisitor)
	{ return iVisitor.Visit_Logical_Not(this); }

ZRef<ZExprRep_Logical> ZExprRep_Logical_Not::GetOperand()
	{ return fOperand; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logical_Dyadic

ZExprRep_Logical_Dyadic::ZExprRep_Logical_Dyadic(
	ZRef<ZExprRep_Logical> iLHS, ZRef<ZExprRep_Logical> iRHS)
:	fLHS(iLHS)
,	fRHS(iRHS)
	{}

ZExprRep_Logical_Dyadic::~ZExprRep_Logical_Dyadic()
	{}

ZRef<ZExprRep_Logical> ZExprRep_Logical_Dyadic::GetLHS()
	{ return fLHS; }

ZRef<ZExprRep_Logical> ZExprRep_Logical_Dyadic::GetRHS()
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logical_And

ZExprRep_Logical_And::ZExprRep_Logical_And(ZRef<ZExprRep_Logical> iLHS, ZRef<ZExprRep_Logical> iRHS)
:	ZExprRep_Logical_Dyadic(iLHS, iRHS)
	{}

bool ZExprRep_Logical_And::Accept(ZVisitor_ExprRep_Logical& iVisitor)
	{ return iVisitor.Visit_Logical_And(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logical_Or

ZExprRep_Logical_Or::ZExprRep_Logical_Or(ZRef<ZExprRep_Logical> iLHS, ZRef<ZExprRep_Logical> iRHS)
:	ZExprRep_Logical_Dyadic(iLHS, iRHS)
	{}

bool ZExprRep_Logical_Or::Accept(ZVisitor_ExprRep_Logical& iVisitor)
	{ return iVisitor.Visit_Logical_Or(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Logical

bool ZVisitor_ExprRep_Logical::Visit_Logical_True(ZRef<ZExprRep_Logical_True> iRep)
	{ return ZVisitor_ExprRep::Visit_ExprRep(iRep); }

bool ZVisitor_ExprRep_Logical::Visit_Logical_False(ZRef<ZExprRep_Logical_False> iRep)
	{ return ZVisitor_ExprRep::Visit_ExprRep(iRep); }

bool ZVisitor_ExprRep_Logical::Visit_Logical_Not(ZRef<ZExprRep_Logical_Not> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Logical> theRep = iRep->GetOperand())
		return theRep->Accept(*this);
	
	return true;
	}

bool ZVisitor_ExprRep_Logical::Visit_Logical_And(ZRef<ZExprRep_Logical_And> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Logical> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Logical> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

bool ZVisitor_ExprRep_Logical::Visit_Logical_Or(ZRef<ZExprRep_Logical_Or> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Logical> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Logical> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logical

ZExpr_Logical::ZExpr_Logical()
	{}

ZExpr_Logical::ZExpr_Logical(const ZExpr_Logical& iOther)
:	inherited(iOther)
	{}

ZExpr_Logical::~ZExpr_Logical()
	{}

ZExpr_Logical& ZExpr_Logical::operator=(const ZExpr_Logical& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ZExpr_Logical::ZExpr_Logical(const ZRef<ZExprRep_Logical>& iRep)
:	inherited(iRep)
	{}

ZExpr_Logical::operator ZRef<ZExprRep_Logical>() const
	{ return this->StaticCast<ZExprRep_Logical>(); }

ZExpr_Logical operator&(const ZExpr_Logical& iLHS, const ZExpr_Logical& iRHS)
	{
	if (iLHS)
		{
		if (iRHS)
			return ZExpr_Logical(new ZExprRep_Logical_And(iLHS, iRHS));
		}
	return ZExpr_Logical();
	}

ZExpr_Logical& operator&=(ZExpr_Logical& iLHS, const ZExpr_Logical& iRHS)
	{
	iLHS = iLHS & iRHS;
	return iLHS;
	}

ZExpr_Logical operator|(const ZExpr_Logical& iLHS, const ZExpr_Logical& iRHS)
	{
	if (iLHS)
		{
		if (iRHS)
			return ZExpr_Logical(new ZExprRep_Logical_Or(iLHS, iRHS));
		return iLHS;
		}
	return iRHS;
	}

ZExpr_Logical& operator|=(ZExpr_Logical& iLHS, const ZExpr_Logical& iRHS)
	{
	iLHS = iLHS | iRHS;
	return iLHS;
	}

NAMESPACE_ZOOLIB_END
