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

#ifndef __ZQL_Expr_Rel_Restrict_T__
#define __ZQL_Expr_Rel_Restrict_T__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Op_T.h"
#include "zoolib/ZValCondition_T.h"
#include "zoolib/zql/ZQL_Expr_Rel.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

template <class Val>
class Visitor_Expr_Rel_Restrict_T;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Restrict_T

template <class Val>
class Expr_Rel_Restrict_T
:	public virtual Expr_Rel
,	public virtual ZExpr_Op1_T<Expr_Rel>
	{
	typedef ZExpr_Op1_T<Expr_Rel> inherited;
public:
	Expr_Rel_Restrict_T(ZRef<Expr_Rel> iOp0, const ZValCondition_T<Val>& iValCondition);
	virtual ~Expr_Rel_Restrict_T();

// From ZExpr_Op1_T<Expr_Rel>
	virtual void Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor);

	virtual ZRef<Expr_Rel> Self();
	virtual ZRef<Expr_Rel> Clone(ZRef<Expr_Rel> iOp0);

// Our protocol
	virtual void Accept_Expr_Rel_Restrict(Visitor_Expr_Rel_Restrict_T<Val>& iVisitor);

	ZValCondition_T<Val> GetValCondition();

private:
	const ZValCondition_T<Val> fValCondition;
	};

template <class Val>
Expr_Rel_Restrict_T<Val>::Expr_Rel_Restrict_T(
	ZRef<Expr_Rel> iOp0, const ZValCondition_T<Val>& iValCondition)
:	inherited(iOp0)
,	fValCondition(iValCondition)
	{}

template <class Val>
Expr_Rel_Restrict_T<Val>::~Expr_Rel_Restrict_T()
	{}
	
template <class Val>
void Expr_Rel_Restrict_T<Val>::Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Restrict_T<Val>* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Restrict_T<Val>*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Restrict(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op1(iVisitor);
		}
	}

template <class Val>
ZRef<Expr_Rel> Expr_Rel_Restrict_T<Val>::Self()
	{ return this; }

template <class Val>
ZRef<Expr_Rel> Expr_Rel_Restrict_T<Val>::Clone(ZRef<Expr_Rel> iOp0)
	{ return new Expr_Rel_Restrict_T<Val>(iOp0, fValCondition); }

template <class Val>
void Expr_Rel_Restrict_T<Val>::Accept_Expr_Rel_Restrict(Visitor_Expr_Rel_Restrict_T<Val>& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Restrict(this); }

template <class Val>
ZValCondition_T<Val> Expr_Rel_Restrict_T<Val>::GetValCondition()
	{ return fValCondition; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Restrict_T

template <class Val>
class Visitor_Expr_Rel_Restrict_T
:	public virtual ZVisitor_Expr_Op1_T<Expr_Rel>
	{
	typedef ZVisitor_Expr_Op1_T<Expr_Rel> inherited;
public:
	virtual void Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict_T<Val> > iExpr);
	};

template <class Val>
void Visitor_Expr_Rel_Restrict_T<Val>::Visit_Expr_Rel_Restrict(
	ZRef<Expr_Rel_Restrict_T<Val> > iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Operators

template <class Val>
ZRef<Expr_Rel_Restrict_T<Val> > sRestrict_T(
	const ZValCondition_T<Val>& iValCondition, const ZRef<Expr_Rel>& iExpr_Rel)
	{ return new Expr_Rel_Restrict_T<Val>(iExpr_Rel, iValCondition); }

template <class Val>
ZRef<Expr_Rel> operator&(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZValCondition_T<Val>& iValCondition)
	{ return new Expr_Rel_Restrict_T<Val>(iExpr_Rel, iValCondition); }

template <class Val>
ZRef<Expr_Rel> operator&(
	const ZValCondition_T<Val>& iValCondition, const ZRef<Expr_Rel>& iExpr_Rel)
	{ return new Expr_Rel_Restrict_T<Val>(iExpr_Rel, iValCondition); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Rel_Restrict_T__
