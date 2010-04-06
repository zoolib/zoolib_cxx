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

#include "zoolib/ZExpr_ValCondition.h"
#include "zoolib/zql/ZQL_Expr_Query.h"

using std::string;
using std::vector;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Query

ExprRep_Query::ExprRep_Query()
	{}

bool ExprRep_Query::Accept(Visitor_ExprRep_Relation& iVisitor)
	{
	if (Visitor_ExprRep_Query* theVisitor =
		dynamic_cast<Visitor_ExprRep_Query*>(&iVisitor))
		{
		return this->Accept(*theVisitor);
		}
	else
		{
		return ExprRep_Relation::Accept(iVisitor);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Query_All

ExprRep_Query_All::ExprRep_Query_All(const string& iIDPropName)
:	fIDPropName(iIDPropName)
	{}

ExprRep_Query_All::ExprRep_Query_All(const ZRelHead& iRelHead)
:	fRelHead(iRelHead)
	{}

ExprRep_Query_All::ExprRep_Query_All(const string& iIDPropName, const ZRelHead& iRelHead)
:	fIDPropName(iIDPropName)
,	fRelHead(iRelHead)
	{}

ExprRep_Query_All::~ExprRep_Query_All()
	{}

ZRelHead ExprRep_Query_All::GetRelHead()
	{
	ZRelHead result = fRelHead;
	if (!fIDPropName.empty())
		result |= fIDPropName;
	return result;
	}

bool ExprRep_Query_All::Accept(Visitor_ExprRep_Query& iVisitor)
	{ return iVisitor.Visit_All(this); }

const string& ExprRep_Query_All::GetIDPropName()
	{ return fIDPropName; }

const ZRelHead& ExprRep_Query_All::GetAllRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Query_Explicit

ExprRep_Query_Explicit::ExprRep_Query_Explicit(const ZVal_Expr* iVals, size_t iCount)
:	fVals(iVals, iVals + iCount)
	{}

ExprRep_Query_Explicit::ExprRep_Query_Explicit(const vector<ZVal_Expr>& iVals)
:	fVals(iVals)
	{}

ExprRep_Query_Explicit::~ExprRep_Query_Explicit()
	{}

ZRelHead ExprRep_Query_Explicit::GetRelHead()
	{ return ZRelHead(); }

bool ExprRep_Query_Explicit::Accept(Visitor_ExprRep_Query& iVisitor)
	{ return iVisitor.Visit_Explicit(this); }

const vector<ZVal_Expr>& ExprRep_Query_Explicit::GetVals()
	{ return fVals; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Query

bool Visitor_ExprRep_Query::Visit_All(ZRef<ExprRep_Query_All> iRep)
	{ return Visitor_ExprRep_Relation::Visit_ExprRep(iRep); }

bool Visitor_ExprRep_Query::Visit_Explicit(ZRef<ExprRep_Query_Explicit> iRep)
	{ return Visitor_ExprRep_Relation::Visit_ExprRep(iRep); }

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Query

Expr_Query::Expr_Query()
	{}

Expr_Query::Expr_Query(const Expr_Query& iOther)
:	inherited(iOther)
	{}

Expr_Query::~Expr_Query()
	{}

Expr_Query& Expr_Query::operator=(const Expr_Query& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

Expr_Query::Expr_Query(const ZRef<ExprRep_Query>& iRep)
:	inherited(iRep)
	{}

Expr_Query::operator ZRef<ExprRep_Query>() const
	{ return this->StaticCast<ExprRep_Query>(); }

// =================================================================================================
#pragma mark -
#pragma mark * Query operators

Expr_Query sAll(const ZRelHead& iRelHead)
	{ return Expr_Query(new ExprRep_Query_All(iRelHead)); }

Expr_Query sAllID(const string& iIDName)
	{ return Expr_Query(new ExprRep_Query_All(iIDName)); }

Expr_Query sAllID(const string& iIDName, const ZRelHead& iRelHead)
	{ return Expr_Query(new ExprRep_Query_All(iIDName, iRelHead)); }

Expr_Query sExplicit(const ZVal_Expr* iVals, size_t iCount)
	{ return Expr_Query(new ExprRep_Query_Explicit(iVals, iCount)); }

Expr_Query sExplicit(const vector<ZVal_Expr>& iVals)
	{ return Expr_Query(new ExprRep_Query_Explicit(iVals)); }

// =================================================================================================
#pragma mark -
#pragma mark * QueryTransformer

bool QueryTransformer::Visit_Difference(ZRef<ExprRep_Relation_Difference> iRep)
	{
	fResult = this->Transform_Difference(iRep);
	return true;
	}

bool QueryTransformer::Visit_Intersect(ZRef<ExprRep_Relation_Intersect> iRep)
	{
	fResult = this->Transform_Intersect(iRep);
	return true;
	}

bool QueryTransformer::Visit_Join(ZRef<ExprRep_Relation_Join> iRep)
	{
	fResult = this->Transform_Join(iRep);
	return true;
	}

bool QueryTransformer::Visit_Project(ZRef<ExprRep_Relation_Project> iRep)
	{
	fResult = this->Transform_Project(iRep);
	return true;
	}

bool QueryTransformer::Visit_Rename(ZRef<ExprRep_Relation_Rename> iRep)
	{
	fResult = this->Transform_Rename(iRep);
	return true;
	}

bool QueryTransformer::Visit_Union(ZRef<ExprRep_Relation_Union> iRep)
	{
	fResult = this->Transform_Union(iRep);
	return true;
	}

bool QueryTransformer::Visit_All(ZRef<ExprRep_Query_All> iRep)
	{
	fResult = this->Transform_All(iRep);
	return true;
	}

bool QueryTransformer::Visit_Explicit(ZRef<ExprRep_Query_Explicit> iRep)
	{
	fResult = this->Transform_Explicit(iRep);
	return true;
	}

bool QueryTransformer::Visit_Restrict(ZRef<ExprRep_Restrict> iRep)
	{
	fResult = this->Transform_Restrict(iRep);
	return true;
	}

bool QueryTransformer::Visit_Select(ZRef<ExprRep_Select> iRep)
	{
	fResult = this->Transform_Select(iRep);
	return true;
	}

ZRef<ExprRep_Relation> QueryTransformer::Transform(ZRef<ExprRep_Relation> iRep)
	{
	if (iRep)
		{
		iRep->Accept(*this);
		return fResult;
		}
	return iRep;
	}

ZRef<ExprRep_Relation> QueryTransformer::Transform_All(ZRef<ExprRep_Query_All> iRep)
	{ return iRep; }

ZRef<ExprRep_Relation> QueryTransformer::Transform_Difference(
	ZRef<ExprRep_Relation_Difference> iRep)
	{
	ZRef<ExprRep_Relation> oldLHS = iRep->GetLHS();
	ZRef<ExprRep_Relation> oldRHS = iRep->GetRHS();
	ZRef<ExprRep_Relation> newLHS = this->Transform(oldLHS);
	ZRef<ExprRep_Relation> newRHS = this->Transform(oldRHS);
	if (oldLHS == newLHS && oldRHS == newRHS)
		return iRep;
	return new ExprRep_Relation_Difference(newLHS, newRHS);
	}

ZRef<ExprRep_Relation> QueryTransformer::Transform_Explicit(ZRef<ExprRep_Query_Explicit> iRep)
	{ return iRep; }

ZRef<ExprRep_Relation> QueryTransformer::Transform_Intersect(
	ZRef<ExprRep_Relation_Intersect> iRep)
	{
	ZRef<ExprRep_Relation> oldLHS = iRep->GetLHS();
	ZRef<ExprRep_Relation> oldRHS = iRep->GetRHS();
	ZRef<ExprRep_Relation> newLHS = this->Transform(oldLHS);
	ZRef<ExprRep_Relation> newRHS = this->Transform(oldRHS);
	if (oldLHS == newLHS && oldRHS == newRHS)
		return iRep;
	return new ExprRep_Relation_Intersect(newLHS, newRHS);
	}

ZRef<ExprRep_Relation> QueryTransformer::Transform_Join(ZRef<ExprRep_Relation_Join> iRep)
	{
	ZRef<ExprRep_Relation> oldLHS = iRep->GetLHS();
	ZRef<ExprRep_Relation> oldRHS = iRep->GetRHS();
	ZRef<ExprRep_Relation> newLHS = this->Transform(oldLHS);
	ZRef<ExprRep_Relation> newRHS = this->Transform(oldRHS);
	if (oldLHS == newLHS && oldRHS == newRHS)
		return iRep;
	return new ExprRep_Relation_Join(newLHS, newRHS);
	}

ZRef<ExprRep_Relation> QueryTransformer::Transform_Project(
	ZRef<ExprRep_Relation_Project> iRep)
	{
	ZRef<ExprRep_Relation> oldRep = iRep->GetExprRep();
	ZRef<ExprRep_Relation> newRep = this->Transform(oldRep);
	if (oldRep == newRep)
		return iRep;
	return new ExprRep_Relation_Project(newRep, iRep->GetRelHead());
	}

ZRef<ExprRep_Relation> QueryTransformer::Transform_Rename(
	ZRef<ExprRep_Relation_Rename> iRep)
	{
	ZRef<ExprRep_Relation> oldRep = iRep->GetExprRep();
	ZRef<ExprRep_Relation> newRep = this->Transform(oldRep);
	if (oldRep == newRep)
		return iRep;
	return new ExprRep_Relation_Rename(newRep, iRep->GetOld(), iRep->GetNew());
	}

ZRef<ExprRep_Relation> QueryTransformer::Transform_Restrict(ZRef<ExprRep_Restrict> iRep)
	{
	ZRef<ExprRep_Relation> oldRep = iRep->GetExprRep();
	ZRef<ExprRep_Relation> newRep = this->Transform(oldRep);
	if (oldRep == newRep)
		return iRep;
	return new ExprRep_Restrict(iRep->GetValCondition(), newRep);
	}

ZRef<ExprRep_Relation> QueryTransformer::Transform_Select(ZRef<ExprRep_Select> iRep)
	{
	ZRef<ExprRep_Relation> oldRep = iRep->GetExprRep_Relation();
	ZRef<ExprRep_Relation> newRep = this->Transform(oldRep);
	if (oldRep == newRep)
		return iRep;
	return new ExprRep_Select(iRep->GetExprRep_Logical(), newRep);
	}

ZRef<ExprRep_Relation> QueryTransformer::Transform_Union(ZRef<ExprRep_Relation_Union> iRep)
	{
	ZRef<ExprRep_Relation> oldLHS = iRep->GetLHS();
	ZRef<ExprRep_Relation> oldRHS = iRep->GetRHS();
	ZRef<ExprRep_Relation> newLHS = this->Transform(oldLHS);
	ZRef<ExprRep_Relation> newRHS = this->Transform(oldRHS);
	if (oldLHS == newLHS && oldRHS == newRHS)
		return iRep;
	return new ExprRep_Relation_Union(newLHS, newRHS);
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
