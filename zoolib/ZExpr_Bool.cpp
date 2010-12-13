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

#include "zoolib/ZExpr_Bool.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool

ZExpr_Bool::ZExpr_Bool()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool_True

ZExpr_Bool_True::ZExpr_Bool_True()
	{}

void ZExpr_Bool_True::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Bool>& iVisitor)
	{
	if (ZVisitor_Expr_Bool_True* theVisitor =
		dynamic_cast<ZVisitor_Expr_Bool_True*>(&iVisitor))
		{
		this->Accept_Expr_Bool_True(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op0(iVisitor);
		}
	}

ZRef<ZExpr_Bool> ZExpr_Bool_True::Self()
	{ return this; }

ZRef<ZExpr_Bool> ZExpr_Bool_True::Clone()
	{ return this; }

void ZExpr_Bool_True::Accept_Expr_Bool_True(ZVisitor_Expr_Bool_True& iVisitor)
	{ iVisitor.Visit_Expr_Bool_True(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_True

void ZVisitor_Expr_Bool_True::Visit_Expr_Bool_True(ZRef<ZExpr_Bool_True> iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool_False

ZExpr_Bool_False::ZExpr_Bool_False()
	{}

void ZExpr_Bool_False::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Bool>& iVisitor)
	{
	if (ZVisitor_Expr_Bool_False* theVisitor =
		dynamic_cast<ZVisitor_Expr_Bool_False*>(&iVisitor))
		{
		this->Accept_Expr_Bool_False(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op0(iVisitor);
		}
	}

ZRef<ZExpr_Bool> ZExpr_Bool_False::Self()
	{ return this; }

ZRef<ZExpr_Bool> ZExpr_Bool_False::Clone()
	{ return this; }

void ZExpr_Bool_False::Accept_Expr_Bool_False(ZVisitor_Expr_Bool_False& iVisitor)
	{ iVisitor.Visit_Expr_Bool_False(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_False

void ZVisitor_Expr_Bool_False::Visit_Expr_Bool_False(ZRef<ZExpr_Bool_False> iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool_Not

ZExpr_Bool_Not::ZExpr_Bool_Not(ZRef<ZExpr_Bool> iOp0)
:	inherited(iOp0)
	{}

void ZExpr_Bool_Not::Accept_Expr_Op1(ZVisitor_Expr_Op1_T<ZExpr_Bool>& iVisitor)
	{
	if (ZVisitor_Expr_Bool_Not* theVisitor =
		dynamic_cast<ZVisitor_Expr_Bool_Not*>(&iVisitor))
		{
		this->Accept_Expr_Bool_Not(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op1(iVisitor);
		}
	}

ZRef<ZExpr_Bool> ZExpr_Bool_Not::Self()
	{ return this; }

ZRef<ZExpr_Bool> ZExpr_Bool_Not::Clone(ZRef<ZExpr_Bool> iOp0)
	{ return new ZExpr_Bool_Not(iOp0); }

void ZExpr_Bool_Not::Accept_Expr_Bool_Not(ZVisitor_Expr_Bool_Not& iVisitor)
	{ iVisitor.Visit_Expr_Bool_Not(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_Not

void ZVisitor_Expr_Bool_Not::Visit_Expr_Bool_Not(ZRef<ZExpr_Bool_Not> iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool_And

ZExpr_Bool_And::ZExpr_Bool_And(ZRef<ZExpr_Bool> iOp0, ZRef<ZExpr_Bool> iOp1)
:	inherited(iOp0, iOp1)
	{}

void ZExpr_Bool_And::Accept_Expr_Op2(ZVisitor_Expr_Op2_T<ZExpr_Bool>& iVisitor)
	{
	if (ZVisitor_Expr_Bool_And* theVisitor =
		dynamic_cast<ZVisitor_Expr_Bool_And*>(&iVisitor))
		{
		this->Accept_Expr_Bool_And(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op2(iVisitor);
		}
	}

ZRef<ZExpr_Bool> ZExpr_Bool_And::Self()
	{ return this; }

ZRef<ZExpr_Bool> ZExpr_Bool_And::Clone(ZRef<ZExpr_Bool> iOp0, ZRef<ZExpr_Bool> iOp1)
	{ return new ZExpr_Bool_And(iOp0, iOp1); }

void ZExpr_Bool_And::Accept_Expr_Bool_And(ZVisitor_Expr_Bool_And& iVisitor)
	{ iVisitor.Visit_Expr_Bool_And(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_And

void ZVisitor_Expr_Bool_And::Visit_Expr_Bool_And(ZRef<ZExpr_Bool_And> iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool_Or

ZExpr_Bool_Or::ZExpr_Bool_Or(ZRef<ZExpr_Bool> iOp0, ZRef<ZExpr_Bool> iOp1)
:	inherited(iOp0, iOp1)
	{}

void ZExpr_Bool_Or::Accept_Expr_Op2(ZVisitor_Expr_Op2_T<ZExpr_Bool>& iVisitor)
	{
	if (ZVisitor_Expr_Bool_Or* theVisitor =
		dynamic_cast<ZVisitor_Expr_Bool_Or*>(&iVisitor))
		{
		this->Accept_Expr_Bool_Or(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op2(iVisitor);
		}
	}

ZRef<ZExpr_Bool> ZExpr_Bool_Or::Self()
	{ return this; }

ZRef<ZExpr_Bool> ZExpr_Bool_Or::Clone(ZRef<ZExpr_Bool> iOp0, ZRef<ZExpr_Bool> iOp1)
	{ return new ZExpr_Bool_Or(iOp0, iOp1); }

void ZExpr_Bool_Or::Accept_Expr_Bool_Or(ZVisitor_Expr_Bool_Or& iVisitor)
	{ iVisitor.Visit_Expr_Bool_Or(this); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_Or

void ZVisitor_Expr_Bool_Or::Visit_Expr_Bool_Or(ZRef<ZExpr_Bool_Or> iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Operators

static ZRef<ZExpr_Bool_True> spTrue = new ZExpr_Bool_True;

ZRef<ZExpr_Bool> sTrue()
	{ return spTrue; }

static ZRef<ZExpr_Bool_False> spFalse = new ZExpr_Bool_False;

ZRef<ZExpr_Bool> sFalse()
	{ return spFalse; }

ZRef<ZExpr_Bool_Not> operator~(const ZRef<ZExpr_Bool>& iExpr_Bool)
	{ return new ZExpr_Bool_Not(iExpr_Bool); }

ZRef<ZExpr_Bool> operator~(const ZRef<ZExpr_Bool_Not>& iExpr_Bool_Not)
	{ return iExpr_Bool_Not->GetOp0(); }

ZRef<ZExpr_Bool> operator&(bool iBool, const ZRef<ZExpr_Bool>& iExpr_Bool)
	{
	if (iBool)
		return iExpr_Bool;
	return sFalse();
	}

ZRef<ZExpr_Bool> operator&(const ZRef<ZExpr_Bool>& iExpr_Bool, bool iBool)
	{
	if (iBool)
		return iExpr_Bool;
	return sFalse();
	}

ZRef<ZExpr_Bool> operator|(bool iBool, const ZRef<ZExpr_Bool>& iExpr_Bool)
	{
	if (iBool)
		return sTrue();
	return iExpr_Bool;
	}

ZRef<ZExpr_Bool> operator|(const ZRef<ZExpr_Bool>& iExpr_Bool, bool iBool)
	{
	if (iBool)
		return sTrue();
	return iExpr_Bool;
	}

ZRef<ZExpr_Bool> operator&(const ZRef<ZExpr_Bool>& iLHS, const ZRef<ZExpr_Bool>& iRHS)
	{
	if (iLHS && iRHS)
		return new ZExpr_Bool_And(iLHS, iRHS);
	return sFalse();
	}

ZRef<ZExpr_Bool>& operator&=(ZRef<ZExpr_Bool>& iLHS, const ZRef<ZExpr_Bool>& iRHS)
	{ return iLHS = iLHS & iRHS; }

ZRef<ZExpr_Bool> operator|(const ZRef<ZExpr_Bool>& iLHS, const ZRef<ZExpr_Bool>& iRHS)
	{
	if (iLHS)
		{
		if (iRHS)
			return new ZExpr_Bool_Or(iLHS, iRHS);
		return iLHS;
		}
	return iRHS;
	}

ZRef<ZExpr_Bool>& operator|=(ZRef<ZExpr_Bool>& iLHS, const ZRef<ZExpr_Bool>& iRHS)
	{ return iLHS = iLHS | iRHS; }

} // namespace ZooLib
