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

#include "zoolib/ZExpr_Relation.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation

ZExprRep_Relation::ZExprRep_Relation()
	{}

bool ZExprRep_Relation::Accept(ZVisitor_ExprRep& iVisitor)
	{
	if (ZVisitor_ExprRep_Relation* theVisitor =
		dynamic_cast<ZVisitor_ExprRep_Relation*>(&iVisitor))
		{
		return this->Accept(*theVisitor);
		}
	else
		{
		return ZExprRep::Accept(iVisitor);
		}
	}

bool ZExprRep_Relation::Accept(ZVisitor_ExprRep_Relation& iVisitor)
	{ return ZExprRep::Accept(iVisitor); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Dyadic

ZExprRep_Relation_Dyadic::ZExprRep_Relation_Dyadic(
	ZRef<ZExprRep_Relation> iLHS, ZRef<ZExprRep_Relation> iRHS)
:	fLHS(iLHS)
,	fRHS(iRHS)
	{}

ZExprRep_Relation_Dyadic::~ZExprRep_Relation_Dyadic()
	{}

ZRef<ZExprRep_Relation> ZExprRep_Relation_Dyadic::GetLHS()
	{ return fLHS; }

ZRef<ZExprRep_Relation> ZExprRep_Relation_Dyadic::GetRHS()
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Difference

ZExprRep_Relation_Difference::ZExprRep_Relation_Difference(
	ZRef<ZExprRep_Relation> iLHS, ZRef<ZExprRep_Relation> iRHS)
:	ZExprRep_Relation_Dyadic(iLHS, iRHS)
	{}

bool ZExprRep_Relation_Difference::Accept(ZVisitor_ExprRep_Relation& iVisitor)
	{ return iVisitor.Visit_Difference(this); }

ZRelHead ZExprRep_Relation_Difference::GetRelHead()
	{
	ZRelHead theRelHeadA = fLHS->GetRelHead();
	ZRelHead theRelHeadB = fRHS->GetRelHead();

	if (theRelHeadA != theRelHeadB)
		return ZRelHead();

	return theRelHeadA;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Intersect

ZExprRep_Relation_Intersect::ZExprRep_Relation_Intersect(
	ZRef<ZExprRep_Relation> iLHS, ZRef<ZExprRep_Relation> iRHS)
:	ZExprRep_Relation_Dyadic(iLHS, iRHS)
	{}

bool ZExprRep_Relation_Intersect::Accept(ZVisitor_ExprRep_Relation& iVisitor)
	{ return iVisitor.Visit_Intersect(this); }

ZRelHead ZExprRep_Relation_Intersect::GetRelHead()
	{
	ZRelHead theRelHeadA = fLHS->GetRelHead();
	ZRelHead theRelHeadB = fRHS->GetRelHead();

	if (theRelHeadA != theRelHeadB)
		return ZRelHead();

	return theRelHeadA;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Join

ZExprRep_Relation_Join::ZExprRep_Relation_Join(
	ZRef<ZExprRep_Relation> iLHS, ZRef<ZExprRep_Relation> iRHS)
:	ZExprRep_Relation_Dyadic(iLHS, iRHS)
	{}

bool ZExprRep_Relation_Join::Accept(ZVisitor_ExprRep_Relation& iVisitor)
	{ return iVisitor.Visit_Join(this); }

ZRelHead ZExprRep_Relation_Join::GetRelHead()
	{ return fLHS->GetRelHead() | fRHS->GetRelHead(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Project

ZExprRep_Relation_Project::ZExprRep_Relation_Project(
	ZRef<ZExprRep_Relation> iExpr, const ZRelHead& iRelHead)
:	fExpr(iExpr)
,	fRelHead(iRelHead)
	{
	ZAssert(iExpr);
	}

ZExprRep_Relation_Project::~ZExprRep_Relation_Project()
	{}

bool ZExprRep_Relation_Project::Accept(ZVisitor_ExprRep_Relation& iVisitor)
	{ return iVisitor.Visit_Project(this); }

ZRelHead ZExprRep_Relation_Project::GetRelHead()
	{ return fExpr->GetRelHead() & fRelHead; }

ZRef<ZExprRep_Relation> ZExprRep_Relation_Project::GetExpr()
	{ return fExpr; }

const ZRelHead& ZExprRep_Relation_Project::GetProjectRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Rename

ZExprRep_Relation_Rename::ZExprRep_Relation_Rename(
	ZRef<ZExprRep_Relation> iExpr, const string& iOld, const string& iNew)
:	fExpr(iExpr)
,	fOld(iOld)
,	fNew(iNew)
	{}

ZExprRep_Relation_Rename::~ZExprRep_Relation_Rename()
	{}

bool ZExprRep_Relation_Rename::Accept(ZVisitor_ExprRep_Relation& iVisitor)
	{ return iVisitor.Visit_Rename(this); }

ZRelHead ZExprRep_Relation_Rename::GetRelHead()
	{
	ZRelHead theRelHead = fExpr->GetRelHead();
	if (theRelHead.Contains(fOld))
		{
		theRelHead -= fOld;
		theRelHead |= fNew;
		}
	return theRelHead;
	}

ZRef<ZExprRep_Relation> ZExprRep_Relation_Rename::GetExpr()
	{ return fExpr; }
	
const string& ZExprRep_Relation_Rename::GetOld()
	{ return fOld; }

const string& ZExprRep_Relation_Rename::GetNew()
	{ return fNew; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relation_Union

ZExprRep_Relation_Union::ZExprRep_Relation_Union(
	ZRef<ZExprRep_Relation> iLHS, ZRef<ZExprRep_Relation> iRHS)
:	ZExprRep_Relation_Dyadic(iLHS, iRHS)
	{}

bool ZExprRep_Relation_Union::Accept(ZVisitor_ExprRep_Relation& iVisitor)
	{ return iVisitor.Visit_Union(this); }

ZRelHead ZExprRep_Relation_Union::GetRelHead()
	{
	ZRelHead theRelHeadA = fLHS->GetRelHead();
	ZRelHead theRelHeadB = fRHS->GetRelHead();

	if (theRelHeadA != theRelHeadB)
		return ZRelHead();

	return theRelHeadA;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Relation

bool ZVisitor_ExprRep_Relation::Visit_Difference(ZRef<ZExprRep_Relation_Difference> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Relation> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Relation> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

bool ZVisitor_ExprRep_Relation::Visit_Intersect(ZRef<ZExprRep_Relation_Intersect> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Relation> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Relation> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

bool ZVisitor_ExprRep_Relation::Visit_Join(ZRef<ZExprRep_Relation_Join> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Relation> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Relation> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

bool ZVisitor_ExprRep_Relation::Visit_Project(ZRef<ZExprRep_Relation_Project> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Relation> theExpr = iRep->GetExpr())
		{
		if (!theExpr->Accept(*this))
			return false;
		}

	return true;
	}

bool ZVisitor_ExprRep_Relation::Visit_Rename(ZRef<ZExprRep_Relation_Rename> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Relation> theExpr = iRep->GetExpr())
		{
		if (!theExpr->Accept(*this))
			return false;
		}

	return true;
	}

bool ZVisitor_ExprRep_Relation::Visit_Union(ZRef<ZExprRep_Relation_Union> iRep)
	{
	if (!ZVisitor_ExprRep::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Relation> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Relation> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Relation

ZExpr_Relation::ZExpr_Relation()
	{}

ZExpr_Relation::ZExpr_Relation(const ZExpr_Relation& iOther)
:	inherited(iOther)
	{}

ZExpr_Relation::~ZExpr_Relation()
	{}

ZExpr_Relation& ZExpr_Relation::operator=(const ZExpr_Relation& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ZExpr_Relation::ZExpr_Relation(const ZRef<ZExprRep_Relation>& iRep)
:	inherited(iRep)
	{}

ZExpr_Relation::operator ZRef<ZExprRep_Relation>() const
	{ return ZRefStaticCast<ZExprRep_Relation>(*this); }

ZRelHead ZExpr_Relation::GetRelHead() const
	{
	if (ZRef<ZExprRep_Relation> theRep = *this)
		return theRep->GetRelHead();
	return ZRelHead();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZExpr_Relation sIntersect(const ZExpr_Relation& iLHS, const ZExpr_Relation& iRHS)
	{ return ZExpr_Relation(new ZExprRep_Relation_Intersect(iLHS, iRHS)); }

ZExpr_Relation sJoin(const ZExpr_Relation& iLHS, const ZExpr_Relation& iRHS)
	{ return ZExpr_Relation(new ZExprRep_Relation_Join(iLHS, iRHS)); }

ZExpr_Relation sProject(const ZRelHead& iRelHead, const ZExpr_Relation& iExpr)
	{ return ZExpr_Relation(new ZExprRep_Relation_Project(iExpr, iRelHead)); }

ZExpr_Relation sRename(const string& iOldPropName, const string& iNewPropName,
	const ZExpr_Relation& iExpr)
	{ return ZExpr_Relation(new ZExprRep_Relation_Rename(iExpr, iOldPropName, iNewPropName)); }

ZExpr_Relation sUnion(const ZExpr_Relation& iLHS, const ZExpr_Relation& iRHS)
	{ return ZExpr_Relation(new ZExprRep_Relation_Union(iLHS, iRHS)); }

ZExpr_Relation operator&(const ZExpr_Relation& iLHS, const ZExpr_Relation& iRHS)
	{ return ZExpr_Relation(new ZExprRep_Relation_Intersect(iLHS, iRHS)); }

ZExpr_Relation operator|(const ZExpr_Relation& iLHS, const ZExpr_Relation& iRHS)
	{ return ZExpr_Relation(new ZExprRep_Relation_Union(iLHS, iRHS)); }

ZExpr_Relation operator*(const ZExpr_Relation& iLHS, const ZExpr_Relation& iRHS)
	{ return ZExpr_Relation(new ZExprRep_Relation_Join(iLHS, iRHS)); }

NAMESPACE_ZOOLIB_END
