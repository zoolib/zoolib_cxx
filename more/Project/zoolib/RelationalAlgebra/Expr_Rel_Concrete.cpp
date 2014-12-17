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
#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"

namespace ZooLib {

// =================================================================================================
// MARK: - sCompare_T

template <>
int sCompare_T(const RelationalAlgebra::Expr_Rel_Concrete& iL,
	const RelationalAlgebra::Expr_Rel_Concrete& iR)
	{ return sCompare_T(iL.GetConcreteHead(), iR.GetConcreteHead()); }

ZMACRO_CompareRegistration_T(RelationalAlgebra::Expr_Rel_Concrete)

namespace RelationalAlgebra {

// =================================================================================================
// MARK: - Expr_Rel_Concrete

Expr_Rel_Concrete::Expr_Rel_Concrete(const ConcreteHead& iConcreteHead)
:	fConcreteHead(iConcreteHead)
	{}

void Expr_Rel_Concrete::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Concrete* theVisitor = sDynNonConst<Visitor_Expr_Rel_Concrete>(&iVisitor))
		this->Accept_Expr_Rel_Concrete(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Rel_Concrete::Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Concrete* theVisitor = sDynNonConst<Visitor_Expr_Rel_Concrete>(&iVisitor))
		this->Accept_Expr_Rel_Concrete(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZRef<Expr_Rel> Expr_Rel_Concrete::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Concrete::Clone()
	{ return this; }

void Expr_Rel_Concrete::Accept_Expr_Rel_Concrete(Visitor_Expr_Rel_Concrete& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Concrete(this); }

const ConcreteHead& Expr_Rel_Concrete::GetConcreteHead() const
	{ return fConcreteHead; }

// =================================================================================================
// MARK: - Visitor_Expr_Rel_Concrete

void Visitor_Expr_Rel_Concrete::Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
// MARK: - sConcrete

ZRef<Expr_Rel> sConcrete(const RelHead& iRelHead)
	{ return new Expr_Rel_Concrete(sConcreteHead(iRelHead)); }

ZRef<Expr_Rel> sConcrete(const RelHead& iRequired, const RelHead& iOptional)
	{ return new Expr_Rel_Concrete(sConcreteHead(iRequired, iOptional)); }

ZRef<Expr_Rel> sConcrete(const ConcreteHead& iConcreteHead)
	{ return new Expr_Rel_Concrete(iConcreteHead); }

} // namespace RelationalAlgebra
} // namespace ZooLib
