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

#include "zoolib/zql/ZQL_ExprRep_Relation_Unary_Project.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Unary_Project

ExprRep_Relation_Unary_Project::ExprRep_Relation_Unary_Project(
		const ZRef<ExprRep_Relation>& iExprRep_Relation, const ZRelHead& iRelHead)
:	ExprRep_Relation_Unary(iExprRep_Relation)
,	fRelHead(iRelHead)
	{}

ExprRep_Relation_Unary_Project::~ExprRep_Relation_Unary_Project()
	{}

ZRelHead ExprRep_Relation_Unary_Project::GetRelHead()
	{ return this->GetExprRep_Relation()->GetRelHead() & fRelHead; }

bool ExprRep_Relation_Unary_Project::Accept_ExprRep_Relation_Unary(
	Visitor_ExprRep_Relation_Unary& iVisitor)
	{
	if (Visitor_ExprRep_Relation_Unary_Project* theVisitor =
		dynamic_cast<Visitor_ExprRep_Relation_Unary_Project*>(&iVisitor))
		{
		return this->Accept_ExprRep_Relation_Unary_Project(*theVisitor);
		}
	else
		{
		return ExprRep_Relation_Unary::Accept_ExprRep_Relation_Unary(iVisitor);
		}
	}

ZRef<ExprRep_Relation_Unary> ExprRep_Relation_Unary_Project::Clone(
	ZRef<ExprRep_Relation> iExprRep_Relation)
	{ return new ExprRep_Relation_Unary_Project(iExprRep_Relation, fRelHead); }

bool ExprRep_Relation_Unary_Project::Accept_ExprRep_Relation_Unary_Project(
	Visitor_ExprRep_Relation_Unary_Project& iVisitor)
	{ return iVisitor.Visit_ExprRep_Relation_Unary_Project(this); }

ZRelHead ExprRep_Relation_Unary_Project::GetProjectRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Unary_Project

bool Visitor_ExprRep_Relation_Unary_Project::Visit_ExprRep_Relation_Unary_Project(
	ZRef<ExprRep_Relation_Unary_Project> iRep)
	{ return Visitor_ExprRep_Relation_Unary::Visit_ExprRep_Relation_Unary(iRep); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
