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

#include "zoolib/zql/ZQL_Expr_Relation_Unary_Rename.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Relation_Unary_Rename

Expr_Relation_Unary_Rename::Expr_Relation_Unary_Rename(
	ZRef<Expr_Relation> iExpr_Relation, const std::string& iOld, const std::string& iNew)
:	Expr_Relation_Unary(iExpr_Relation)
,	fOld(iOld)
,	fNew(iNew)
	{}

Expr_Relation_Unary_Rename::~Expr_Relation_Unary_Rename()
	{}

ZRelHead Expr_Relation_Unary_Rename::GetRelHead()
	{
	ZRelHead theRelHead = this->GetExpr_Relation()->GetRelHead();
	if (theRelHead.Contains(fOld))
		{
		theRelHead -= fOld;
		theRelHead |= fNew;
		}
	return theRelHead;
	}

void Expr_Relation_Unary_Rename::Accept_Expr_Relation_Unary(
	Visitor_Expr_Relation_Unary& iVisitor)
	{
	if (Visitor_Expr_Relation_Unary_Rename* theVisitor =
		dynamic_cast<Visitor_Expr_Relation_Unary_Rename*>(&iVisitor))
		{
		this->Accept_Expr_Relation_Unary_Rename(*theVisitor);
		}
	else
		{
		Expr_Relation_Unary::Accept_Expr_Relation_Unary(iVisitor);
		}
	}

ZRef<Expr_Relation_Unary> Expr_Relation_Unary_Rename::Clone(
	ZRef<Expr_Relation> iExpr_Relation)
	{ return new Expr_Relation_Unary_Rename(iExpr_Relation, fOld, fNew); }

void Expr_Relation_Unary_Rename::Accept_Expr_Relation_Unary_Rename(
	Visitor_Expr_Relation_Unary_Rename& iVisitor)
	{ iVisitor.Visit_Expr_Relation_Unary_Rename(this); }

const string& Expr_Relation_Unary_Rename::GetOld()
	{ return fOld; }

const string& Expr_Relation_Unary_Rename::GetNew()
	{ return fNew; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Relation_Unary_Rename

void Visitor_Expr_Relation_Unary_Rename::Visit_Expr_Relation_Unary_Rename(
	ZRef<Expr_Relation_Unary_Rename> iRep)
	{ Visitor_Expr_Relation_Unary::Visit_Expr_Relation_Unary(iRep); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
