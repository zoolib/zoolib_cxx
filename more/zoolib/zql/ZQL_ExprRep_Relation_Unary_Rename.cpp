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

#include "zoolib/zql/ZQL_ExprRep_Relation_Unary_Rename.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Unary_Rename

ExprRep_Relation_Unary_Rename::ExprRep_Relation_Unary_Rename(
	ZRef<ExprRep_Relation> iExprRep_Relation, const std::string& iOld, const std::string& iNew)
:	ExprRep_Relation_Unary(iExprRep_Relation)
,	fOld(iOld)
,	fNew(iNew)
	{}

ExprRep_Relation_Unary_Rename::~ExprRep_Relation_Unary_Rename()
	{}

ZRelHead ExprRep_Relation_Unary_Rename::GetRelHead()
	{
	ZRelHead theRelHead = this->GetExprRep_Relation()->GetRelHead();
	if (theRelHead.Contains(fOld))
		{
		theRelHead -= fOld;
		theRelHead |= fNew;
		}
	return theRelHead;
	}

void ExprRep_Relation_Unary_Rename::Accept_ExprRep_Relation_Unary(
	Visitor_ExprRep_Relation_Unary& iVisitor)
	{
	if (Visitor_ExprRep_Relation_Unary_Rename* theVisitor =
		dynamic_cast<Visitor_ExprRep_Relation_Unary_Rename*>(&iVisitor))
		{
		this->Accept_ExprRep_Relation_Unary_Rename(*theVisitor);
		}
	else
		{
		ExprRep_Relation_Unary::Accept_ExprRep_Relation_Unary(iVisitor);
		}
	}

ZRef<ExprRep_Relation_Unary> ExprRep_Relation_Unary_Rename::Clone(
	ZRef<ExprRep_Relation> iExprRep_Relation)
	{ return new ExprRep_Relation_Unary_Rename(iExprRep_Relation, fOld, fNew); }

void ExprRep_Relation_Unary_Rename::Accept_ExprRep_Relation_Unary_Rename(
	Visitor_ExprRep_Relation_Unary_Rename& iVisitor)
	{ iVisitor.Visit_ExprRep_Relation_Unary_Rename(this); }

const string& ExprRep_Relation_Unary_Rename::GetOld()
	{ return fOld; }

const string& ExprRep_Relation_Unary_Rename::GetNew()
	{ return fNew; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Unary_Rename

void Visitor_ExprRep_Relation_Unary_Rename::Visit_ExprRep_Relation_Unary_Rename(
	ZRef<ExprRep_Relation_Unary_Rename> iRep)
	{ Visitor_ExprRep_Relation_Unary::Visit_ExprRep_Relation_Unary(iRep); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
