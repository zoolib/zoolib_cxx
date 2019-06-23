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

#include "zoolib/ValPred/Expr_Bool_ValPred.h"

#include "zoolib/Compare.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Expr_Bool_ValPred

Expr_Bool_ValPred::Expr_Bool_ValPred(const ValPred& iValPred)
:	fValPred(iValPred)
	{}

Expr_Bool_ValPred::~Expr_Bool_ValPred()
	{}

void Expr_Bool_ValPred::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Bool_ValPred* theVisitor = sDynNonConst<Visitor_Expr_Bool_ValPred>(&iVisitor))
		this->Accept_Expr_Bool_ValPred(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Bool_ValPred::Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Bool>& iVisitor)
	{
	if (Visitor_Expr_Bool_ValPred* theVisitor = sDynNonConst<Visitor_Expr_Bool_ValPred>(&iVisitor))
		this->Accept_Expr_Bool_ValPred(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZP<Expr_Bool> Expr_Bool_ValPred::Self()
	{ return this; }

ZP<Expr_Bool> Expr_Bool_ValPred::Clone()
	{ return this; }

void Expr_Bool_ValPred::Accept_Expr_Bool_ValPred(Visitor_Expr_Bool_ValPred& iVisitor)
	{ iVisitor.Visit_Expr_Bool_ValPred(this); }

const ValPred& Expr_Bool_ValPred::GetValPred() const
	{ return fValPred; }

template <>
int sCompare_T(const Expr_Bool_ValPred& iL, const Expr_Bool_ValPred& iR)
	{ return sCompare_T(iL.GetValPred(), iR.GetValPred()); }

ZMACRO_CompareRegistration_T(Expr_Bool_ValPred)

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_ValPred

void Visitor_Expr_Bool_ValPred::Visit_Expr_Bool_ValPred(const ZP<Expr_Bool_ValPred >& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark - Operators

ZP<Expr_Bool> sExpr_Bool(const ValPred& iValPred)
	{ return new Expr_Bool_ValPred(iValPred); }

ZP<Expr_Bool> operator~(const ValPred& iValPred)
	{ return new Expr_Bool_Not(sExpr_Bool(iValPred)); }

ZP<Expr_Bool> operator&(bool iBool, const ValPred& iValPred)
	{
	if (iBool)
		return sExpr_Bool(iValPred);
	return sFalse();
	}

ZP<Expr_Bool> operator&(const ValPred& iValPred, bool iBool)
	{
	if (iBool)
		return sExpr_Bool(iValPred);
	return sFalse();
	}

ZP<Expr_Bool> operator|(bool iBool, const ValPred& iValPred)
	{
	if (iBool)
		return sTrue();
	return sExpr_Bool(iValPred);
	}

ZP<Expr_Bool> operator|(const ValPred& iValPred, bool iBool)
	{
	if (iBool)
		return sTrue();
	return sExpr_Bool(iValPred);
	}

ZP<Expr_Bool> operator&(const ValPred& iLHS, const ValPred& iRHS)
	{ return new Expr_Bool_And(sExpr_Bool(iLHS), sExpr_Bool(iRHS)); }

ZP<Expr_Bool> operator&(const ValPred& iLHS, const ZP<Expr_Bool>& iRHS)
	{ return new Expr_Bool_And(sExpr_Bool(iLHS), iRHS); }

ZP<Expr_Bool> operator&(const ZP<Expr_Bool>& iLHS, const ValPred& iRHS)
	{ return new Expr_Bool_And(iLHS, sExpr_Bool(iRHS)); }

ZP<Expr_Bool>& operator&=(ZP<Expr_Bool>& ioLHS, const ValPred& iRHS)
	{ return ioLHS = ioLHS & iRHS; }

ZP<Expr_Bool> operator|(const ValPred& iLHS, const ValPred& iRHS)
	{ return new Expr_Bool_Or(sExpr_Bool(iLHS), sExpr_Bool(iRHS)); }

ZP<Expr_Bool> operator|(const ValPred& iLHS, const ZP<Expr_Bool>& iRHS)
	{ return new Expr_Bool_Or(sExpr_Bool(iLHS), iRHS); }

ZP<Expr_Bool> operator|(const ZP<Expr_Bool>& iLHS, const ValPred& iRHS)
	{ return new Expr_Bool_Or(iLHS, sExpr_Bool(iRHS)); }

ZP<Expr_Bool>& operator|=(ZP<Expr_Bool>& ioLHS, const ValPred& iRHS)
	{ return ioLHS = ioLHS | iRHS; }

} // namespace ZooLib
