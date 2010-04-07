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

#include "zoolib/zql/ZQL_Expr_Concrete.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Concrete

ExprRep_Concrete::ExprRep_Concrete()
	{}

ExprRep_Concrete::~ExprRep_Concrete()
	{}

bool ExprRep_Concrete::Accept(Visitor_ExprRep_Relation& iVisitor)
	{
	if (Visitor_ExprRep_Concrete* theVisitor =
		dynamic_cast<Visitor_ExprRep_Concrete*>(&iVisitor))
		{
		return theVisitor->Visit_Concrete(this);
		}
	else
		{
		return ExprRep_Relation::Accept(iVisitor);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Concrete

bool Visitor_ExprRep_Concrete::Visit_Concrete(ZRef<ExprRep_Concrete> iRep)
	{ return Visitor_ExprRep_Relation::Visit_ExprRep(iRep); }

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Concrete

Expr_Concrete::Expr_Concrete()
	{}

Expr_Concrete::Expr_Concrete(const Expr_Concrete& iOther)
:	inherited(iOther)
	{}

Expr_Concrete::~Expr_Concrete()
	{}

Expr_Concrete& Expr_Concrete::operator=(const Expr_Concrete& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

Expr_Concrete::Expr_Concrete(const ZRef<ExprRep_Concrete>& iRep)
:	inherited(iRep)
	{}

Expr_Concrete::operator ZRef<ExprRep_Concrete>() const
	{ return this->StaticCast<ExprRep_Concrete>(); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
