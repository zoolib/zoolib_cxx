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

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic

ZExpr_Logic::ZExpr_Logic()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_True

ZExpr_Logic_True::ZExpr_Logic_True()
	{}

void ZExpr_Logic_True::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Logic>& iVisitor)
	{
	if (ZVisitor_Expr_Logic_True* theVisitor =
		dynamic_cast<ZVisitor_Expr_Logic_True*>(&iVisitor))
		{
		this->Accept_Expr_Logic_True(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op0(iVisitor);
		}
	}

ZRef<ZExpr_Logic> ZExpr_Logic_True::Self()
	{ return this; }

ZRef<ZExpr_Logic> ZExpr_Logic_True::Clone()
	{ return this; }

void ZExpr_Logic_True::Accept_Expr_Logic_True(ZVisitor_Expr_Logic_True& iVisitor)
	{ iVisitor.Visit_Expr_Logic_True(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_True

void ZVisitor_Expr_Logic_True::Visit_Expr_Logic_True(ZRef<ZExpr_Logic_True> iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_False

ZExpr_Logic_False::ZExpr_Logic_False()
	{}

void ZExpr_Logic_False::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Logic>& iVisitor)
	{
	if (ZVisitor_Expr_Logic_False* theVisitor =
		dynamic_cast<ZVisitor_Expr_Logic_False*>(&iVisitor))
		{
		this->Accept_Expr_Logic_False(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op0(iVisitor);
		}
	}

ZRef<ZExpr_Logic> ZExpr_Logic_False::Self()
	{ return this; }

ZRef<ZExpr_Logic> ZExpr_Logic_False::Clone()
	{ return this; }

void ZExpr_Logic_False::Accept_Expr_Logic_False(ZVisitor_Expr_Logic_False& iVisitor)
	{ iVisitor.Visit_Expr_Logic_False(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_False

void ZVisitor_Expr_Logic_False::Visit_Expr_Logic_False(ZRef<ZExpr_Logic_False> iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_Not

ZExpr_Logic_Not::ZExpr_Logic_Not(ZRef<ZExpr_Logic> iOp0)
:	inherited(iOp0)
	{}

void ZExpr_Logic_Not::Accept_Expr_Op1(ZVisitor_Expr_Op1_T<ZExpr_Logic>& iVisitor)
	{
	if (ZVisitor_Expr_Logic_Not* theVisitor =
		dynamic_cast<ZVisitor_Expr_Logic_Not*>(&iVisitor))
		{
		this->Accept_Expr_Logic_Not(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op1(iVisitor);
		}
	}

ZRef<ZExpr_Logic> ZExpr_Logic_Not::Self()
	{ return this; }

ZRef<ZExpr_Logic> ZExpr_Logic_Not::Clone(ZRef<ZExpr_Logic> iOp0)
	{ return new ZExpr_Logic_Not(iOp0); }

void ZExpr_Logic_Not::Accept_Expr_Logic_Not(ZVisitor_Expr_Logic_Not& iVisitor)
	{ iVisitor.Visit_Expr_Logic_Not(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_Not

void ZVisitor_Expr_Logic_Not::Visit_Expr_Logic_Not(ZRef<ZExpr_Logic_Not> iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_And

ZExpr_Logic_And::ZExpr_Logic_And(ZRef<ZExpr_Logic> iOp0, ZRef<ZExpr_Logic> iOp1)
:	inherited(iOp0, iOp1)
	{}

void ZExpr_Logic_And::Accept_Expr_Op2(ZVisitor_Expr_Op2_T<ZExpr_Logic>& iVisitor)
	{
	if (ZVisitor_Expr_Logic_And* theVisitor =
		dynamic_cast<ZVisitor_Expr_Logic_And*>(&iVisitor))
		{
		this->Accept_Expr_Logic_And(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op2(iVisitor);
		}
	}

ZRef<ZExpr_Logic> ZExpr_Logic_And::Self()
	{ return this; }

ZRef<ZExpr_Logic> ZExpr_Logic_And::Clone(ZRef<ZExpr_Logic> iOp0, ZRef<ZExpr_Logic> iOp1)
	{ return new ZExpr_Logic_And(iOp0, iOp1); }

void ZExpr_Logic_And::Accept_Expr_Logic_And(ZVisitor_Expr_Logic_And& iVisitor)
	{ iVisitor.Visit_Expr_Logic_And(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_And

void ZVisitor_Expr_Logic_And::Visit_Expr_Logic_And(ZRef<ZExpr_Logic_And> iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_Or

ZExpr_Logic_Or::ZExpr_Logic_Or(ZRef<ZExpr_Logic> iOp0, ZRef<ZExpr_Logic> iOp1)
:	inherited(iOp0, iOp1)
	{}

void ZExpr_Logic_Or::Accept_Expr_Op2(ZVisitor_Expr_Op2_T<ZExpr_Logic>& iVisitor)
	{
	if (ZVisitor_Expr_Logic_Or* theVisitor =
		dynamic_cast<ZVisitor_Expr_Logic_Or*>(&iVisitor))
		{
		this->Accept_Expr_Logic_Or(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op2(iVisitor);
		}
	}

ZRef<ZExpr_Logic> ZExpr_Logic_Or::Self()
	{ return this; }

ZRef<ZExpr_Logic> ZExpr_Logic_Or::Clone(ZRef<ZExpr_Logic> iOp0, ZRef<ZExpr_Logic> iOp1)
	{ return new ZExpr_Logic_Or(iOp0, iOp1); }

void ZExpr_Logic_Or::Accept_Expr_Logic_Or(ZVisitor_Expr_Logic_Or& iVisitor)
	{ iVisitor.Visit_Expr_Logic_Or(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_Or

void ZVisitor_Expr_Logic_Or::Visit_Expr_Logic_Or(ZRef<ZExpr_Logic_Or> iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Operators

static ZRef<ZExpr_Logic_True> spTrue = new ZExpr_Logic_True;

ZRef<ZExpr_Logic> sTrue()
	{ return spTrue; }

static ZRef<ZExpr_Logic_False> spFalse = new ZExpr_Logic_False;

ZRef<ZExpr_Logic> sFalse()
	{ return spFalse; }

ZRef<ZExpr_Logic_Not> operator~(const ZRef<ZExpr_Logic>& iExpr_Logic)
	{ return new ZExpr_Logic_Not(iExpr_Logic); }

ZRef<ZExpr_Logic> operator~(const ZRef<ZExpr_Logic_Not>& iExpr_Logic_Not)
	{ return iExpr_Logic_Not->GetOp0(); }

ZRef<ZExpr_Logic> operator&(bool iBool, const ZRef<ZExpr_Logic>& iExpr_Logic)
	{
	if (iBool)
		return iExpr_Logic;
	return sFalse();
	}

ZRef<ZExpr_Logic> operator&(const ZRef<ZExpr_Logic>& iExpr_Logic, bool iBool)
	{
	if (iBool)
		return iExpr_Logic;
	return sFalse();
	}

ZRef<ZExpr_Logic> operator|(bool iBool, const ZRef<ZExpr_Logic>& iExpr_Logic)
	{
	if (iBool)
		return sTrue();
	return iExpr_Logic;
	}

ZRef<ZExpr_Logic> operator|(const ZRef<ZExpr_Logic>& iExpr_Logic, bool iBool)
	{
	if (iBool)
		return sTrue();
	return iExpr_Logic;
	}

ZRef<ZExpr_Logic> operator&(const ZRef<ZExpr_Logic>& iLHS, const ZRef<ZExpr_Logic>& iRHS)
	{
	if (iLHS && iRHS)
		return new ZExpr_Logic_And(iLHS, iRHS);
	return sFalse();
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

} // namespace ZooLib
