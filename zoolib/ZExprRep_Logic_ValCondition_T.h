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

#ifndef __ZExprRep_Logic_ValCondition_T__
#define __ZExprRep_Logic_ValCondition_T__ 1
#include "zconfig.h"

#include "zoolib/ZExprRep_Logic.h"
#include "zoolib/ZValCondition_T.h"
#include "zoolib/ZVisitor_ExprRep_Logic_DoEval.h"

NAMESPACE_ZOOLIB_BEGIN

template <class Val> class ZVisitor_ExprRep_Logic_ValCondition_T;

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_ValCondition_T

template <class Val>
class ZExprRep_Logic_ValCondition_T : public ZExprRep_Logic
	{
public:
	ZExprRep_Logic_ValCondition_T(const ZValCondition_T<Val>& iCondition);
	virtual ~ZExprRep_Logic_ValCondition_T();

// From ZExprRep_Logic
	virtual bool Accept(ZVisitor_ExprRep_Logic& iVisitor);

// Our protocol
	const ZValCondition_T<Val>& GetValCondition();

private:
	const ZValCondition_T<Val> fValCondition;
	};

template <class Val>
ZExprRep_Logic_ValCondition_T<Val>::ZExprRep_Logic_ValCondition_T(const ZValCondition_T<Val>& iValCondition)
:	fValCondition(iValCondition)
	{}

template <class Val>
ZExprRep_Logic_ValCondition_T<Val>::~ZExprRep_Logic_ValCondition_T()
	{}

template <class Val>
bool ZExprRep_Logic_ValCondition_T<Val>::Accept(ZVisitor_ExprRep_Logic& iVisitor)
	{
	if (ZVisitor_ExprRep_Logic_ValCondition_T<Val>* theVisitor =
		dynamic_cast<ZVisitor_ExprRep_Logic_ValCondition_T<Val>*>(&iVisitor))
		{
		return theVisitor->Visit_ValCondition(this);
		}
	else
		{
		return ZExprRep_Logic::Accept(iVisitor);
		}
	}

template <class Val>
const ZValCondition_T<Val>&
ZExprRep_Logic_ValCondition_T<Val>::GetValCondition()
	{ return fValCondition; }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Logic_ValCondition_T

template <class Val>
class ZVisitor_ExprRep_Logic_ValCondition_T : public virtual ZVisitor_ExprRep_Logic
	{
public:
	virtual bool Visit_ValCondition(ZRef<ZExprRep_Logic_ValCondition_T<Val> > iRep);
	};

template <class Val>
bool ZVisitor_ExprRep_Logic_ValCondition_T<Val>::Visit_ValCondition(
	ZRef<ZExprRep_Logic_ValCondition_T<Val> > iRep)
	{ return ZVisitor_ExprRep_Logic::Visit_ExprRep(iRep); }

// =================================================================================================
#pragma mark -
#pragma mark * Operators

template <class Val>
ZRef<ZExprRep_Logic> operator&(
	const ZValCondition_T<Val>& iLHS,
	const ZValCondition_T<Val>& iRHS)
	{
	return new ZExprRep_Logic_And(
		new ZExprRep_Logic_ValCondition_T<Val>(iLHS),
		new ZExprRep_Logic_ValCondition_T<Val>(iRHS));
	}

template <class Val>
ZRef<ZExprRep_Logic> operator&(
	const ZValCondition_T<Val>& iLHS,
	const ZRef<ZExprRep_Logic>& iRHS)
	{ return new ZExprRep_Logic_And(new ZExprRep_Logic_ValCondition_T<Val>(iLHS), iRHS); }

template <class Val>
ZRef<ZExprRep_Logic> operator&(
	const ZRef<ZExprRep_Logic>& iLHS,
	const ZValCondition_T<Val>& iRHS)
	{ return new ZExprRep_Logic_And(new ZExprRep_Logic_ValCondition_T<Val>(iRHS), iLHS); }

template <class Val>
ZRef<ZExprRep_Logic>& operator&=(
	ZRef<ZExprRep_Logic>& iLHS,
	const ZValCondition_T<Val>& iRHS)
	{
	iLHS = iLHS & iRHS;
	return iLHS;
	}

template <class Val>
ZRef<ZExprRep_Logic> operator|(
	const ZValCondition_T<Val>& iLHS,
	const ZValCondition_T<Val>& iRHS)
	{
	return new ZExprRep_Logic_Or(
		new ZExprRep_Logic_ValCondition_T<Val>(iLHS),
		new ZExprRep_Logic_ValCondition_T<Val>(iRHS));
	}

template <class Val>
ZRef<ZExprRep_Logic> operator|(
	const ZValCondition_T<Val>& iLHS,
	const ZRef<ZExprRep_Logic>& iRHS)
	{ return new ZExprRep_Logic_Or(new ZExprRep_Logic_ValCondition_T<Val>(iLHS), iRHS); }

template <class Val>
ZRef<ZExprRep_Logic> operator|(
	const ZRef<ZExprRep_Logic>& iLHS,
	const ZValCondition_T<Val>& iRHS)
	{
	return new ZExprRep_Logic_Or(new ZExprRep_Logic_ValCondition_T<Val>(iRHS), iLHS);
	}

template <class Val>
ZRef<ZExprRep_Logic>& operator|=(
	ZRef<ZExprRep_Logic>& iLHS,
	const ZValCondition_T<Val>& iRHS)
	{
	iLHS = iLHS | iRHS;
	return iLHS;
	}

// =================================================================================================
#pragma mark -
#pragma mark * GetRelHead

template <class Val>
class ZVisitor_ExprRep_Logic_ValCondition_GetRelHead_T
:	public ZVisitor_ExprRep_Logic_ValCondition_T<Val>
	{
	typedef ZVisitor_ExprRep_Logic_ValCondition_T<Val> inherited;
public:
	ZVisitor_ExprRep_Logic_ValCondition_GetRelHead_T(ZRelHead& ioRelHead);

// From ZVisitor_ExprRep_Logic_ValCondition_T
	virtual bool Visit_ValCondition(ZRef<ZExprRep_Logic_ValCondition_T<Val> > iRep);

private:
	ZRelHead& fRelHead;
	};

template <class Val>
ZVisitor_ExprRep_Logic_ValCondition_GetRelHead_T<Val>::ZVisitor_ExprRep_Logic_ValCondition_GetRelHead_T(
	ZRelHead& ioRelHead)
:	fRelHead(ioRelHead)
	{}

template <class Val>
bool ZVisitor_ExprRep_Logic_ValCondition_GetRelHead_T<Val>::Visit_ValCondition(
	ZRef<ZExprRep_Logic_ValCondition_T<Val> > iRep)
	{
	if (!inherited::Visit_ValCondition(iRep))
		return false;
	fRelHead |= iRep->GetValCondition().GetRelHead();
	return true;
	}

template <class Val>
ZRelHead sGetRelHead_T(const ZRef<ZExprRep_Logic>& iRep)
	{
	ZRelHead theRelHead;
	if (iRep)
		{
		ZVisitor_ExprRep_Logic_ValCondition_GetRelHead_T<Val> theVisitor(theRelHead);
		iRep->Accept(theVisitor);
		}
	return theRelHead;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Matches

template <class Val>
class ZVisitor_ExprRep_Logic_ValCondition_Matches_T
:	public virtual ZVisitor_ExprRep_Logic_DoEval
,	public virtual ZVisitor_ExprRep_Logic_ValCondition_T<Val>
	{
	typedef ZVisitor_ExprRep_Logic_ValCondition_T<Val> inherited;
public:

	ZVisitor_ExprRep_Logic_ValCondition_Matches_T(const Val& iVal);

// From ZVisitor_ExprRep_Logic_ValCondition_T
	virtual bool Visit_ValCondition(ZRef<ZExprRep_Logic_ValCondition_T<Val> > iRep);

private:
	const Val& fVal;
	ZValContext fValContext;
	};

template <class Val>
ZVisitor_ExprRep_Logic_ValCondition_Matches_T<Val>::ZVisitor_ExprRep_Logic_ValCondition_Matches_T(
	const Val& iVal)
:	fVal(iVal)
	{}

template <class Val>
bool ZVisitor_ExprRep_Logic_ValCondition_Matches_T<Val>::Visit_ValCondition(
	ZRef<ZExprRep_Logic_ValCondition_T<Val> > iRep)
	{
//	return iRep->GetValCondition().Matches(fValContext, fVal);
	if (iRep->GetValCondition().Matches(fValContext, fVal))
		return true;
	else
		return false;
	}

template <class Val>
bool sMatches_T(const ZRef<ZExprRep_Logic>& iRep, const Val& iVal)
	{
	if (iRep)
		{
		ZVisitor_ExprRep_Logic_ValCondition_Matches_T<Val> theVisitor(iVal);
		return iRep->Accept(theVisitor);
		}
	return false;
	}

template <class Val>
bool sMatches_T(const ZValCondition_T<Val>& iValCondition, const Val& iVal)
	{ return sMatches_T<Val>(new ZExprRep_Logic_ValCondition_T<Val>(iValCondition), iVal); }

NAMESPACE_ZOOLIB_END

#endif // __ZExprRep_Logic_ValCondition_T__
