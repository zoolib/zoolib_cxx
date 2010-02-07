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

#include "zoolib/ZExpr_Select.h"
#include "zoolib/ZExpr_ValCondition.h" // For sGetRelHead

using std::string;
//using std::vector;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Select

ZExprRep_Select::ZExprRep_Select(
	const ZRef<ZExprRep_Logical>& iExpr_Logical, const ZRef<ZExprRep_Relation>& iExpr_Relation)
:	fExpr_Logical(iExpr_Logical)
,	fExpr_Relation(iExpr_Relation)
	{}

ZExprRep_Select::~ZExprRep_Select()
	{}

bool ZExprRep_Select::Accept(ZVisitor_ExprRep_Relation& iVisitor)
	{
	if (ZVisitor_ExprRep_Select* theVisitor =
		dynamic_cast<ZVisitor_ExprRep_Select*>(&iVisitor))
		{
		return this->Accept(*theVisitor);
		}
	else
		{
		return ZExprRep_Relation::Accept(iVisitor);
		}
	}

ZRelHead ZExprRep_Select::GetRelHead()
	{ return sGetRelHead(fExpr_Logical) | fExpr_Relation->GetRelHead(); }

bool ZExprRep_Select::Accept(ZVisitor_ExprRep_Select& iVisitor)
	{ return iVisitor.Visit_Select(this); }

ZRef<ZExprRep_Logical> ZExprRep_Select::GetExpr_Logical()
	{ return fExpr_Logical; }

ZRef<ZExprRep_Relation> ZExprRep_Select::GetExpr_Relation()
	{ return fExpr_Relation; }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Select

bool ZVisitor_ExprRep_Select::Visit_Select(ZRef<ZExprRep_Select> iRep)
	{
	if (!ZVisitor_ExprRep_Relation::Visit(iRep))
		return false;

	if (ZRef<ZExprRep_Logical> theExpr_Logical = iRep->GetExpr_Logical())
		{
		if (!theExpr_Logical->Accept(*this))
			return false;
		}

	if (ZRef<ZExprRep_Relation> theExpr_Relation = iRep->GetExpr_Relation())
		{
		if (!theExpr_Relation->Accept(*this))
			return false;
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Select

ZExpr_Select::ZExpr_Select()
	{}

ZExpr_Select::ZExpr_Select(const ZExpr_Select& iOther)
:	inherited(iOther)
	{}

ZExpr_Select::~ZExpr_Select()
	{}

ZExpr_Select& ZExpr_Select::operator=(const ZExpr_Select& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ZExpr_Select::ZExpr_Select(const ZRef<ZExprRep_Select>& iRep)
:	inherited(iRep)
	{}

ZExpr_Select::operator ZRef<ZExprRep_Select>() const
	{ return ZRefStaticCast<ZExprRep_Select>(*this); }

// =================================================================================================
#pragma mark -
#pragma mark *

ZExpr_Select sSelect(const ZExpr_Logical& iExpr_Logical, const ZExpr_Relation& iExpr_Relation)
	{ return ZExpr_Select(new ZExprRep_Select(iExpr_Logical, iExpr_Relation)); }

ZExpr_Select operator&(const ZExpr_Logical& iExpr_Logical, const ZExpr_Relation& iExpr_Relation)
	{ return ZExpr_Select(new ZExprRep_Select(iExpr_Logical, iExpr_Relation)); }

ZExpr_Select operator&(const ZExpr_Relation& iExpr_Relation, const ZExpr_Logical& iExpr_Logical)
	{ return ZExpr_Select(new ZExprRep_Select(iExpr_Logical, iExpr_Relation)); }

NAMESPACE_ZOOLIB_END
