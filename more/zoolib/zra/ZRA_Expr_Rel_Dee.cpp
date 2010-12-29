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

#include "zoolib/zra/ZRA_Expr_Rel_Dee.h"

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Dee

Expr_Rel_Dee::Expr_Rel_Dee()
	{}

Expr_Rel_Dee::~Expr_Rel_Dee()
	{}

void Expr_Rel_Dee::Accept(ZVisitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Dee* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Dee*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Dee(*theVisitor);
		}
	else
		{
		inherited::Accept(iVisitor);
		}
	}

void Expr_Rel_Dee::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Dee* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Dee*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Dee(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op0(iVisitor);
		}
	}

ZRef<Expr_Rel> Expr_Rel_Dee::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Dee::Clone()
	{ return this; }

void Expr_Rel_Dee::Accept_Expr_Rel_Dee(Visitor_Expr_Rel_Dee& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Dee(this); }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Dee

void Visitor_Expr_Rel_Dee::Visit_Expr_Rel_Dee(const ZRef<Expr_Rel_Dee>& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel> sDee()
	{ return new Expr_Rel_Dee; }

} // namespace ZRA
} // namespace ZooLib
