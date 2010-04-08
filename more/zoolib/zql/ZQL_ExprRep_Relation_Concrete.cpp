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

#include "zoolib/zql/ZQL_ExprRep_Relation_Concrete.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Concrete

ExprRep_Relation_Concrete::ExprRep_Relation_Concrete()
	{}

ExprRep_Relation_Concrete::~ExprRep_Relation_Concrete()
	{}

bool ExprRep_Relation_Concrete::Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor)
	{
	if (Visitor_ExprRep_Relation_Concrete* theVisitor =
		dynamic_cast<Visitor_ExprRep_Relation_Concrete*>(&iVisitor))
		{
		return this->Accept_ExprRep_Relation_Concrete(*theVisitor);
		}
	else
		{
		return ExprRep_Relation::Accept(iVisitor);
		}
	}

bool ExprRep_Relation_Concrete::Accept_ExprRep_Relation_Concrete(
	Visitor_ExprRep_Relation_Concrete& iVisitor)
	{ return iVisitor.Visit_ExprRep_Relation_Concrete(this); }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Concrete

bool Visitor_ExprRep_Relation_Concrete::Visit_ExprRep_Relation_Concrete(
	ZRef<ExprRep_Relation_Concrete> iRep)
	{ return ZVisitor_ExprRep::Visit_ExprRep(iRep); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
