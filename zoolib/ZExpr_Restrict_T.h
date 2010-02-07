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

#ifndef __ZExpr_Restrict__
#define __ZExpr_Restrict__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Relation.h"
#include "zoolib/ZValCondition_T.h"

NAMESPACE_ZOOLIB_BEGIN

template <class Val>
class ZVisitor_ExprRep_Restrict_T;

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Restrict_T

template <class Val>
class ZExprRep_Restrict_T : public ZExprRep_Relation
	{
public:
	ZExprRep_Restrict_T(
		const ZValCondition_T<Val>& iValCondition, const ZRef<ZExprRep_Relation>& iExpr);

	virtual ~ZExprRep_Restrict_T();

// From ZExprRep_Relation
	virtual bool Accept(ZVisitor_ExprRep_Relation& iVisitor);

	virtual ZRelHead GetRelHead();

// Our protocol
	virtual bool Accept(ZVisitor_ExprRep_Restrict_T<Val>& iVisitor);

	ZValCondition_T<Val> GetValCondition();
	ZRef<ZExprRep_Relation> GetExpr();

private:
	const ZValCondition_T<Val> fValCondition;
	const ZRef<ZExprRep_Relation> fExpr;
	};

template <class Val>
ZExprRep_Restrict_T<Val>::ZExprRep_Restrict_T(
	const ZValCondition_T<Val>& iValCondition, const ZRef<ZExprRep_Relation>& iExpr)
:	fValCondition(iValCondition)
,	fExpr(iExpr)
	{}

template <class Val>
ZExprRep_Restrict_T<Val>::~ZExprRep_Restrict_T()
	{}

template <class Val>
bool ZExprRep_Restrict_T<Val>::Accept(ZVisitor_ExprRep_Relation& iVisitor)
	{
	if (ZVisitor_ExprRep_Restrict_T<Val>* theVisitor =
		dynamic_cast<ZVisitor_ExprRep_Restrict_T<Val>*>(&iVisitor))
		{
		return this->Accept(*theVisitor);
		}
	else
		{
		return ZExprRep_Relation::Accept(iVisitor);
		}
	}

template <class Val>
ZRelHead ZExprRep_Restrict_T<Val>::GetRelHead()
	{ return fExpr->GetRelHead() | fValCondition.GetRelHead(); }
	
template <class Val>
bool ZExprRep_Restrict_T<Val>::Accept(ZVisitor_ExprRep_Restrict_T<Val>& iVisitor)
	{ return iVisitor.Visit_Restrict(this); }

template <class Val>
ZValCondition_T<Val> ZExprRep_Restrict_T<Val>::GetValCondition()
	{ return fValCondition; }

template <class Val>
ZRef<ZExprRep_Relation> ZExprRep_Restrict_T<Val>::GetExpr()
	{ return fExpr; }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Restrict_T

template <class Val>
class ZVisitor_ExprRep_Restrict_T : public virtual ZVisitor_ExprRep_Relation
	{
public:
	virtual bool Visit_Restrict(ZRef<ZExprRep_Restrict_T<Val> > iRep);
	};

template <class Val>
bool ZVisitor_ExprRep_Restrict_T<Val>::Visit_Restrict(ZRef<ZExprRep_Restrict_T<Val> > iRep)
	{
	if (!ZVisitor_ExprRep_Relation::Visit(iRep))
		return false;

	if (ZRef<ZExprRep_Relation> theExpr = iRep->GetExpr())
		{
		if (!theExpr->Accept(*this))
			return false;
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Restrict_T

template <class Val>
class ZExpr_Restrict_T : public ZExpr_Relation
	{
	typedef ZExpr_Relation inherited;

	ZExpr_Restrict_T operator=(const ZExpr_Relation&);
	ZExpr_Restrict_T operator=(const ZRef<ZExprRep_Relation>&);

public:
	ZExpr_Restrict_T();
	ZExpr_Restrict_T(const ZExpr_Restrict_T& iOther);
	~ZExpr_Restrict_T();
	ZExpr_Restrict_T& operator=(const ZExpr_Restrict_T& iOther);

	ZExpr_Restrict_T(const ZRef<ZExprRep_Restrict_T<Val> >& iRep);

	operator ZRef<ZExprRep_Restrict_T<Val> >() const;
	};

template <class Val>
ZExpr_Restrict_T<Val>::ZExpr_Restrict_T()
	{}

template <class Val>
ZExpr_Restrict_T<Val>::ZExpr_Restrict_T(const ZExpr_Restrict_T& iOther)
:	inherited(iOther)
	{}

template <class Val>
ZExpr_Restrict_T<Val>::~ZExpr_Restrict_T()
	{}

template <class Val>
ZExpr_Restrict_T<Val>& ZExpr_Restrict_T<Val>::operator=(const ZExpr_Restrict_T& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

template <class Val>
ZExpr_Restrict_T<Val>::ZExpr_Restrict_T(const ZRef<ZExprRep_Restrict_T<Val> >& iRep)
:	inherited(iRep)
	{}

template <class Val>
ZExpr_Restrict_T<Val>::operator ZRef<ZExprRep_Restrict_T<Val> >() const
	{ return ZRefStaticCast<ZExprRep_Restrict_T<Val> >(*this); }

// =================================================================================================
#pragma mark -
#pragma mark *

template <class Val>
ZExpr_Restrict_T<Val> sRestrict_T(
	const ZValCondition_T<Val>& iValCondition, const ZExpr_Relation& iExpr_Relation)
	{ return ZExpr_Restrict_T<Val>(new ZExprRep_Restrict_T<Val>(iValCondition, iExpr_Relation)); }

template <class Val>
ZExpr_Restrict_T<Val> operator&(
	const ZExpr_Relation& iExpr_Relation, const ZValCondition_T<Val>& iValCondition)
	{ return ZExpr_Restrict_T<Val>(new ZExprRep_Restrict_T<Val>(iValCondition, iExpr_Relation)); }

template <class Val>
ZExpr_Restrict_T<Val> operator&(
	const ZValCondition_T<Val>& iValCondition, const ZExpr_Relation& iExpr_Relation)
	{ return ZExpr_Restrict_T<Val>(new ZExprRep_Restrict_T<Val>(iValCondition, iExpr_Relation)); }

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Restrict__
