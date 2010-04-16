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

#include "zoolib/zql/ZQL_Expr_Rel_Project.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Project

Expr_Rel_Project::Expr_Rel_Project(ZRef<Expr_Rel> iOp0, const RelHead& iRelHead)
:	inherited(iOp0)
,	fRelHead(iRelHead)
	{}

Expr_Rel_Project::~Expr_Rel_Project()
	{}

void Expr_Rel_Project::Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Project* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Project*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Project(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op1(iVisitor);
		}
	}

ZRef<Expr_Rel> Expr_Rel_Project::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Project::Clone(ZRef<Expr_Rel> iOp0)
	{ return new Expr_Rel_Project(iOp0, fRelHead); }

void Expr_Rel_Project::Accept_Expr_Rel_Project(Visitor_Expr_Rel_Project& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Project(this); }

RelHead Expr_Rel_Project::GetRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Project

void Visitor_Expr_Rel_Project::Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Project> sProject(const ZRef<Expr_Rel>& iExpr, const RelHead& iRelHead)
	{ return new Expr_Rel_Project(iExpr, iRelHead); }

ZRef<Expr_Rel_Project> operator&(const ZRef<Expr_Rel>& iExpr, const RelHead& iRelHead)
	{ return new Expr_Rel_Project(iExpr, iRelHead); }

ZRef<Expr_Rel_Project> operator&(const RelHead& iRelHead, const ZRef<Expr_Rel>& iExpr)
	{ return new Expr_Rel_Project(iExpr, iRelHead); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
