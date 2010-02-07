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

#include "zoolib/ZExpr_Query.h"
#include "zoolib/ZExpr_ValCondition.h"

using std::string;
using std::vector;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Query

ZExprRep_Query::ZExprRep_Query()
	{}

bool ZExprRep_Query::Accept(ZVisitor_ExprRep_Relation& iVisitor)
	{
	if (ZVisitor_ExprRep_Query* theVisitor =
		dynamic_cast<ZVisitor_ExprRep_Query*>(&iVisitor))
		{
		return this->Accept(*theVisitor);
		}
	else
		{
		return ZExprRep_Relation::Accept(iVisitor);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Query_All

ZExprRep_Query_All::ZExprRep_Query_All(const string& iIDPropName)
:	fIDPropName(iIDPropName)
	{}

ZExprRep_Query_All::ZExprRep_Query_All(const ZRelHead& iRelHead)
:	fRelHead(iRelHead)
	{}

ZExprRep_Query_All::ZExprRep_Query_All(const string& iIDPropName, const ZRelHead& iRelHead)
:	fIDPropName(iIDPropName)
,	fRelHead(iRelHead)
	{}

ZExprRep_Query_All::~ZExprRep_Query_All()
	{}

ZRelHead ZExprRep_Query_All::GetRelHead()
	{
	ZRelHead result = fRelHead;
	if (!fIDPropName.empty())
		result |= fIDPropName;
	return result;
	}

bool ZExprRep_Query_All::Accept(ZVisitor_ExprRep_Query& iVisitor)
	{ return iVisitor.Visit_All(this); }

const string& ZExprRep_Query_All::GetIDPropName()
	{ return fIDPropName; }

const ZRelHead& ZExprRep_Query_All::GetAllRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Query_Explicit

ZExprRep_Query_Explicit::ZExprRep_Query_Explicit(const ZVal_Expr* iVals, size_t iCount)
:	fVals(iVals, iVals + iCount)
	{}

ZExprRep_Query_Explicit::ZExprRep_Query_Explicit(const vector<ZVal_Expr>& iVals)
:	fVals(iVals)
	{}

ZExprRep_Query_Explicit::~ZExprRep_Query_Explicit()
	{}

ZRelHead ZExprRep_Query_Explicit::GetRelHead()
	{ return ZRelHead(); }

bool ZExprRep_Query_Explicit::Accept(ZVisitor_ExprRep_Query& iVisitor)
	{ return iVisitor.Visit_Explicit(this); }

const vector<ZVal_Expr>& ZExprRep_Query_Explicit::GetVals()
	{ return fVals; }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Query

bool ZVisitor_ExprRep_Query::Visit_All(ZRef<ZExprRep_Query_All> iRep)
	{ return ZVisitor_ExprRep_Relation::Visit(iRep); }

bool ZVisitor_ExprRep_Query::Visit_Explicit(ZRef<ZExprRep_Query_Explicit> iRep)
	{ return ZVisitor_ExprRep_Relation::Visit(iRep); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Query

ZExpr_Query::ZExpr_Query()
	{}

ZExpr_Query::ZExpr_Query(const ZExpr_Query& iOther)
:	inherited(iOther)
	{}

ZExpr_Query::~ZExpr_Query()
	{}

ZExpr_Query& ZExpr_Query::operator=(const ZExpr_Query& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ZExpr_Query::ZExpr_Query(const ZRef<ZExprRep_Query>& iRep)
:	inherited(iRep)
	{}

ZExpr_Query::operator ZRef<ZExprRep_Query>() const
	{ return ZRefStaticCast<ZExprRep_Query>(*this); }

// =================================================================================================
#pragma mark -
#pragma mark * Query operators

ZExpr_Query sAll(const ZRelHead& iRelHead)
	{ return ZExpr_Query(new ZExprRep_Query_All(iRelHead)); }

ZExpr_Query sAllID(const string& iIDName)
	{ return ZExpr_Query(new ZExprRep_Query_All(iIDName)); }

ZExpr_Query sAllID(const string& iIDName, const ZRelHead& iRelHead)
	{ return ZExpr_Query(new ZExprRep_Query_All(iIDName, iRelHead)); }

ZExpr_Query sExplicit(const ZVal_Expr* iVals, size_t iCount)
	{ return ZExpr_Query(new ZExprRep_Query_Explicit(iVals, iCount)); }

ZExpr_Query sExplicit(const vector<ZVal_Expr>& iVals)
	{ return ZExpr_Query(new ZExprRep_Query_Explicit(iVals)); }

// =================================================================================================
#pragma mark -
#pragma mark * ZQueryTransformer

bool ZQueryTransformer::Visit_Difference(ZRef<ZExprRep_Relation_Difference> iRep)
	{
	fResult = this->Transform_Difference(iRep);
	return true;
	}

bool ZQueryTransformer::Visit_Intersect(ZRef<ZExprRep_Relation_Intersect> iRep)
	{
	fResult = this->Transform_Intersect(iRep);
	return true;
	}

bool ZQueryTransformer::Visit_Join(ZRef<ZExprRep_Relation_Join> iRep)
	{
	fResult = this->Transform_Join(iRep);
	return true;
	}

bool ZQueryTransformer::Visit_Project(ZRef<ZExprRep_Relation_Project> iRep)
	{
	fResult = this->Transform_Project(iRep);
	return true;
	}

bool ZQueryTransformer::Visit_Rename(ZRef<ZExprRep_Relation_Rename> iRep)
	{
	fResult = this->Transform_Rename(iRep);
	return true;
	}

bool ZQueryTransformer::Visit_Union(ZRef<ZExprRep_Relation_Union> iRep)
	{
	fResult = this->Transform_Union(iRep);
	return true;
	}

bool ZQueryTransformer::Visit_All(ZRef<ZExprRep_Query_All> iRep)
	{
	fResult = this->Transform_All(iRep);
	return true;
	}

bool ZQueryTransformer::Visit_Explicit(ZRef<ZExprRep_Query_Explicit> iRep)
	{
	fResult = this->Transform_Explicit(iRep);
	return true;
	}

bool ZQueryTransformer::Visit_Restrict(ZRef<ZExprRep_Restrict> iRep)
	{
	fResult = this->Transform_Restrict(iRep);
	return true;
	}

bool ZQueryTransformer::Visit_Select(ZRef<ZExprRep_Select> iRep)
	{
	fResult = this->Transform_Select(iRep);
	return true;
	}

// Our protocol
ZRef<ZExprRep_Relation> ZQueryTransformer::Transform(ZRef<ZExprRep_Relation> iRep)
	{
	if (iRep)
		{
		iRep->Accept(*this);
		return fResult;
		}
	return iRep;
	}

ZRef<ZExprRep_Relation> ZQueryTransformer::Transform_All(ZRef<ZExprRep_Query_All> iRep)
	{ return iRep; }

ZRef<ZExprRep_Relation> ZQueryTransformer::Transform_Difference(
	ZRef<ZExprRep_Relation_Difference> iRep)
	{
	ZRef<ZExprRep_Relation> newLHS = this->Transform(iRep->GetLHS());
	ZRef<ZExprRep_Relation> newRHS = this->Transform(iRep->GetRHS());

	return new ZExprRep_Relation_Difference(newLHS, newRHS);
	}

ZRef<ZExprRep_Relation> ZQueryTransformer::Transform_Explicit(ZRef<ZExprRep_Query_Explicit> iRep)
	{ return iRep; }

ZRef<ZExprRep_Relation> ZQueryTransformer::Transform_Intersect(
	ZRef<ZExprRep_Relation_Intersect> iRep)
	{
	ZRef<ZExprRep_Relation> newLHS = this->Transform(iRep->GetLHS());
	ZRef<ZExprRep_Relation> newRHS = this->Transform(iRep->GetRHS());

	return new ZExprRep_Relation_Intersect(newLHS, newRHS);
	}

ZRef<ZExprRep_Relation> ZQueryTransformer::Transform_Join(ZRef<ZExprRep_Relation_Join> iRep)
	{
	ZRef<ZExprRep_Relation> newLHS = this->Transform(iRep->GetLHS());
	ZRef<ZExprRep_Relation> newRHS = this->Transform(iRep->GetRHS());

	return new ZExprRep_Relation_Join(newLHS, newRHS);
	}

ZRef<ZExprRep_Relation> ZQueryTransformer::Transform_Project(
	ZRef<ZExprRep_Relation_Project> iRep)
	{
	ZRef<ZExprRep_Relation> newRep = this->Transform(iRep->GetExpr());
	return new ZExprRep_Relation_Project(newRep, iRep->GetRelHead());
	}

ZRef<ZExprRep_Relation> ZQueryTransformer::Transform_Rename(
	ZRef<ZExprRep_Relation_Rename> iRep)
	{
	ZRef<ZExprRep_Relation> newRep = this->Transform(iRep->GetExpr());
	return new ZExprRep_Relation_Rename(newRep, iRep->GetOld(), iRep->GetNew());
	}

ZRef<ZExprRep_Relation> ZQueryTransformer::Transform_Restrict(ZRef<ZExprRep_Restrict> iRep)
	{
	ZRef<ZExprRep_Relation> newRep = this->Transform(iRep->GetExpr());
	return new ZExprRep_Restrict(iRep->GetValCondition(), newRep);
	}

ZRef<ZExprRep_Relation> ZQueryTransformer::Transform_Select(ZRef<ZExprRep_Select> iRep)
	{
	ZRef<ZExprRep_Relation> newRep = this->Transform(iRep->GetExpr_Relation());
	return new ZExprRep_Select(iRep->GetExpr_Logical(), newRep);
	}

ZRef<ZExprRep_Relation> ZQueryTransformer::Transform_Union(ZRef<ZExprRep_Relation_Union> iRep)
	{
	ZRef<ZExprRep_Relation> newLHS = this->Transform(iRep->GetLHS());
	ZRef<ZExprRep_Relation> newRHS = this->Transform(iRep->GetRHS());

	return new ZExprRep_Relation_Union(newLHS, newRHS);
	}

NAMESPACE_ZOOLIB_END
