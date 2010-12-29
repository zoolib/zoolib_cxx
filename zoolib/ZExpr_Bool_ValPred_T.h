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

#ifndef __ZExpr_Bool_ValPred_T__
#define __ZExpr_Bool_ValPred_T__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Bool.h"
#include "zoolib/ZValPred_T.h"

namespace ZooLib {

template <class Val> class ZVisitor_Expr_Bool_ValPred_T;

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool_ValPred_T

template <class Val>
class ZExpr_Bool_ValPred_T
:	public virtual ZExpr_Bool
,	public virtual ZExpr_Op0_T<ZExpr_Bool>
	{
	typedef ZExpr_Op0_T<ZExpr_Bool> inherited;
public:
	ZExpr_Bool_ValPred_T(const ZValPred_T<Val>& iValPred);
	virtual ~ZExpr_Bool_ValPred_T();

// From ZExpr_Op0
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Bool>& iVisitor);

	virtual ZRef<ZExpr_Bool> Self();
	virtual ZRef<ZExpr_Bool> Clone();

// Our protocol
	virtual void Accept_Expr_Bool_ValPred(ZVisitor_Expr_Bool_ValPred_T<Val>& iVisitor);

	const ZValPred_T<Val>& GetValPred();

private:
	const ZValPred_T<Val> fValPred;
	};

template <class Val>
ZExpr_Bool_ValPred_T<Val>::ZExpr_Bool_ValPred_T(const ZValPred_T<Val>& iValPred)
:	fValPred(iValPred)
	{}

template <class Val>
ZExpr_Bool_ValPred_T<Val>::~ZExpr_Bool_ValPred_T()
	{}

template <class Val>
void ZExpr_Bool_ValPred_T<Val>::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Bool>& iVisitor)
	{
	if (ZVisitor_Expr_Bool_ValPred_T<Val>* theVisitor =
		dynamic_cast<ZVisitor_Expr_Bool_ValPred_T<Val>*>(&iVisitor))
		{
		this->Accept_Expr_Bool_ValPred(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op0(iVisitor);
		}
	}

template <class Val>
ZRef<ZExpr_Bool> ZExpr_Bool_ValPred_T<Val>::Self()
	{ return this; }

template <class Val>
ZRef<ZExpr_Bool> ZExpr_Bool_ValPred_T<Val>::Clone()
	{ return this; }

template <class Val>
void ZExpr_Bool_ValPred_T<Val>::Accept_Expr_Bool_ValPred(
	ZVisitor_Expr_Bool_ValPred_T<Val>& iVisitor)
	{ iVisitor.Visit_Expr_Bool_ValPred(this); }

template <class Val>
const ZValPred_T<Val>& ZExpr_Bool_ValPred_T<Val>::GetValPred()
	{ return fValPred; }

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_ValPred_T

template <class Val>
class ZVisitor_Expr_Bool_ValPred_T
:	public virtual ZVisitor_Expr_Op0_T<ZExpr_Bool>
	{
	typedef ZVisitor_Expr_Op0_T<ZExpr_Bool> inherited;
public:
	virtual void Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred_T<Val> >& iExpr);
	};

template <class Val>
void ZVisitor_Expr_Bool_ValPred_T<Val>::Visit_Expr_Bool_ValPred(
	const ZRef<ZExpr_Bool_ValPred_T<Val> >& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Operators

template <class Val>
ZRef<ZExpr_Bool> sExpr_Bool(const ZValPred_T<Val>& iValPred)
	{ return new ZExpr_Bool_ValPred_T<Val>(iValPred); }

template <class Val>
ZRef<ZExpr_Bool_Not> operator~(const ZValPred_T<Val>& iValPred)
	{ return new ZExpr_Bool_Not(sExpr_Bool(iValPred)); }

template <class Val>
ZRef<ZExpr_Bool> operator&(bool iBool, const ZValPred_T<Val>& iValPred)
	{
	if (iBool)
		return sExpr_Bool(iValPred);
	return new ZExpr_Bool_False;
	}

template <class Val>
ZRef<ZExpr_Bool> operator&(const ZValPred_T<Val>& iValPred, bool iBool)
	{
	if (iBool)
		return sExpr_Bool(iValPred);
	return new ZExpr_Bool_False;
	}

template <class Val>
ZRef<ZExpr_Bool> operator|(bool iBool, const ZValPred_T<Val>& iValPred)
	{
	if (iBool)
		return new ZExpr_Bool_True;
	return sExpr_Bool(iValPred);
	}

template <class Val>
ZRef<ZExpr_Bool> operator|(const ZValPred_T<Val>& iValPred, bool iBool)
	{
	if (iBool)
		return new ZExpr_Bool_True;
	return sExpr_Bool(iValPred);
	}

template <class Val>
ZRef<ZExpr_Bool> operator&(const ZValPred_T<Val>& iLHS, const ZValPred_T<Val>& iRHS)
	{ return new ZExpr_Bool_And(sExpr_Bool(iLHS), sExpr_Bool(iRHS)); }

template <class Val>
ZRef<ZExpr_Bool> operator&(const ZValPred_T<Val>& iLHS, const ZRef<ZExpr_Bool>& iRHS)
	{ return new ZExpr_Bool_And(sExpr_Bool(iLHS), iRHS); }

template <class Val>
ZRef<ZExpr_Bool> operator&(const ZRef<ZExpr_Bool>& iLHS, const ZValPred_T<Val>& iRHS)
	{ return new ZExpr_Bool_And(iLHS, sExpr_Bool(iRHS)); }

template <class Val>
ZRef<ZExpr_Bool>& operator&=(ZRef<ZExpr_Bool>& ioLHS, const ZValPred_T<Val>& iRHS)
	{ return ioLHS = ioLHS & iRHS; }

template <class Val>
ZRef<ZExpr_Bool> operator|(const ZValPred_T<Val>& iLHS, const ZValPred_T<Val>& iRHS)
	{ return new ZExpr_Bool_Or(sExpr_Bool(iLHS), sExpr_Bool(iRHS)); }

template <class Val>
ZRef<ZExpr_Bool> operator|(const ZValPred_T<Val>& iLHS, const ZRef<ZExpr_Bool>& iRHS)
	{ return new ZExpr_Bool_Or(sExpr_Bool(iLHS), iRHS); }

template <class Val>
ZRef<ZExpr_Bool> operator|(const ZRef<ZExpr_Bool>& iLHS, const ZValPred_T<Val>& iRHS)
	{ return new ZExpr_Bool_Or(iLHS, sExpr_Bool(iRHS)); }

template <class Val>
ZRef<ZExpr_Bool>& operator|=(ZRef<ZExpr_Bool>& ioLHS, const ZValPred_T<Val>& iRHS)
	{ return ioLHS = ioLHS | iRHS; }

} // namespace ZooLib

#endif // __ZExpr_Bool_ValPred_T__
