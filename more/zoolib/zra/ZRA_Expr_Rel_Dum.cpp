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

#include "zoolib/ZCompare_Ref.h"
#include "zoolib/zra/ZRA_Expr_Rel_Dum.h"

namespace ZooLib {

// =================================================================================================
// MARK: - sCompare_T

template <>
int sCompare_T(const ZRA::Expr_Rel_Dum& iL, const ZRA::Expr_Rel_Dum& iR)
	{ return 0; }

ZMACRO_CompareRegistration_T(ZRA::Expr_Rel_Dum)

namespace ZRA {

// =================================================================================================
// MARK: - Expr_Rel_Dum

Expr_Rel_Dum::Expr_Rel_Dum()
	{}

Expr_Rel_Dum::~Expr_Rel_Dum()
	{}

void Expr_Rel_Dum::Accept(const ZVisitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Dum* theVisitor = DynNonConst<Visitor_Expr_Rel_Dum>(&iVisitor))
		this->Accept_Expr_Rel_Dum(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Rel_Dum::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Dum* theVisitor = DynNonConst<Visitor_Expr_Rel_Dum>(&iVisitor))
		this->Accept_Expr_Rel_Dum(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZRef<Expr_Rel> Expr_Rel_Dum::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Dum::Clone()
	{ return this; }

void Expr_Rel_Dum::Accept_Expr_Rel_Dum(Visitor_Expr_Rel_Dum& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Dum(this); }

// =================================================================================================
// MARK: - Visitor_Expr_Rel_Dum

void Visitor_Expr_Rel_Dum::Visit_Expr_Rel_Dum(const ZRef<Expr_Rel_Dum>& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
// MARK: - Relational operators

ZRef<Expr_Rel> sDum()
	{ return new Expr_Rel_Dum; }

} // namespace ZRA
} // namespace ZooLib
