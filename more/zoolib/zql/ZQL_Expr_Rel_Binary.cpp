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

#include "zoolib/zql/ZQL_Expr_Rel_Binary.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Binary

Expr_Rel_Binary::Expr_Rel_Binary(
	ZRef<Expr_Rel> iLHS, ZRef<Expr_Rel> iRHS)
:	fLHS(iLHS)
,	fRHS(iRHS)
	{}

Expr_Rel_Binary::~Expr_Rel_Binary()
	{}

void Expr_Rel_Binary::Accept_Expr_Rel(Visitor_Expr_Rel& iVisitor)
	{
	if (Visitor_Expr_Rel_Binary* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Binary*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Binary(*theVisitor);
		}
	else
		{
		Expr_Rel::Accept_Expr_Rel(iVisitor);
		}
	}

void Expr_Rel_Binary::Accept_Expr_Rel_Binary(
	Visitor_Expr_Rel_Binary& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Binary(this); }

ZRef<Expr_Rel> Expr_Rel_Binary::GetLHS()
	{ return fLHS; }

ZRef<Expr_Rel> Expr_Rel_Binary::GetRHS()
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Binary

void Visitor_Expr_Rel_Binary::Visit_Expr_Rel_Binary(
	ZRef<Expr_Rel_Binary> iRep)
	{
	if (ZRef<Expr_Rel> theRelation = iRep->GetLHS())
		theRelation->Accept(*this);

	if (ZRef<Expr_Rel> theRelation = iRep->GetRHS())
		theRelation->Accept(*this);
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
