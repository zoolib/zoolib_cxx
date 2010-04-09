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

#include "zoolib/zql/ZQL_ExprRep_Relation_Binary_Join.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Binary_Join

ExprRep_Relation_Binary_Join::ExprRep_Relation_Binary_Join(
	ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS)
:	ExprRep_Relation_Binary(iLHS, iRHS)
	{}

ZRelHead ExprRep_Relation_Binary_Join::GetRelHead()
	{ return this->GetLHS()->GetRelHead() | this->GetRHS()->GetRelHead(); }

void ExprRep_Relation_Binary_Join::Accept_ExprRep_Relation_Binary(
	Visitor_ExprRep_Relation_Binary& iVisitor)
	{
	if (Visitor_ExprRep_Relation_Binary_Join* theVisitor =
		dynamic_cast<Visitor_ExprRep_Relation_Binary_Join*>(&iVisitor))
		{
		this->Accept_ExprRep_Relation_Binary_Join(*theVisitor);
		}
	else
		{
		ExprRep_Relation_Binary::Accept_ExprRep_Relation_Binary(iVisitor);
		}
	}

void ExprRep_Relation_Binary_Join::Accept_ExprRep_Relation_Binary_Join(
	Visitor_ExprRep_Relation_Binary_Join& iVisitor)
	{ iVisitor.Visit_ExprRep_Relation_Binary_Join(this); }

ZRef<ExprRep_Relation_Binary> ExprRep_Relation_Binary_Join::Clone(
	ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS)
	{ return new ExprRep_Relation_Binary_Join(iLHS, iRHS); }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Binary_Join

void Visitor_ExprRep_Relation_Binary_Join::Visit_ExprRep_Relation_Binary_Join(
	ZRef<ExprRep_Relation_Binary_Join> iRep)
	{ Visitor_ExprRep_Relation_Binary::Visit_ExprRep_Relation_Binary(iRep); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
