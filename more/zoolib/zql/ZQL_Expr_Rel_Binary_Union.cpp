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

#include "zoolib/zql/ZQL_Expr_Rel_Binary_Union.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Binary_Union

Expr_Rel_Binary_Union::Expr_Rel_Binary_Union(
	ZRef<Expr_Rel> iLHS, ZRef<Expr_Rel> iRHS)
:	Expr_Rel_Binary(iLHS, iRHS)
	{}

Expr_Rel_Binary_Union::~Expr_Rel_Binary_Union()
	{}

ZRelHead Expr_Rel_Binary_Union::GetRelHead()
	{ return this->GetLHS()->GetRelHead() | this->GetRHS()->GetRelHead(); }

void Expr_Rel_Binary_Union::Accept_Expr_Rel_Binary(
	Visitor_Expr_Rel_Binary& iVisitor)
	{
	if (Visitor_Expr_Rel_Binary_Union* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Binary_Union*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Binary_Union(*theVisitor);
		}
	else
		{
		Expr_Rel_Binary::Accept_Expr_Rel_Binary(iVisitor);
		}
	}

void Expr_Rel_Binary_Union::Accept_Expr_Rel_Binary_Union(
	Visitor_Expr_Rel_Binary_Union& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Binary_Union(this); }

ZRef<Expr_Rel_Binary> Expr_Rel_Binary_Union::Clone(
	ZRef<Expr_Rel> iLHS, ZRef<Expr_Rel> iRHS)
	{ return new Expr_Rel_Binary_Union(iLHS, iRHS); }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Binary_Union

void Visitor_Expr_Rel_Binary_Union::Visit_Expr_Rel_Binary_Union(
	ZRef<Expr_Rel_Binary_Union> iRep)
	{ Visitor_Expr_Rel_Binary::Visit_Expr_Rel_Binary(iRep); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Binary_Union> sUnion(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS)
	{ return new Expr_Rel_Binary_Union(iLHS, iRHS); }

ZRef<Expr_Rel_Binary_Union> operator|(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS)
	{ return new Expr_Rel_Binary_Union(iLHS, iRHS); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
