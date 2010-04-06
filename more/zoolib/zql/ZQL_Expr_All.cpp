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

#include "zoolib/zql/ZQL_Expr_All.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_All

ExprRep_All::ExprRep_All(const string& iIDPropName)
:	fIDPropName(iIDPropName)
	{}

ExprRep_All::ExprRep_All(const ZRelHead& iRelHead)
:	fRelHead(iRelHead)
	{}

ExprRep_All::ExprRep_All(const string& iIDPropName, const ZRelHead& iRelHead)
:	fIDPropName(iIDPropName)
,	fRelHead(iRelHead)
	{}

ExprRep_All::~ExprRep_All()
	{}

bool ExprRep_All::Accept(Visitor_ExprRep_Relation& iVisitor)
	{
	if (Visitor_ExprRep_All* theVisitor =
		dynamic_cast<Visitor_ExprRep_All*>(&iVisitor))
		{
		return theVisitor->Visit_All(this);
		}
	else
		{
		return ExprRep_Relation::Accept(iVisitor);
		}
	}

ZRelHead ExprRep_All::GetRelHead()
	{
	ZRelHead result = fRelHead;
	if (!fIDPropName.empty())
		result |= fIDPropName;
	return result;
	}

const string& ExprRep_All::GetIDPropName()
	{ return fIDPropName; }

const ZRelHead& ExprRep_All::GetAllRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_All

bool Visitor_ExprRep_All::Visit_All(ZRef<ExprRep_All> iRep)
	{ return Visitor_ExprRep_Relation::Visit_ExprRep(iRep); }

// =================================================================================================
#pragma mark -
#pragma mark * Query operators

Expr_Relation sAll(const ZRelHead& iRelHead)
	{ return Expr_Relation(new ExprRep_All(iRelHead)); }

Expr_Relation sAllID(const string& iIDName)
	{ return Expr_Relation(new ExprRep_All(iIDName)); }

Expr_Relation sAllID(const string& iIDName, const ZRelHead& iRelHead)
	{ return Expr_Relation(new ExprRep_All(iIDName, iRelHead)); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
