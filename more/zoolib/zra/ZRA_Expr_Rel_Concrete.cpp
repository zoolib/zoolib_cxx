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

#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Concrete

Expr_Rel_Concrete::Expr_Rel_Concrete(const RelHead& iRelHead)
:	fRelHead(iRelHead)
	{}

void Expr_Rel_Concrete::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Concrete* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Concrete*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Concrete(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op0(iVisitor);
		}
	}

ZRef<Expr_Rel> Expr_Rel_Concrete::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Concrete::Clone()
	{ return this; }

void Expr_Rel_Concrete::Accept_Expr_Rel_Concrete(Visitor_Expr_Rel_Concrete& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Concrete(this); }

RelHead Expr_Rel_Concrete::GetConcreteRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Concrete

void Visitor_Expr_Rel_Concrete::Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * sConcrete

ZRef<Expr_Rel> sConcrete(const RelHead& iRelHead)
	{ return new Expr_Rel_Concrete(iRelHead); }

} // namespace ZRA
} // namespace ZooLib
