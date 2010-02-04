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

#include "zoolib/ZExpr_Relational.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational

ZExprRep_Relational::ZExprRep_Relational()
	{}

bool ZExprRep_Relational::Accept(ZVisitor_ExprRep& iVisitor)
	{
	if (ZVisitor_ExprRep_Relational* theVisitor =
		dynamic_cast<ZVisitor_ExprRep_Relational*>(&iVisitor))
		{
		return this->Accept(*theVisitor);
		}
	else
		{
		return ZExprRep::Accept(iVisitor);
		}
	}

bool ZExprRep_Relational::Accept(ZVisitor_ExprRep_Relational& iVisitor)
	{ return ZExprRep::Accept(iVisitor); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Dyadic

ZExprRep_Relational_Dyadic::ZExprRep_Relational_Dyadic(
	ZRef<ZExprRep_Relational> iLHS, ZRef<ZExprRep_Relational> iRHS)
:	fLHS(iLHS)
,	fRHS(iRHS)
	{}

ZExprRep_Relational_Dyadic::~ZExprRep_Relational_Dyadic()
	{}

ZRef<ZExprRep_Relational> ZExprRep_Relational_Dyadic::GetLHS()
	{ return fLHS; }

ZRef<ZExprRep_Relational> ZExprRep_Relational_Dyadic::GetRHS()
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Difference

ZExprRep_Relational_Difference::ZExprRep_Relational_Difference(
	ZRef<ZExprRep_Relational> iLHS, ZRef<ZExprRep_Relational> iRHS)
:	ZExprRep_Relational_Dyadic(iLHS, iRHS)
	{}

bool ZExprRep_Relational_Difference::Accept(ZVisitor_ExprRep_Relational& iVisitor)
	{ return iVisitor.Visit_Difference(this); }

ZRelHead ZExprRep_Relational_Difference::GetRelHead()
	{
	ZRelHead theRelHeadA = fLHS->GetRelHead();
	ZRelHead theRelHeadB = fRHS->GetRelHead();

	if (theRelHeadA != theRelHeadB)
		return ZRelHead();

	return theRelHeadA;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Intersect

ZExprRep_Relational_Intersect::ZExprRep_Relational_Intersect(
	ZRef<ZExprRep_Relational> iLHS, ZRef<ZExprRep_Relational> iRHS)
:	ZExprRep_Relational_Dyadic(iLHS, iRHS)
	{}

bool ZExprRep_Relational_Intersect::Accept(ZVisitor_ExprRep_Relational& iVisitor)
	{ return iVisitor.Visit_Intersect(this); }

ZRelHead ZExprRep_Relational_Intersect::GetRelHead()
	{
	ZRelHead theRelHeadA = fLHS->GetRelHead();
	ZRelHead theRelHeadB = fRHS->GetRelHead();

	if (theRelHeadA != theRelHeadB)
		return ZRelHead();

	return theRelHeadA;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Join

ZExprRep_Relational_Join::ZExprRep_Relational_Join(
	ZRef<ZExprRep_Relational> iLHS, ZRef<ZExprRep_Relational> iRHS)
:	ZExprRep_Relational_Dyadic(iLHS, iRHS)
	{}

bool ZExprRep_Relational_Join::Accept(ZVisitor_ExprRep_Relational& iVisitor)
	{ return iVisitor.Visit_Join(this); }

ZRelHead ZExprRep_Relational_Join::GetRelHead()
	{ return fLHS->GetRelHead() | fRHS->GetRelHead(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Project

ZExprRep_Relational_Project::ZExprRep_Relational_Project(
	ZRef<ZExprRep_Relational> iExpr, const ZRelHead& iRelHead)
:	fExpr(iExpr)
,	fRelHead(iRelHead)
	{
	ZAssert(iExpr);
	}

ZExprRep_Relational_Project::~ZExprRep_Relational_Project()
	{}

bool ZExprRep_Relational_Project::Accept(ZVisitor_ExprRep_Relational& iVisitor)
	{ return iVisitor.Visit_Project(this); }

ZRelHead ZExprRep_Relational_Project::GetRelHead()
	{ return fExpr->GetRelHead() & fRelHead; }

ZRef<ZExprRep_Relational> ZExprRep_Relational_Project::GetExpr()
	{ return fExpr; }

const ZRelHead& ZExprRep_Relational_Project::GetProjectRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Rename

ZExprRep_Relational_Rename::ZExprRep_Relational_Rename(
	ZRef<ZExprRep_Relational> iExpr, const string& iOld, const string& iNew)
:	fExpr(iExpr)
,	fOld(iOld)
,	fNew(iNew)
	{}

ZExprRep_Relational_Rename::~ZExprRep_Relational_Rename()
	{}

bool ZExprRep_Relational_Rename::Accept(ZVisitor_ExprRep_Relational& iVisitor)
	{ return iVisitor.Visit_Rename(this); }

ZRelHead ZExprRep_Relational_Rename::GetRelHead()
	{
	ZRelHead theRelHead = fExpr->GetRelHead();
	if (theRelHead.Contains(fOld))
		{
		theRelHead -= fOld;
		theRelHead |= fNew;
		}
	return theRelHead;
	}

ZRef<ZExprRep_Relational> ZExprRep_Relational_Rename::GetExpr()
	{ return fExpr; }
	
const string& ZExprRep_Relational_Rename::GetOld()
	{ return fOld; }

const string& ZExprRep_Relational_Rename::GetNew()
	{ return fNew; }

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Relational_Union

ZExprRep_Relational_Union::ZExprRep_Relational_Union(
	ZRef<ZExprRep_Relational> iLHS, ZRef<ZExprRep_Relational> iRHS)
:	ZExprRep_Relational_Dyadic(iLHS, iRHS)
	{}

bool ZExprRep_Relational_Union::Accept(ZVisitor_ExprRep_Relational& iVisitor)
	{ return iVisitor.Visit_Union(this); }

ZRelHead ZExprRep_Relational_Union::GetRelHead()
	{
	ZRelHead theRelHeadA = fLHS->GetRelHead();
	ZRelHead theRelHeadB = fRHS->GetRelHead();

	if (theRelHeadA != theRelHeadB)
		return ZRelHead();

	return theRelHeadA;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Relational

bool ZVisitor_ExprRep_Relational::Visit_Difference(ZRef<ZExprRep_Relational_Difference> iRep)
	{
	if (!ZVisitor_ExprRep::Visit(iRep))
		return false;

	if (ZRef<ZExprRep_Relational> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Relational> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

bool ZVisitor_ExprRep_Relational::Visit_Intersect(ZRef<ZExprRep_Relational_Intersect> iRep)
	{
	if (!ZVisitor_ExprRep::Visit(iRep))
		return false;

	if (ZRef<ZExprRep_Relational> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Relational> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

bool ZVisitor_ExprRep_Relational::Visit_Join(ZRef<ZExprRep_Relational_Join> iRep)
	{
	if (!ZVisitor_ExprRep::Visit(iRep))
		return false;

	if (ZRef<ZExprRep_Relational> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Relational> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

bool ZVisitor_ExprRep_Relational::Visit_Project(ZRef<ZExprRep_Relational_Project> iRep)
	{
	if (!ZVisitor_ExprRep::Visit(iRep))
		return false;

	if (ZRef<ZExprRep_Relational> theExpr = iRep->GetExpr())
		{
		if (!theExpr->Accept(*this))
			return false;
		}

	return true;
	}

bool ZVisitor_ExprRep_Relational::Visit_Rename(ZRef<ZExprRep_Relational_Rename> iRep)
	{
	if (!ZVisitor_ExprRep::Visit(iRep))
		return false;

	if (ZRef<ZExprRep_Relational> theExpr = iRep->GetExpr())
		{
		if (!theExpr->Accept(*this))
			return false;
		}

	return true;
	}

bool ZVisitor_ExprRep_Relational::Visit_Union(ZRef<ZExprRep_Relational_Union> iRep)
	{
	if (!ZVisitor_ExprRep::Visit(iRep))
		return false;

	if (ZRef<ZExprRep_Relational> theLHS = iRep->GetLHS())
		{
		if (!theLHS->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Relational> theRHS = iRep->GetRHS())
		{
		if (!theRHS->Accept(*this))
			return false;
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Relational

ZExpr_Relational::ZExpr_Relational()
	{}

ZExpr_Relational::ZExpr_Relational(const ZExpr_Relational& iOther)
:	inherited(iOther)
	{}

ZExpr_Relational::~ZExpr_Relational()
	{}

ZExpr_Relational& ZExpr_Relational::operator=(const ZExpr_Relational& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ZExpr_Relational::ZExpr_Relational(const ZRef<ZExprRep_Relational>& iRep)
:	inherited(iRep)
	{}

ZExpr_Relational::operator ZRef<ZExprRep_Relational>() const
	{ return ZRefStaticCast<ZExprRep_Relational>(*this); }

ZRelHead ZExpr_Relational::GetRelHead() const
	{
	if (ZRef<ZExprRep_Relational> theRep = *this)
		return theRep->GetRelHead();
	return ZRelHead();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZExpr_Relational sIntersect(const ZExpr_Relational& iLHS, const ZExpr_Relational& iRHS)
	{ return ZExpr_Relational(new ZExprRep_Relational_Intersect(iLHS, iRHS)); }

ZExpr_Relational sJoin(const ZExpr_Relational& iLHS, const ZExpr_Relational& iRHS)
	{ return ZExpr_Relational(new ZExprRep_Relational_Join(iLHS, iRHS)); }

ZExpr_Relational sProject(const ZRelHead& iRelHead, const ZExpr_Relational& iExpr)
	{ return ZExpr_Relational(new ZExprRep_Relational_Project(iExpr, iRelHead)); }

ZExpr_Relational sRename(const std::string& iOldPropName, const std::string& iNewPropName,
	const ZExpr_Relational& iExpr)
	{ return ZExpr_Relational(new ZExprRep_Relational_Rename(iExpr, iOldPropName, iNewPropName)); }

ZExpr_Relational sUnion(const ZExpr_Relational& iLHS, const ZExpr_Relational& iRHS)
	{ return ZExpr_Relational(new ZExprRep_Relational_Union(iLHS, iRHS)); }

ZExpr_Relational operator&(const ZExpr_Relational& iLHS, const ZExpr_Relational& iRHS)
	{ return ZExpr_Relational(new ZExprRep_Relational_Intersect(iLHS, iRHS)); }

ZExpr_Relational operator|(const ZExpr_Relational& iLHS, const ZExpr_Relational& iRHS)
	{ return ZExpr_Relational(new ZExprRep_Relational_Union(iLHS, iRHS)); }

ZExpr_Relational operator*(const ZExpr_Relational& iLHS, const ZExpr_Relational& iRHS)
	{ return ZExpr_Relational(new ZExprRep_Relational_Join(iLHS, iRHS)); }

NAMESPACE_ZOOLIB_END
