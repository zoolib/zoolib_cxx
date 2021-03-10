// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Expr/Expr_Bool.h"

#include "zoolib/Compare_Ref.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Expr_Bool

Expr_Bool::Expr_Bool()
	{}

// =================================================================================================
#pragma mark - Expr_Bool_True

Expr_Bool_True::Expr_Bool_True()
	{}

ZP<Expr_Bool> Expr_Bool_True::sTrue()
	{
	static ZP<Expr_Bool_True> spTrue = new Expr_Bool_True;
	return spTrue;
	}
//	{ return sSingleton<ZP<Expr_Bool_True>>(); } ???

void Expr_Bool_True::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Bool_True* theVisitor = sDynNonConst<Visitor_Expr_Bool_True>(&iVisitor))
		this->Accept_Expr_Bool_True(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Bool_True::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Bool_True> other = iOther.DynamicCast<Expr_Bool_True>())
		return 0;
	return Expr::Compare(iOther);
	}

void Expr_Bool_True::Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Bool>& iVisitor)
	{
	if (Visitor_Expr_Bool_True* theVisitor = sDynNonConst<Visitor_Expr_Bool_True>(&iVisitor))
		this->Accept_Expr_Bool_True(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZP<Expr_Bool> Expr_Bool_True::Self()
	{ return this; }

ZP<Expr_Bool> Expr_Bool_True::Clone()
	{ return this; }

void Expr_Bool_True::Accept_Expr_Bool_True(Visitor_Expr_Bool_True& iVisitor)
	{ iVisitor.Visit_Expr_Bool_True(this); }

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_True

void Visitor_Expr_Bool_True::Visit_Expr_Bool_True(const ZP<Expr_Bool_True>& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark - Expr_Bool_False

Expr_Bool_False::Expr_Bool_False()
	{}

ZP<Expr_Bool> Expr_Bool_False::sFalse()
	{
	static ZP<Expr_Bool> spFalse = new Expr_Bool_False;
	return spFalse;
	}

void Expr_Bool_False::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Bool_False* theVisitor = sDynNonConst<Visitor_Expr_Bool_False>(&iVisitor))
		this->Accept_Expr_Bool_False(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Bool_False::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Bool_False> other = iOther.DynamicCast<Expr_Bool_False>())
		return 0;
	return Expr::Compare(iOther);
	}

void Expr_Bool_False::Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Bool>& iVisitor)
	{
	if (Visitor_Expr_Bool_False* theVisitor = sDynNonConst<Visitor_Expr_Bool_False>(&iVisitor))
		this->Accept_Expr_Bool_False(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZP<Expr_Bool> Expr_Bool_False::Self()
	{ return this; }

ZP<Expr_Bool> Expr_Bool_False::Clone()
	{ return this; }

void Expr_Bool_False::Accept_Expr_Bool_False(Visitor_Expr_Bool_False& iVisitor)
	{ iVisitor.Visit_Expr_Bool_False(this); }

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_False

void Visitor_Expr_Bool_False::Visit_Expr_Bool_False(const ZP<Expr_Bool_False>& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark - Expr_Bool_Not

Expr_Bool_Not::Expr_Bool_Not(const ZP<Expr_Bool>& iOp0)
:	inherited(iOp0)
	{}

void Expr_Bool_Not::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Bool_Not* theVisitor = sDynNonConst<Visitor_Expr_Bool_Not>(&iVisitor))
		this->Accept_Expr_Bool_Not(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Bool_Not::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Bool_Not> other = iOther.DynamicCast<Expr_Bool_Not>())
		return this->GetOp0()->Compare(other->GetOp0());
	return Expr::Compare(iOther);
	}

void Expr_Bool_Not::Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Bool>& iVisitor)
	{
	if (Visitor_Expr_Bool_Not* theVisitor = sDynNonConst<Visitor_Expr_Bool_Not>(&iVisitor))
		this->Accept_Expr_Bool_Not(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZP<Expr_Bool> Expr_Bool_Not::Self()
	{ return this; }

ZP<Expr_Bool> Expr_Bool_Not::Clone(const ZP<Expr_Bool>& iOp0)
	{ return new Expr_Bool_Not(iOp0); }

void Expr_Bool_Not::Accept_Expr_Bool_Not(Visitor_Expr_Bool_Not& iVisitor)
	{ iVisitor.Visit_Expr_Bool_Not(this); }

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_Not

void Visitor_Expr_Bool_Not::Visit_Expr_Bool_Not(const ZP<Expr_Bool_Not>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark - Expr_Bool_And

Expr_Bool_And::Expr_Bool_And(const ZP<Expr_Bool>& iOp0, const ZP<Expr_Bool>& iOp1)
:	inherited(iOp0, iOp1)
	{}

void Expr_Bool_And::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Bool_And* theVisitor = sDynNonConst<Visitor_Expr_Bool_And>(&iVisitor))
		this->Accept_Expr_Bool_And(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Bool_And::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Bool_And> other = iOther.DynamicCast<Expr_Bool_And>())
		{
		if (int compare = this->GetOp0()->Compare(other->GetOp0()))
			return compare;
		return this->GetOp1()->Compare(other->GetOp1());
		}
	return Expr::Compare(iOther);
	}

void Expr_Bool_And::Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Bool>& iVisitor)
	{
	if (Visitor_Expr_Bool_And* theVisitor = sDynNonConst<Visitor_Expr_Bool_And>(&iVisitor))
		this->Accept_Expr_Bool_And(*theVisitor);
	else
		inherited::Accept_Expr_Op2(iVisitor);
	}

ZP<Expr_Bool> Expr_Bool_And::Self()
	{ return this; }

ZP<Expr_Bool> Expr_Bool_And::Clone(const ZP<Expr_Bool>& iOp0, const ZP<Expr_Bool>& iOp1)
	{ return new Expr_Bool_And(iOp0, iOp1); }

void Expr_Bool_And::Accept_Expr_Bool_And(Visitor_Expr_Bool_And& iVisitor)
	{ iVisitor.Visit_Expr_Bool_And(this); }

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_And

void Visitor_Expr_Bool_And::Visit_Expr_Bool_And(const ZP<Expr_Bool_And>& iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark - Expr_Bool_Or

Expr_Bool_Or::Expr_Bool_Or(const ZP<Expr_Bool>& iOp0, const ZP<Expr_Bool>& iOp1)
:	inherited(iOp0, iOp1)
	{}

void Expr_Bool_Or::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Bool_Or* theVisitor = sDynNonConst<Visitor_Expr_Bool_Or>(&iVisitor))
		this->Accept_Expr_Bool_Or(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Bool_Or::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Bool_Or> other = iOther.DynamicCast<Expr_Bool_Or>())
		{
		if (int compare = this->GetOp0()->Compare(other->GetOp0()))
			return compare;
		return this->GetOp1()->Compare(other->GetOp1());
		}
	return Expr::Compare(iOther);
	}

void Expr_Bool_Or::Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Bool>& iVisitor)
	{
	if (Visitor_Expr_Bool_Or* theVisitor = sDynNonConst<Visitor_Expr_Bool_Or>(&iVisitor))
		this->Accept_Expr_Bool_Or(*theVisitor);
	else
		inherited::Accept_Expr_Op2(iVisitor);
	}

ZP<Expr_Bool> Expr_Bool_Or::Self()
	{ return this; }

ZP<Expr_Bool> Expr_Bool_Or::Clone(const ZP<Expr_Bool>& iOp0, const ZP<Expr_Bool>& iOp1)
	{ return new Expr_Bool_Or(iOp0, iOp1); }

void Expr_Bool_Or::Accept_Expr_Bool_Or(Visitor_Expr_Bool_Or& iVisitor)
	{ iVisitor.Visit_Expr_Bool_Or(this); }

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_Or

void Visitor_Expr_Bool_Or::Visit_Expr_Bool_Or(const ZP<Expr_Bool_Or>& iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark - Operators

ZP<Expr_Bool> sTrue()
	{ return Expr_Bool_True::sTrue(); }

ZP<Expr_Bool> sFalse()
	{ return Expr_Bool_False::sFalse(); }

ZP<Expr_Bool> operator~(const ZP<Expr_Bool>& iExpr_Bool)
	{ return new Expr_Bool_Not(iExpr_Bool); }

ZP<Expr_Bool> sNot(const ZP<Expr_Bool>& iExpr_Bool)
	{ return new Expr_Bool_Not(iExpr_Bool); }

ZP<Expr_Bool> sAnd(const ZP<Expr_Bool>& iLHS, const ZP<Expr_Bool>& iRHS)
	{ return iLHS & iRHS; }

ZP<Expr_Bool> sOr(const ZP<Expr_Bool>& iLHS, const ZP<Expr_Bool>& iRHS)
	{ return iLHS | iRHS; }

// =================================================================================================
#pragma mark - Operators

ZP<Expr_Bool> operator&(bool iBool, const ZP<Expr_Bool>& iExpr_Bool)
	{
	if (iBool)
		return iExpr_Bool;
	return sFalse();
	}

ZP<Expr_Bool> operator&(const ZP<Expr_Bool>& iExpr_Bool, bool iBool)
	{
	if (iBool)
		return iExpr_Bool;
	return sFalse();
	}

ZP<Expr_Bool> operator|(bool iBool, const ZP<Expr_Bool>& iExpr_Bool)
	{
	if (iBool)
		return sTrue();
	return iExpr_Bool;
	}

ZP<Expr_Bool> operator|(const ZP<Expr_Bool>& iExpr_Bool, bool iBool)
	{
	if (iBool)
		return sTrue();
	return iExpr_Bool;
	}

ZP<Expr_Bool> operator&(const ZP<Expr_Bool>& iLHS, const ZP<Expr_Bool>& iRHS)
	{
	if (iLHS && iRHS)
		return new Expr_Bool_And(iLHS, iRHS);
	return sFalse();
	}

ZP<Expr_Bool>& operator&=(ZP<Expr_Bool>& ioLHS, const ZP<Expr_Bool>& iRHS)
	{ return ioLHS = ioLHS & iRHS; }

ZP<Expr_Bool> operator|(const ZP<Expr_Bool>& iLHS, const ZP<Expr_Bool>& iRHS)
	{
	if (iLHS)
		{
		if (iRHS)
			return new Expr_Bool_Or(iLHS, iRHS);
		return iLHS;
		}
	return iRHS;
	}

ZP<Expr_Bool>& operator|=(ZP<Expr_Bool>& ioLHS, const ZP<Expr_Bool>& iRHS)
	{ return ioLHS = ioLHS | iRHS; }

} // namespace ZooLib
