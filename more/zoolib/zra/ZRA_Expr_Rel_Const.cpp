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

#include "zoolib/ZCompare_Ref.h"
#include "zoolib/zra/ZRA_Expr_Rel_Const.h"

namespace ZooLib {

// =================================================================================================
// MARK: - sCompare_T

template <>
int sCompare_T(const ZRA::Expr_Rel_Const& iL, const ZRA::Expr_Rel_Const& iR)
	{
	if (int compare = sCompare_T(iL.GetRelName(), iR.GetRelName()))
		return compare;

	return sCompare_T(iL.GetVal(), iR.GetVal());
	}

ZMACRO_CompareRegistration_T(ZRA::Expr_Rel_Const)

namespace ZRA {

// =================================================================================================
// MARK: - Expr_Rel_Const

Expr_Rel_Const::Expr_Rel_Const(const RelName& iRelName, const ZVal_Any& iVal)
:	fRelName(iRelName)
,	fVal(iVal)
	{}

Expr_Rel_Const::~Expr_Rel_Const()
	{}

void Expr_Rel_Const::Accept(const ZVisitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Const* theVisitor = DynNonConst<Visitor_Expr_Rel_Const*>(&iVisitor))
		this->Accept_Expr_Rel_Const(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Rel_Const::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Const* theVisitor = DynNonConst<Visitor_Expr_Rel_Const*>(&iVisitor))
		this->Accept_Expr_Rel_Const(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZRef<Expr_Rel> Expr_Rel_Const::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Const::Clone()
	{ return this; }

void Expr_Rel_Const::Accept_Expr_Rel_Const(Visitor_Expr_Rel_Const& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Const(this); }

const RelName& Expr_Rel_Const::GetRelName() const
	{ return fRelName; }

const ZVal_Any& Expr_Rel_Const::GetVal() const
	{ return fVal; }

// =================================================================================================
// MARK: - Visitor_Expr_Rel_Const

void Visitor_Expr_Rel_Const::Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
// MARK: - Relational operators

ZRef<Expr_Rel> sConst(const RelName& iRelName, const ZVal_Any& iVal)
	{ return new Expr_Rel_Const(iRelName, iVal); }

} // namespace ZRA
} // namespace ZooLib
