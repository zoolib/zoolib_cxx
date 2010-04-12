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

#ifndef __ZExpr_Logic_ValCondition_T__
#define __ZExpr_Logic_ValCondition_T__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Logic.h"
#include "zoolib/ZValCondition_T.h"
#include "zoolib/ZVisitor_Expr_Logic_DoEval.h"

NAMESPACE_ZOOLIB_BEGIN

template <class Val> class ZVisitor_Expr_Logic_ValCondition_T;

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_ValCondition_T

template <class Val>
class ZExpr_Logic_ValCondition_T : public ZExpr_Logic
	{
public:
	ZExpr_Logic_ValCondition_T(const ZValCondition_T<Val>& iCondition);
	virtual ~ZExpr_Logic_ValCondition_T();

// From ZExpr_Logic
	virtual void Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor);

// Our protocol
	virtual void Accept_Expr_Logic_ValCondition(ZVisitor_Expr_Logic_ValCondition_T<Val>& iVisitor);

	const ZValCondition_T<Val>& GetValCondition();

private:
	const ZValCondition_T<Val> fValCondition;
	};

template <class Val>
ZExpr_Logic_ValCondition_T<Val>::ZExpr_Logic_ValCondition_T(const ZValCondition_T<Val>& iValCondition)
:	fValCondition(iValCondition)
	{}

template <class Val>
ZExpr_Logic_ValCondition_T<Val>::~ZExpr_Logic_ValCondition_T()
	{}

template <class Val>
void ZExpr_Logic_ValCondition_T<Val>::Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor)
	{
	if (ZVisitor_Expr_Logic_ValCondition_T<Val>* theVisitor =
		dynamic_cast<ZVisitor_Expr_Logic_ValCondition_T<Val>*>(&iVisitor))
		{
		this->Accept_Expr_Logic_ValCondition(*theVisitor);
		}
	else
		{
		ZExpr_Logic::Accept_Expr_Logic(iVisitor);
		}
	}

template <class Val>
void ZExpr_Logic_ValCondition_T<Val>::Accept_Expr_Logic_ValCondition(ZVisitor_Expr_Logic_ValCondition_T<Val>& iVisitor)
	{ iVisitor.Visit_Logic_ValCondition(this); }

template <class Val>
const ZValCondition_T<Val>&
ZExpr_Logic_ValCondition_T<Val>::GetValCondition()
	{ return fValCondition; }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_ValCondition_T

template <class Val>
class ZVisitor_Expr_Logic_ValCondition_T : public virtual ZVisitor_Expr_Logic
	{
public:
	virtual void Visit_Logic_ValCondition(ZRef<ZExpr_Logic_ValCondition_T<Val> > iExpr);
	};

template <class Val>
void ZVisitor_Expr_Logic_ValCondition_T<Val>::Visit_Logic_ValCondition(
	ZRef<ZExpr_Logic_ValCondition_T<Val> > iExpr)
	{ ZVisitor_Expr_Logic::Visit_Expr(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Operators

template <class Val>
ZRef<ZExpr_Logic> sLogic(const ZValCondition_T<Val>& iValCondition)
	{ return new ZExpr_Logic_ValCondition_T<Val>(iValCondition); }

template <class Val>
ZRef<ZExpr_Logic_Not> operator~(const ZValCondition_T<Val>& iValCondition)
	{ return new ZExpr_Logic_Not(new ZExpr_Logic_ValCondition_T<Val>(iValCondition)); }

template <class Val>
ZRef<ZExpr_Logic> operator&(bool iBool, const ZValCondition_T<Val>& iValCondition)
	{
	if (iBool)
		return new ZExpr_Logic_ValCondition_T<Val>(iValCondition);
	return new ZExpr_Logic_False;
	}

template <class Val>
ZRef<ZExpr_Logic> operator&(const ZValCondition_T<Val>& iValCondition, bool iBool)
	{
	if (iBool)
		return new ZExpr_Logic_ValCondition_T<Val>(iValCondition);
	return new ZExpr_Logic_False;
	}

template <class Val>
ZRef<ZExpr_Logic> operator|(bool iBool, const ZValCondition_T<Val>& iValCondition)
	{
	if (iBool)
		return new ZExpr_Logic_True;
	return new ZExpr_Logic_ValCondition_T<Val>(iValCondition);
	}

template <class Val>
ZRef<ZExpr_Logic> operator|(const ZValCondition_T<Val>& iValCondition, bool iBool)
	{
	if (iBool)
		return new ZExpr_Logic_True;
	return new ZExpr_Logic_ValCondition_T<Val>(iValCondition);
	}

template <class Val>
ZRef<ZExpr_Logic> operator&(const ZValCondition_T<Val>& iLHS, const ZValCondition_T<Val>& iRHS)
	{
	return new ZExpr_Logic_And(
		new ZExpr_Logic_ValCondition_T<Val>(iLHS),
		new ZExpr_Logic_ValCondition_T<Val>(iRHS));
	}

template <class Val>
ZRef<ZExpr_Logic> operator&(const ZValCondition_T<Val>& iLHS, const ZRef<ZExpr_Logic>& iRHS)
	{ return new ZExpr_Logic_And(new ZExpr_Logic_ValCondition_T<Val>(iLHS), iRHS); }

template <class Val>
ZRef<ZExpr_Logic> operator&(const ZRef<ZExpr_Logic>& iLHS, const ZValCondition_T<Val>& iRHS)
	{ return new ZExpr_Logic_And(new ZExpr_Logic_ValCondition_T<Val>(iRHS), iLHS); }

template <class Val>
ZRef<ZExpr_Logic>& operator&=(ZRef<ZExpr_Logic>& ioLHS, const ZValCondition_T<Val>& iRHS)
	{
	ioLHS = ioLHS & iRHS;
	return ioLHS;
	}

template <class Val>
ZRef<ZExpr_Logic> operator|(const ZValCondition_T<Val>& iLHS, const ZValCondition_T<Val>& iRHS)
	{
	return new ZExpr_Logic_Or(
		new ZExpr_Logic_ValCondition_T<Val>(iLHS),
		new ZExpr_Logic_ValCondition_T<Val>(iRHS));
	}

template <class Val>
ZRef<ZExpr_Logic> operator|(const ZValCondition_T<Val>& iLHS, const ZRef<ZExpr_Logic>& iRHS)
	{ return new ZExpr_Logic_Or(new ZExpr_Logic_ValCondition_T<Val>(iLHS), iRHS); }

template <class Val>
ZRef<ZExpr_Logic> operator|(const ZRef<ZExpr_Logic>& iLHS, const ZValCondition_T<Val>& iRHS)
	{ return new ZExpr_Logic_Or(new ZExpr_Logic_ValCondition_T<Val>(iRHS), iLHS); }

template <class Val>
ZRef<ZExpr_Logic>& operator|=(ZRef<ZExpr_Logic>& ioLHS, const ZValCondition_T<Val>& iRHS)
	{
	ioLHS = ioLHS | iRHS;
	return ioLHS;
	}

// =================================================================================================
#pragma mark -
#pragma mark * GetNames

template <class Val>
class ZVisitor_Expr_Logic_ValCondition_DoGetNames_T
:	public ZVisitor_Expr_Logic_ValCondition_T<Val>
	{
public:
// From ZVisitor_Expr_Logic via ZVisitor_Expr_Logic_ValCondition_T
	virtual void Visit_Logic_Not(ZRef<ZExpr_Logic_Not> iExpr);
	virtual void Visit_Logic_And(ZRef<ZExpr_Logic_And> iExpr);
	virtual void Visit_Logic_Or(ZRef<ZExpr_Logic_Or> iExpr);

// From ZVisitor_Expr_Logic_ValCondition_T
	virtual void Visit_Logic_ValCondition(ZRef<ZExpr_Logic_ValCondition_T<Val> > iExpr);

// Our protocol
	std::set<std::string> DoGetNames(ZRef<ZExpr_Logic> iExpr);

protected:
	std::set<std::string> fResult;
	};

template <class Val>
void ZVisitor_Expr_Logic_ValCondition_DoGetNames_T<Val>::Visit_Logic_Not(
	ZRef<ZExpr_Logic_Not> iExpr)
	{ fResult = this->DoGetNames(iExpr); }

template <class Val>
void ZVisitor_Expr_Logic_ValCondition_DoGetNames_T<Val>::Visit_Logic_And(
	ZRef<ZExpr_Logic_And> iExpr)
	{
	std::set<std::string> result;
	ZUtil_STL_set::sOr(this->DoGetNames(iExpr->GetLHS()), this->DoGetNames(iExpr->GetRHS()), result);
	fResult.swap(result);
	}

template <class Val>
void ZVisitor_Expr_Logic_ValCondition_DoGetNames_T<Val>::Visit_Logic_Or(
	ZRef<ZExpr_Logic_Or> iExpr)
	{
	std::set<std::string> result;
	ZUtil_STL_set::sOr(this->DoGetNames(iExpr->GetLHS()), this->DoGetNames(iExpr->GetRHS()), result);
	fResult.swap(result);
	}

template <class Val>
void ZVisitor_Expr_Logic_ValCondition_DoGetNames_T<Val>::Visit_Logic_ValCondition(
	ZRef<ZExpr_Logic_ValCondition_T<Val> > iExpr)
	{ fResult = iExpr->GetValCondition().GetNames(); }

template <class Val>
std::set<std::string> ZVisitor_Expr_Logic_ValCondition_DoGetNames_T<Val>::DoGetNames(
	ZRef<ZExpr_Logic> iExpr)
	{
	std::set<std::string> theResult;
	if (iExpr)
		{
		iExpr->Accept(*this);
		std::swap(theResult, fResult);
		}
	return theResult;
	}

template <class Val>
std::set<std::string> sGetNames_T(const ZRef<ZExpr_Logic>& iExpr)
	{ return ZVisitor_Expr_Logic_ValCondition_DoGetNames_T<Val>().DoGetNames(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Matches

template <class Val>
class ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T
:	public virtual ZVisitor_Expr_Logic_DoEval
,	public virtual ZVisitor_Expr_Logic_ValCondition_T<Val>
	{
	typedef ZVisitor_Expr_Logic_ValCondition_T<Val> inherited;
public:

	ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T(const Val& iVal);

// From ZVisitor_Expr_Logic_ValCondition_T
	virtual void Visit_Logic_ValCondition(ZRef<ZExpr_Logic_ValCondition_T<Val> > iExpr);

private:
	const Val& fVal;
	ZValContext fValContext;
	};

template <class Val>
ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T<Val>::
ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T(const Val& iVal)
:	fVal(iVal)
	{}

template <class Val>
void ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T<Val>::Visit_Logic_ValCondition(
	ZRef<ZExpr_Logic_ValCondition_T<Val> > iExpr)
	{ fResult = iExpr->GetValCondition().Matches(fValContext, fVal); }

template <class Val>
bool sMatches_T(const ZRef<ZExpr_Logic>& iExpr, const Val& iVal)
	{ return ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T<Val>(iVal).DoEval(iExpr); }

template <class Val>
bool sMatches_T(const ZValCondition_T<Val>& iValCondition, const Val& iVal)
	{ return sMatches_T<Val>(new ZExpr_Logic_ValCondition_T<Val>(iValCondition), iVal); }

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Logic_ValCondition_T__
