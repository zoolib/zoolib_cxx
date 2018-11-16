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

#include "zoolib/Compare_Ref.h"

#include "zoolib/Expr/Expr_Bool.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Expr_Bool

Expr_Bool::Expr_Bool()
	{}

// =================================================================================================
#pragma mark - Expr_Bool_True

Expr_Bool_True::Expr_Bool_True()
	{}

ZRef<Expr_Bool> Expr_Bool_True::sTrue()
	{
	static ZRef<Expr_Bool_True> spTrue = new Expr_Bool_True;
	return spTrue;
	}
//	{ return sSingleton<ZRef<Expr_Bool_True> >(); } ???

void Expr_Bool_True::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Bool_True* theVisitor = sDynNonConst<Visitor_Expr_Bool_True>(&iVisitor))
		this->Accept_Expr_Bool_True(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Bool_True::Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Bool>& iVisitor)
	{
	if (Visitor_Expr_Bool_True* theVisitor = sDynNonConst<Visitor_Expr_Bool_True>(&iVisitor))
		this->Accept_Expr_Bool_True(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZRef<Expr_Bool> Expr_Bool_True::Self()
	{ return this; }

ZRef<Expr_Bool> Expr_Bool_True::Clone()
	{ return this; }

void Expr_Bool_True::Accept_Expr_Bool_True(Visitor_Expr_Bool_True& iVisitor)
	{ iVisitor.Visit_Expr_Bool_True(this); }

template <>
int sCompare_T(const Expr_Bool_True& iL, const Expr_Bool_True& iR)
	{ return 0; }

ZMACRO_CompareRegistration_T(Expr_Bool_True)

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_True

void Visitor_Expr_Bool_True::Visit_Expr_Bool_True(const ZRef<Expr_Bool_True>& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark - Expr_Bool_False

Expr_Bool_False::Expr_Bool_False()
	{}

ZRef<Expr_Bool> Expr_Bool_False::sFalse()
	{
	static ZRef<Expr_Bool> spFalse = new Expr_Bool_False;
	return spFalse;
	}

void Expr_Bool_False::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Bool_False* theVisitor = sDynNonConst<Visitor_Expr_Bool_False>(&iVisitor))
		this->Accept_Expr_Bool_False(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Bool_False::Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Bool>& iVisitor)
	{
	if (Visitor_Expr_Bool_False* theVisitor = sDynNonConst<Visitor_Expr_Bool_False>(&iVisitor))
		this->Accept_Expr_Bool_False(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZRef<Expr_Bool> Expr_Bool_False::Self()
	{ return this; }

ZRef<Expr_Bool> Expr_Bool_False::Clone()
	{ return this; }

void Expr_Bool_False::Accept_Expr_Bool_False(Visitor_Expr_Bool_False& iVisitor)
	{ iVisitor.Visit_Expr_Bool_False(this); }

template <>
int sCompare_T(const Expr_Bool_False& iL, const Expr_Bool_False& iR)
	{ return 0; }

ZMACRO_CompareRegistration_T(Expr_Bool_False)

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_False

void Visitor_Expr_Bool_False::Visit_Expr_Bool_False(const ZRef<Expr_Bool_False>& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark - Expr_Bool_Not

Expr_Bool_Not::Expr_Bool_Not(const ZRef<Expr_Bool>& iOp0)
:	inherited(iOp0)
	{}

void Expr_Bool_Not::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Bool_Not* theVisitor = sDynNonConst<Visitor_Expr_Bool_Not>(&iVisitor))
		this->Accept_Expr_Bool_Not(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Bool_Not::Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Bool>& iVisitor)
	{
	if (Visitor_Expr_Bool_Not* theVisitor = sDynNonConst<Visitor_Expr_Bool_Not>(&iVisitor))
		this->Accept_Expr_Bool_Not(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZRef<Expr_Bool> Expr_Bool_Not::Self()
	{ return this; }

ZRef<Expr_Bool> Expr_Bool_Not::Clone(const ZRef<Expr_Bool>& iOp0)
	{ return new Expr_Bool_Not(iOp0); }

void Expr_Bool_Not::Accept_Expr_Bool_Not(Visitor_Expr_Bool_Not& iVisitor)
	{ iVisitor.Visit_Expr_Bool_Not(this); }

template <>
int sCompare_T(const Expr_Bool_Not& iL, const Expr_Bool_Not& iR)
	{ return sCompare_T(iL.GetOp0(), iR.GetOp0()); }

ZMACRO_CompareRegistration_T(Expr_Bool_Not)

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_Not

void Visitor_Expr_Bool_Not::Visit_Expr_Bool_Not(const ZRef<Expr_Bool_Not>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark - Expr_Bool_And

Expr_Bool_And::Expr_Bool_And(const ZRef<Expr_Bool>& iOp0, const ZRef<Expr_Bool>& iOp1)
:	inherited(iOp0, iOp1)
	{}

void Expr_Bool_And::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Bool_And* theVisitor = sDynNonConst<Visitor_Expr_Bool_And>(&iVisitor))
		this->Accept_Expr_Bool_And(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Bool_And::Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Bool>& iVisitor)
	{
	if (Visitor_Expr_Bool_And* theVisitor = sDynNonConst<Visitor_Expr_Bool_And>(&iVisitor))
		this->Accept_Expr_Bool_And(*theVisitor);
	else
		inherited::Accept_Expr_Op2(iVisitor);
	}

ZRef<Expr_Bool> Expr_Bool_And::Self()
	{ return this; }

ZRef<Expr_Bool> Expr_Bool_And::Clone(const ZRef<Expr_Bool>& iOp0, const ZRef<Expr_Bool>& iOp1)
	{ return new Expr_Bool_And(iOp0, iOp1); }

void Expr_Bool_And::Accept_Expr_Bool_And(Visitor_Expr_Bool_And& iVisitor)
	{ iVisitor.Visit_Expr_Bool_And(this); }

template <>
int sCompare_T(const Expr_Bool_And& iL, const Expr_Bool_And& iR)
	{
	if (int compare = sCompare_T(iL.GetOp0(), iR.GetOp0()))
		return compare;
	return sCompare_T(iL.GetOp1(), iR.GetOp1());
	}

ZMACRO_CompareRegistration_T(Expr_Bool_And)

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_And

void Visitor_Expr_Bool_And::Visit_Expr_Bool_And(const ZRef<Expr_Bool_And>& iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark - Expr_Bool_Or

Expr_Bool_Or::Expr_Bool_Or(const ZRef<Expr_Bool>& iOp0, const ZRef<Expr_Bool>& iOp1)
:	inherited(iOp0, iOp1)
	{}

void Expr_Bool_Or::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Bool_Or* theVisitor = sDynNonConst<Visitor_Expr_Bool_Or>(&iVisitor))
		this->Accept_Expr_Bool_Or(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Bool_Or::Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Bool>& iVisitor)
	{
	if (Visitor_Expr_Bool_Or* theVisitor = sDynNonConst<Visitor_Expr_Bool_Or>(&iVisitor))
		this->Accept_Expr_Bool_Or(*theVisitor);
	else
		inherited::Accept_Expr_Op2(iVisitor);
	}

ZRef<Expr_Bool> Expr_Bool_Or::Self()
	{ return this; }

ZRef<Expr_Bool> Expr_Bool_Or::Clone(const ZRef<Expr_Bool>& iOp0, const ZRef<Expr_Bool>& iOp1)
	{ return new Expr_Bool_Or(iOp0, iOp1); }

void Expr_Bool_Or::Accept_Expr_Bool_Or(Visitor_Expr_Bool_Or& iVisitor)
	{ iVisitor.Visit_Expr_Bool_Or(this); }

template <>
int sCompare_T(const Expr_Bool_Or& iL, const Expr_Bool_Or& iR)
	{
	if (int compare = sCompare_T(iL.GetOp0(), iR.GetOp0()))
		return compare;
	return sCompare_T(iL.GetOp1(), iR.GetOp1());
	}

ZMACRO_CompareRegistration_T(Expr_Bool_Or)

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_Or

void Visitor_Expr_Bool_Or::Visit_Expr_Bool_Or(const ZRef<Expr_Bool_Or>& iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark - Operators

ZRef<Expr_Bool> sTrue()
	{ return Expr_Bool_True::sTrue(); }

ZRef<Expr_Bool> sFalse()
	{ return Expr_Bool_False::sFalse(); }

ZRef<Expr_Bool> operator~(const ZRef<Expr_Bool>& iExpr_Bool)
	{ return new Expr_Bool_Not(iExpr_Bool); }

ZRef<Expr_Bool> sNot(const ZRef<Expr_Bool>& iExpr_Bool)
	{ return new Expr_Bool_Not(iExpr_Bool); }

ZRef<Expr_Bool> sAnd(const ZRef<Expr_Bool>& iLHS, const ZRef<Expr_Bool>& iRHS)
	{ return iLHS & iRHS; }

ZRef<Expr_Bool> sOr(const ZRef<Expr_Bool>& iLHS, const ZRef<Expr_Bool>& iRHS)
	{ return iLHS | iRHS; }

// =================================================================================================
#pragma mark - Operators

ZRef<Expr_Bool> operator&(bool iBool, const ZRef<Expr_Bool>& iExpr_Bool)
	{
	if (iBool)
		return iExpr_Bool;
	return sFalse();
	}

ZRef<Expr_Bool> operator&(const ZRef<Expr_Bool>& iExpr_Bool, bool iBool)
	{
	if (iBool)
		return iExpr_Bool;
	return sFalse();
	}

ZRef<Expr_Bool> operator|(bool iBool, const ZRef<Expr_Bool>& iExpr_Bool)
	{
	if (iBool)
		return sTrue();
	return iExpr_Bool;
	}

ZRef<Expr_Bool> operator|(const ZRef<Expr_Bool>& iExpr_Bool, bool iBool)
	{
	if (iBool)
		return sTrue();
	return iExpr_Bool;
	}

ZRef<Expr_Bool> operator&(const ZRef<Expr_Bool>& iLHS, const ZRef<Expr_Bool>& iRHS)
	{
	if (iLHS && iRHS)
		return new Expr_Bool_And(iLHS, iRHS);
	return sFalse();
	}

ZRef<Expr_Bool>& operator&=(ZRef<Expr_Bool>& ioLHS, const ZRef<Expr_Bool>& iRHS)
	{ return ioLHS = ioLHS & iRHS; }

ZRef<Expr_Bool> operator|(const ZRef<Expr_Bool>& iLHS, const ZRef<Expr_Bool>& iRHS)
	{
	if (iLHS)
		{
		if (iRHS)
			return new Expr_Bool_Or(iLHS, iRHS);
		return iLHS;
		}
	return iRHS;
	}

ZRef<Expr_Bool>& operator|=(ZRef<Expr_Bool>& ioLHS, const ZRef<Expr_Bool>& iRHS)
	{ return ioLHS = ioLHS | iRHS; }

} // namespace ZooLib
