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

#ifndef __ZExpr_Logic_ValPred_T__
#define __ZExpr_Logic_ValPred_T__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Logic.h"
#include "zoolib/ZValPred_T.h"

NAMESPACE_ZOOLIB_BEGIN

template <class Val> class ZVisitor_Expr_Logic_ValPred_T;

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_ValPred_T

template <class Val>
class ZExpr_Logic_ValPred_T
:	public virtual ZExpr_Logic
,	public virtual ZExpr_Op0_T<ZExpr_Logic>
	{
	typedef ZExpr_Op0_T<ZExpr_Logic> inherited;
public:
	ZExpr_Logic_ValPred_T(const ZValPred_T<Val>& iCondition);
	virtual ~ZExpr_Logic_ValPred_T();

// From ZExpr_Op0
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Logic>& iVisitor);

	virtual ZRef<ZExpr_Logic> Self();
	virtual ZRef<ZExpr_Logic> Clone();

// Our protocol
	virtual void Accept_Expr_Logic_ValPred(ZVisitor_Expr_Logic_ValPred_T<Val>& iVisitor);

	const ZValPred_T<Val>& GetValPred();

private:
	const ZValPred_T<Val> fValPred;
	};

template <class Val>
ZExpr_Logic_ValPred_T<Val>::ZExpr_Logic_ValPred_T(
	const ZValPred_T<Val>& iValPred)
:	fValPred(iValPred)
	{}

template <class Val>
ZExpr_Logic_ValPred_T<Val>::~ZExpr_Logic_ValPred_T()
	{}

template <class Val>
void ZExpr_Logic_ValPred_T<Val>::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Logic>& iVisitor)
	{
	if (ZVisitor_Expr_Logic_ValPred_T<Val>* theVisitor =
		dynamic_cast<ZVisitor_Expr_Logic_ValPred_T<Val>*>(&iVisitor))
		{
		this->Accept_Expr_Logic_ValPred(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op0(iVisitor);
		}
	}

template <class Val>
ZRef<ZExpr_Logic> ZExpr_Logic_ValPred_T<Val>::Self()
	{ return this; }

template <class Val>
ZRef<ZExpr_Logic> ZExpr_Logic_ValPred_T<Val>::Clone()
	{ return this; }

template <class Val>
void ZExpr_Logic_ValPred_T<Val>::Accept_Expr_Logic_ValPred(
	ZVisitor_Expr_Logic_ValPred_T<Val>& iVisitor)
	{ iVisitor.Visit_Expr_Logic_ValPred(this); }

template <class Val>
const ZValPred_T<Val>&
ZExpr_Logic_ValPred_T<Val>::GetValPred()
	{ return fValPred; }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_ValPred_T

template <class Val>
class ZVisitor_Expr_Logic_ValPred_T
:	public virtual ZVisitor_Expr_Op0_T<ZExpr_Logic>
	{
	typedef ZVisitor_Expr_Op0_T<ZExpr_Logic> inherited;
public:
	virtual void Visit_Expr_Logic_ValPred(ZRef<ZExpr_Logic_ValPred_T<Val> > iExpr);
	};

template <class Val>
void ZVisitor_Expr_Logic_ValPred_T<Val>::Visit_Expr_Logic_ValPred(
	ZRef<ZExpr_Logic_ValPred_T<Val> > iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Operators

template <class Val>
ZRef<ZExpr_Logic> sLogic(const ZValPred_T<Val>& iValPred)
	{ return new ZExpr_Logic_ValPred_T<Val>(iValPred); }

template <class Val>
ZRef<ZExpr_Logic_Not> operator~(const ZValPred_T<Val>& iValPred)
	{ return new ZExpr_Logic_Not(new ZExpr_Logic_ValPred_T<Val>(iValPred)); }

template <class Val>
ZRef<ZExpr_Logic> operator&(bool iBool, const ZValPred_T<Val>& iValPred)
	{
	if (iBool)
		return new ZExpr_Logic_ValPred_T<Val>(iValPred);
	return new ZExpr_Logic_False;
	}

template <class Val>
ZRef<ZExpr_Logic> operator&(const ZValPred_T<Val>& iValPred, bool iBool)
	{
	if (iBool)
		return new ZExpr_Logic_ValPred_T<Val>(iValPred);
	return new ZExpr_Logic_False;
	}

template <class Val>
ZRef<ZExpr_Logic> operator|(bool iBool, const ZValPred_T<Val>& iValPred)
	{
	if (iBool)
		return new ZExpr_Logic_True;
	return new ZExpr_Logic_ValPred_T<Val>(iValPred);
	}

template <class Val>
ZRef<ZExpr_Logic> operator|(const ZValPred_T<Val>& iValPred, bool iBool)
	{
	if (iBool)
		return new ZExpr_Logic_True;
	return new ZExpr_Logic_ValPred_T<Val>(iValPred);
	}

template <class Val>
ZRef<ZExpr_Logic> operator&(const ZValPred_T<Val>& iLHS, const ZValPred_T<Val>& iRHS)
	{
	return new ZExpr_Logic_And(
		new ZExpr_Logic_ValPred_T<Val>(iLHS),
		new ZExpr_Logic_ValPred_T<Val>(iRHS));
	}

template <class Val>
ZRef<ZExpr_Logic> operator&(const ZValPred_T<Val>& iLHS, const ZRef<ZExpr_Logic>& iRHS)
	{ return new ZExpr_Logic_And(new ZExpr_Logic_ValPred_T<Val>(iLHS), iRHS); }

template <class Val>
ZRef<ZExpr_Logic> operator&(const ZRef<ZExpr_Logic>& iLHS, const ZValPred_T<Val>& iRHS)
	{ return new ZExpr_Logic_And(new ZExpr_Logic_ValPred_T<Val>(iRHS), iLHS); }

template <class Val>
ZRef<ZExpr_Logic>& operator&=(ZRef<ZExpr_Logic>& ioLHS, const ZValPred_T<Val>& iRHS)
	{
	ioLHS = ioLHS & iRHS;
	return ioLHS;
	}

template <class Val>
ZRef<ZExpr_Logic> operator|(const ZValPred_T<Val>& iLHS, const ZValPred_T<Val>& iRHS)
	{
	return new ZExpr_Logic_Or(
		new ZExpr_Logic_ValPred_T<Val>(iLHS),
		new ZExpr_Logic_ValPred_T<Val>(iRHS));
	}

template <class Val>
ZRef<ZExpr_Logic> operator|(const ZValPred_T<Val>& iLHS, const ZRef<ZExpr_Logic>& iRHS)
	{ return new ZExpr_Logic_Or(new ZExpr_Logic_ValPred_T<Val>(iLHS), iRHS); }

template <class Val>
ZRef<ZExpr_Logic> operator|(const ZRef<ZExpr_Logic>& iLHS, const ZValPred_T<Val>& iRHS)
	{ return new ZExpr_Logic_Or(new ZExpr_Logic_ValPred_T<Val>(iRHS), iLHS); }

template <class Val>
ZRef<ZExpr_Logic>& operator|=(ZRef<ZExpr_Logic>& ioLHS, const ZValPred_T<Val>& iRHS)
	{
	ioLHS = ioLHS | iRHS;
	return ioLHS;
	}

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Logic_ValPred_T__
