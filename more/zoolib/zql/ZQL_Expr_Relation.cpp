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

#include "zoolib/zql/ZQL_Expr_Relation.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation

ExprRep_Relation::ExprRep_Relation()
	{}

bool ExprRep_Relation::Accept(ZVisitor_ExprRep& iVisitor)
	{
	if (Visitor_ExprRep_Relation* theVisitor =
		dynamic_cast<Visitor_ExprRep_Relation*>(&iVisitor))
		{
		return this->Accept(*theVisitor);
		}
	else
		{
		return ZExprRep::Accept(iVisitor);
		}
	}

bool ExprRep_Relation::Accept(Visitor_ExprRep_Relation& iVisitor)
	{ return ZExprRep::Accept(iVisitor); }

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Dyadic

ExprRep_Relation_Dyadic::ExprRep_Relation_Dyadic(
	ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS)
:	fLHS(iLHS)
,	fRHS(iRHS)
	{}

ExprRep_Relation_Dyadic::~ExprRep_Relation_Dyadic()
	{}

ZRef<ExprRep_Relation> ExprRep_Relation_Dyadic::GetLHS()
	{ return fLHS; }

ZRef<ExprRep_Relation> ExprRep_Relation_Dyadic::GetRHS()
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Difference

ExprRep_Relation_Difference::ExprRep_Relation_Difference(
	ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS)
:	ExprRep_Relation_Dyadic(iLHS, iRHS)
	{}

bool ExprRep_Relation_Difference::Accept(Visitor_ExprRep_Relation& iVisitor)
	{ return iVisitor.Visit_Difference(this); }

ZRelHead ExprRep_Relation_Difference::GetRelHead()
	{
	ZRelHead theRelHeadA = fLHS->GetRelHead();
	ZRelHead theRelHeadB = fRHS->GetRelHead();

	if (theRelHeadA != theRelHeadB)
		return ZRelHead();

	return theRelHeadA;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Intersect

ExprRep_Relation_Intersect::ExprRep_Relation_Intersect(
	ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS)
:	ExprRep_Relation_Dyadic(iLHS, iRHS)
	{}

bool ExprRep_Relation_Intersect::Accept(Visitor_ExprRep_Relation& iVisitor)
	{ return iVisitor.Visit_Intersect(this); }

ZRelHead ExprRep_Relation_Intersect::GetRelHead()
	{
	ZRelHead theRelHeadA = fLHS->GetRelHead();
	ZRelHead theRelHeadB = fRHS->GetRelHead();

	if (theRelHeadA != theRelHeadB)
		return ZRelHead();

	return theRelHeadA;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Join

ExprRep_Relation_Join::ExprRep_Relation_Join(
	ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS)
:	ExprRep_Relation_Dyadic(iLHS, iRHS)
	{}

bool ExprRep_Relation_Join::Accept(Visitor_ExprRep_Relation& iVisitor)
	{ return iVisitor.Visit_Join(this); }

ZRelHead ExprRep_Relation_Join::GetRelHead()
	{ return fLHS->GetRelHead() | fRHS->GetRelHead(); }

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Project

ExprRep_Relation_Project::ExprRep_Relation_Project(
	ZRef<ExprRep_Relation> iExprRep, const ZRelHead& iRelHead)
:	fExprRep(iExprRep)
,	fRelHead(iRelHead)
	{
	ZAssert(iExprRep);
	}

ExprRep_Relation_Project::~ExprRep_Relation_Project()
	{}

bool ExprRep_Relation_Project::Accept(Visitor_ExprRep_Relation& iVisitor)
	{ return iVisitor.Visit_Project(this); }

ZRelHead ExprRep_Relation_Project::GetRelHead()
	{ return fExprRep->GetRelHead() & fRelHead; }

ZRef<ExprRep_Relation> ExprRep_Relation_Project::GetExprRep()
	{ return fExprRep; }

const ZRelHead& ExprRep_Relation_Project::GetProjectRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Rename

ExprRep_Relation_Rename::ExprRep_Relation_Rename(
	ZRef<ExprRep_Relation> iExprRep, const string& iOld, const string& iNew)
:	fExprRep(iExprRep)
,	fOld(iOld)
,	fNew(iNew)
	{}

ExprRep_Relation_Rename::~ExprRep_Relation_Rename()
	{}

bool ExprRep_Relation_Rename::Accept(Visitor_ExprRep_Relation& iVisitor)
	{ return iVisitor.Visit_Rename(this); }

ZRelHead ExprRep_Relation_Rename::GetRelHead()
	{
	ZRelHead theRelHead = fExprRep->GetRelHead();
	if (theRelHead.Contains(fOld))
		{
		theRelHead -= fOld;
		theRelHead |= fNew;
		}
	return theRelHead;
	}

ZRef<ExprRep_Relation> ExprRep_Relation_Rename::GetExprRep()
	{ return fExprRep; }
	
const string& ExprRep_Relation_Rename::GetOld()
	{ return fOld; }

const string& ExprRep_Relation_Rename::GetNew()
	{ return fNew; }

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Union

ExprRep_Relation_Union::ExprRep_Relation_Union(
	ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS)
:	ExprRep_Relation_Dyadic(iLHS, iRHS)
	{}

bool ExprRep_Relation_Union::Accept(Visitor_ExprRep_Relation& iVisitor)
	{ return iVisitor.Visit_Union(this); }

ZRelHead ExprRep_Relation_Union::GetRelHead()
	{
	ZRelHead theRelHeadA = fLHS->GetRelHead();
	ZRelHead theRelHeadB = fRHS->GetRelHead();

	if (theRelHeadA != theRelHeadB)
		return ZRelHead();

	return theRelHeadA;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation

bool Visitor_ExprRep_Relation::Visit_Difference(ZRef<ExprRep_Relation_Difference> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ExprRep_Relation> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ExprRep_Relation> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

bool Visitor_ExprRep_Relation::Visit_Intersect(ZRef<ExprRep_Relation_Intersect> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ExprRep_Relation> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ExprRep_Relation> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

bool Visitor_ExprRep_Relation::Visit_Join(ZRef<ExprRep_Relation_Join> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ExprRep_Relation> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ExprRep_Relation> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

bool Visitor_ExprRep_Relation::Visit_Project(ZRef<ExprRep_Relation_Project> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ExprRep_Relation> theExprRep = iRep->GetExprRep())
		{
		if (!theExprRep->Accept(*this))
			return false;
		}

	return true;
	}

bool Visitor_ExprRep_Relation::Visit_Rename(ZRef<ExprRep_Relation_Rename> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ExprRep_Relation> theExprRep = iRep->GetExprRep())
		{
		if (!theExprRep->Accept(*this))
			return false;
		}

	return true;
	}

bool Visitor_ExprRep_Relation::Visit_Union(ZRef<ExprRep_Relation_Union> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ExprRep_Relation> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ExprRep_Relation> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Relation

Expr_Relation::Expr_Relation()
	{}

Expr_Relation::Expr_Relation(const Expr_Relation& iOther)
:	inherited(iOther)
	{}

Expr_Relation::~Expr_Relation()
	{}

Expr_Relation& Expr_Relation::operator=(const Expr_Relation& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

Expr_Relation::Expr_Relation(const ZRef<ExprRep_Relation>& iRep)
:	inherited(iRep)
	{}

Expr_Relation::operator ZRef<ExprRep_Relation>() const
	{ return this->StaticCast<ExprRep_Relation>(); }

ZRelHead Expr_Relation::GetRelHead() const
	{
	if (ZRef<ExprRep_Relation> theRep = *this)
		return theRep->GetRelHead();
	return ZRelHead();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

Expr_Relation sIntersect(const Expr_Relation& iLHS, const Expr_Relation& iRHS)
	{ return Expr_Relation(new ExprRep_Relation_Intersect(iLHS, iRHS)); }

Expr_Relation sJoin(const Expr_Relation& iLHS, const Expr_Relation& iRHS)
	{ return Expr_Relation(new ExprRep_Relation_Join(iLHS, iRHS)); }

Expr_Relation sProject(const ZRelHead& iRelHead, const Expr_Relation& iExpr)
	{ return Expr_Relation(new ExprRep_Relation_Project(iExpr, iRelHead)); }

Expr_Relation sRename(const string& iOldPropName, const string& iNewPropName,
	const Expr_Relation& iExpr)
	{ return Expr_Relation(new ExprRep_Relation_Rename(iExpr, iOldPropName, iNewPropName)); }

Expr_Relation sUnion(const Expr_Relation& iLHS, const Expr_Relation& iRHS)
	{ return Expr_Relation(new ExprRep_Relation_Union(iLHS, iRHS)); }

Expr_Relation operator&(const Expr_Relation& iLHS, const Expr_Relation& iRHS)
	{ return Expr_Relation(new ExprRep_Relation_Intersect(iLHS, iRHS)); }

Expr_Relation operator*(const Expr_Relation& iLHS, const Expr_Relation& iRHS)
	{ return Expr_Relation(new ExprRep_Relation_Join(iLHS, iRHS)); }

Expr_Relation operator|(const Expr_Relation& iLHS, const Expr_Relation& iRHS)
	{ return Expr_Relation(new ExprRep_Relation_Union(iLHS, iRHS)); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
