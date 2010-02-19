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

#ifndef __ZExpr_ValCondition_T__
#define __ZExpr_ValCondition_T__ 1

#include "zconfig.h"

#include "zoolib/ZExpr_Logical.h"
#include "zoolib/ZValCondition_T.h"

NAMESPACE_ZOOLIB_BEGIN

template <class Val> class ZVisitor_ExprRep_ValCondition_T;

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_ValCondition_T

template <class Val>
class ZExprRep_ValCondition_T : public ZExprRep_Logical
	{
public:
	ZExprRep_ValCondition_T(const ZValCondition_T<Val>& iCondition);
	virtual ~ZExprRep_ValCondition_T();

// From ZExprRep_Logical
	virtual bool Accept(ZVisitor_ExprRep_Logical& iVisitor);

// Our protocol
	const ZValCondition_T<Val>& GetValCondition();

private:
	const ZValCondition_T<Val> fValCondition;
	};

template <class Val>
ZExprRep_ValCondition_T<Val>::ZExprRep_ValCondition_T(const ZValCondition_T<Val>& iValCondition)
:	fValCondition(iValCondition)
	{}

template <class Val>
ZExprRep_ValCondition_T<Val>::~ZExprRep_ValCondition_T()
	{}

template <class Val>
bool ZExprRep_ValCondition_T<Val>::Accept(ZVisitor_ExprRep_Logical& iVisitor)
	{
	if (ZVisitor_ExprRep_ValCondition_T<Val>* theVisitor =
		dynamic_cast<ZVisitor_ExprRep_ValCondition_T<Val>*>(&iVisitor))
		{
		return theVisitor->Visit_ValCondition(this);
		}
	else
		{
		return ZExprRep_Logical::Accept(iVisitor);
		}
	}

template <class Val>
const ZValCondition_T<Val>&
ZExprRep_ValCondition_T<Val>::GetValCondition()
	{ return fValCondition; }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_ValCondition_T

template <class Val>
class ZVisitor_ExprRep_ValCondition_T : public ZVisitor_ExprRep_Logical
	{
public:
	virtual bool Visit_ValCondition(ZRef<ZExprRep_ValCondition_T<Val> > iRep);
	};

template <class Val>
bool ZVisitor_ExprRep_ValCondition_T<Val>::Visit_ValCondition(
	ZRef<ZExprRep_ValCondition_T<Val> > iRep)
	{ return ZVisitor_ExprRep_Logical::Visit_ExprRep(iRep); }

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_ValCondition_T

template <class Val>
class ZExpr_ValCondition_T : public ZExpr_Logical
	{
	typedef ZExpr_Logical inherited;

	ZExpr_ValCondition_T operator=(const ZExpr_Logical&);
	ZExpr_ValCondition_T operator=(const ZRef<ZExprRep_Logical>&);

public:
	ZExpr_ValCondition_T();
	ZExpr_ValCondition_T(const ZExpr_ValCondition_T& iOther);
	~ZExpr_ValCondition_T();
	ZExpr_ValCondition_T& operator=(const ZExpr_ValCondition_T& iOther);

	ZExpr_ValCondition_T(const ZValCondition_T<Val>& iValCondition);
	ZExpr_ValCondition_T(const ZRef<ZExprRep_ValCondition_T<Val> >& iRep);

	operator ZRef<ZExprRep_ValCondition_T<Val> >() const;
	};

template <class Val>
ZExpr_ValCondition_T<Val>::ZExpr_ValCondition_T()
	{}

template <class Val>
ZExpr_ValCondition_T<Val>::ZExpr_ValCondition_T(const ZExpr_ValCondition_T& iOther)
:	inherited(iOther)
	{}

template <class Val>
ZExpr_ValCondition_T<Val>::~ZExpr_ValCondition_T()
	{}

template <class Val>
ZExpr_ValCondition_T<Val>&
ZExpr_ValCondition_T<Val>::operator=(const ZExpr_ValCondition_T& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

template <class Val>
ZExpr_ValCondition_T<Val>::ZExpr_ValCondition_T(const ZValCondition_T<Val>& iValCondition)
:	inherited(new ZExprRep_ValCondition_T<Val>(iValCondition))
	{}

template <class Val>
ZExpr_ValCondition_T<Val>::ZExpr_ValCondition_T(const ZRef<ZExprRep_ValCondition_T<Val> >& iRep)
:	inherited(ZRef<ZExprRep_Logical>(iRep))
	{}

template <class Val>
ZExpr_ValCondition_T<Val>::operator ZRef<ZExprRep_ValCondition_T<Val> >() const
	{ return ZRefStaticCast<ZExprRep_ValCondition_T<Val> >(*this); }

// =================================================================================================
#pragma mark -
#pragma mark * Operators

template <class Val>
ZExpr_Logical operator&(
	const ZValCondition_T<Val>& iLHS,
	const ZValCondition_T<Val>& iRHS)
	{
	return ZExpr_Logical(new ZExprRep_Logical_And(
		new ZExprRep_ValCondition_T<Val>(iLHS),
		new ZExprRep_ValCondition_T<Val>(iRHS)));
	}

template <class Val>
ZExpr_Logical operator&(
	const ZValCondition_T<Val>& iLHS,
	const ZExpr_Logical& iRHS)
	{
	return ZExpr_Logical(new ZExprRep_Logical_And(new ZExprRep_ValCondition_T<Val>(iLHS), iRHS));
	}

template <class Val>
ZExpr_Logical operator&(
	const ZExpr_Logical& iLHS,
	const ZValCondition_T<Val>& iRHS)
	{
	return ZExpr_Logical(new ZExprRep_Logical_And(new ZExprRep_ValCondition_T<Val>(iRHS), iLHS));
	}

template <class Val>
ZExpr_Logical& operator&=(
	ZExpr_Logical& iLHS,
	const ZValCondition_T<Val>& iRHS)
	{
	iLHS = iLHS & iRHS;
	return iLHS;
	}

template <class Val>
ZExpr_Logical operator|(
	const ZValCondition_T<Val>& iLHS,
	const ZValCondition_T<Val>& iRHS)
	{
	return ZExpr_Logical(new ZExprRep_Logical_Or(
		new ZExprRep_ValCondition_T<Val>(iLHS),
		new ZExprRep_ValCondition_T<Val>(iRHS)));
	}

template <class Val>
ZExpr_Logical operator|(
	const ZValCondition_T<Val>& iLHS,
	const ZExpr_Logical& iRHS)
	{
	return ZExpr_Logical(new ZExprRep_Logical_Or(new ZExprRep_ValCondition_T<Val>(iLHS), iRHS));
	}

template <class Val>
ZExpr_Logical operator|(
	const ZExpr_Logical& iLHS,
	const ZValCondition_T<Val>& iRHS)
	{
	return ZExpr_Logical(new ZExprRep_Logical_Or(new ZExprRep_ValCondition_T<Val>(iRHS), iLHS));
	}

template <class Val>
ZExpr_Logical& operator|=(
	ZExpr_Logical& iLHS,
	const ZValCondition_T<Val>& iRHS)
	{
	iLHS = iLHS | iRHS;
	return iLHS;
	}

// =================================================================================================
#pragma mark -
#pragma mark * GetRelHead

template <class Val>
class ZVisitor_ExprRep_ValCondition_GetRelHead_T
:	public ZVisitor_ExprRep_ValCondition_T<Val>
	{
	typedef ZVisitor_ExprRep_ValCondition_T<Val> inherited;
public:
	ZVisitor_ExprRep_ValCondition_GetRelHead_T(ZRelHead& ioRelHead);

// From ZVisitor_ExprRep_ValCondition_T
	virtual bool Visit_ValCondition(ZRef<ZExprRep_ValCondition_T<Val> > iRep);

private:
	ZRelHead& fRelHead;
	};

template <class Val>
ZVisitor_ExprRep_ValCondition_GetRelHead_T<Val>::ZVisitor_ExprRep_ValCondition_GetRelHead_T(
	ZRelHead& ioRelHead)
:	fRelHead(ioRelHead)
	{}

template <class Val>
bool ZVisitor_ExprRep_ValCondition_GetRelHead_T<Val>::Visit_ValCondition(
	ZRef<ZExprRep_ValCondition_T<Val> > iRep)
	{
	if (!inherited::Visit_ValCondition(iRep))
		return false;
	fRelHead |= iRep->GetValCondition().GetRelHead();
	return true;
	}

template <class Val>
ZRelHead sGetRelHead_T(const ZRef<ZExprRep_Logical>& iRep)
	{
	ZRelHead theRelHead;
	if (iRep)
		{
		ZVisitor_ExprRep_ValCondition_GetRelHead_T<Val> theVisitor(theRelHead);
		iRep->Accept(theVisitor);
		}
	return theRelHead;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Matches

template <class Val>
class ZVisitor_ExprRep_ValCondition_Matches_T
:	public ZVisitor_ExprRep_ValCondition_T<Val>
	{
	typedef ZVisitor_ExprRep_ValCondition_T<Val> inherited;
public:

	ZVisitor_ExprRep_ValCondition_Matches_T(const Val& iVal);

// From ZVisitor_ExprRep_Logical via ZVisitor_ExprRep_ValCondition_T
	virtual bool Visit_Logical_True(ZRef<ZExprRep_Logical_True> iRep);
	virtual bool Visit_Logical_False(ZRef<ZExprRep_Logical_False> iRep);
	virtual bool Visit_Logical_Not(ZRef<ZExprRep_Logical_Not> iRep);
	virtual bool Visit_Logical_And(ZRef<ZExprRep_Logical_And> iRep);
	virtual bool Visit_Logical_Or(ZRef<ZExprRep_Logical_Or> iRep);

// From ZVisitor_ExprRep_ValCondition_T
	virtual bool Visit_ValCondition(ZRef<ZExprRep_ValCondition_T<Val> > iRep);

private:
	const Val& fVal;
	ZValContext fContext;
	};

template <class Val>
ZVisitor_ExprRep_ValCondition_Matches_T<Val>::ZVisitor_ExprRep_ValCondition_Matches_T(
	const Val& iVal)
:	fVal(iVal)
	{}

template <class Val>
bool ZVisitor_ExprRep_ValCondition_Matches_T<Val>::Visit_Logical_True(
	ZRef<ZExprRep_Logical_True> iRep)
	{ return true; }

template <class Val>
bool ZVisitor_ExprRep_ValCondition_Matches_T<Val>::Visit_Logical_False(
	ZRef<ZExprRep_Logical_False> iRep)
	{ return false; }

template <class Val>
bool ZVisitor_ExprRep_ValCondition_Matches_T<Val>::Visit_Logical_Not(
	ZRef<ZExprRep_Logical_Not> iRep)
	{
	if (ZRef<ZExprRep_Logical> theRep = iRep->GetOperand())
		return ! theRep->Accept(*this);
	return true;
	}

template <class Val>
bool ZVisitor_ExprRep_ValCondition_Matches_T<Val>::Visit_Logical_And(
	ZRef<ZExprRep_Logical_And> iRep)
	{
	if (ZRef<ZExprRep_Logical> theLHS = iRep->GetLHS())
		{
		if (ZRef<ZExprRep_Logical> theRHS = iRep->GetRHS())
			return theLHS->Accept(*this) && theRHS->Accept(*this);
		}
	return false;
	}

template <class Val>
bool ZVisitor_ExprRep_ValCondition_Matches_T<Val>::Visit_Logical_Or(ZRef<ZExprRep_Logical_Or> iRep)
	{
	if (ZRef<ZExprRep_Logical> theLHS = iRep->GetLHS())
		{
		if (theLHS->Accept(*this))
			return true;
		}

	if (ZRef<ZExprRep_Logical> theRHS = iRep->GetRHS())
		{
		if (theRHS->Accept(*this))
			return true;
		}

	return false;
	}

template <class Val>
bool ZVisitor_ExprRep_ValCondition_Matches_T<Val>::Visit_ValCondition(
	ZRef<ZExprRep_ValCondition_T<Val> > iRep)
	{ return iRep->GetValCondition().Matches(fContext, fVal); }

template <class Val>
bool sMatches_T(const ZRef<ZExprRep_Logical>& iRep, const Val& iVal)
	{
	if (iRep)
		{
		ZVisitor_ExprRep_ValCondition_Matches_T<Val> theVisitor(iVal);
		return iRep->Accept(theVisitor);
		}
	return false;
	}

template <class Val>
bool sMatches_T(const ZValCondition_T<Val>& iValCondition, const Val& iVal)
	{ return sMatches_T<Val>(new ZExprRep_ValCondition_T<Val>(iValCondition), iVal); }

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_ValCondition_T__
