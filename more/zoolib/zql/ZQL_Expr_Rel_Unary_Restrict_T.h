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

#ifndef __ZQL_Expr_Rel_Unary_Restrict_T__
#define __ZQL_Expr_Rel_Unary_Restrict_T__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_Expr_Rel_Unary.h"
#include "zoolib/ZValCondition_T.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

template <class Val>
class Visitor_Expr_Rel_Unary_Restrict_T;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Unary_Restrict_T

template <class Val>
class Expr_Rel_Unary_Restrict_T : public Expr_Rel_Unary
	{
public:
	Expr_Rel_Unary_Restrict_T(
		const ZRef<Expr_Rel>& iExpr_Rel, const ZValCondition_T<Val>& iValCondition);

	virtual ~Expr_Rel_Unary_Restrict_T();

// From Expr_Rel via Expr_Rel_Unary
	virtual ZRelHead GetRelHead();

// From Expr_Rel_Unary
	virtual void Accept_Expr_Rel_Unary(Visitor_Expr_Rel_Unary& iVisitor);

	virtual ZRef<Expr_Rel_Unary> Clone(ZRef<Expr_Rel> iExpr_Rel);

// Our protocol
	virtual void Accept_Expr_Rel_Unary_Restrict(
		Visitor_Expr_Rel_Unary_Restrict_T<Val>& iVisitor);

	ZValCondition_T<Val> GetValCondition();

private:
	const ZValCondition_T<Val> fValCondition;
	};

template <class Val>
Expr_Rel_Unary_Restrict_T<Val>::Expr_Rel_Unary_Restrict_T(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZValCondition_T<Val>& iValCondition)
:	Expr_Rel_Unary(iExpr_Rel)
,	fValCondition(iValCondition)
	{}

template <class Val>
Expr_Rel_Unary_Restrict_T<Val>::~Expr_Rel_Unary_Restrict_T()
	{}

template <class Val>
ZRelHead Expr_Rel_Unary_Restrict_T<Val>::GetRelHead()
	{ return this->GetExpr_Rel()->GetRelHead() | fValCondition.GetRelHead(); }
	
template <class Val>
void Expr_Rel_Unary_Restrict_T<Val>::Accept_Expr_Rel_Unary(
	Visitor_Expr_Rel_Unary& iVisitor)
	{
	if (Visitor_Expr_Rel_Unary_Restrict_T<Val>* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Unary_Restrict_T<Val>*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Unary_Restrict(*theVisitor);
		}
	else
		{
		Expr_Rel_Unary::Accept_Expr_Rel_Unary(iVisitor);
		}
	}

template <class Val>
ZRef<Expr_Rel_Unary> Expr_Rel_Unary_Restrict_T<Val>::Clone(
	ZRef<Expr_Rel> iExpr_Rel)
	{ return new Expr_Rel_Unary_Restrict_T<Val>(iExpr_Rel, fValCondition); }

template <class Val>
void Expr_Rel_Unary_Restrict_T<Val>::Accept_Expr_Rel_Unary_Restrict(
	Visitor_Expr_Rel_Unary_Restrict_T<Val>& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Unary_Restrict(this); }

template <class Val>
ZValCondition_T<Val> Expr_Rel_Unary_Restrict_T<Val>::GetValCondition()
	{ return fValCondition; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Unary_Restrict_T

template <class Val>
class Visitor_Expr_Rel_Unary_Restrict_T : public virtual Visitor_Expr_Rel_Unary
	{
public:
	virtual void Visit_Expr_Rel_Unary_Restrict(
		ZRef<Expr_Rel_Unary_Restrict_T<Val> > iRep);
	};

template <class Val>
void Visitor_Expr_Rel_Unary_Restrict_T<Val>::Visit_Expr_Rel_Unary_Restrict(
	ZRef<Expr_Rel_Unary_Restrict_T<Val> > iRep)
	{ Visitor_Expr_Rel_Unary::Visit_Expr_Rel_Unary(iRep); }

// =================================================================================================
#pragma mark -
#pragma mark * Operators

template <class Val>
ZRef<Expr_Rel_Unary_Restrict_T<Val> > sRestrict_T(
	const ZValCondition_T<Val>& iValCondition, const ZRef<Expr_Rel>& iExpr_Rel)
	{ return new Expr_Rel_Unary_Restrict_T<Val>(iExpr_Rel, iValCondition); }

template <class Val>
ZRef<Expr_Rel_Unary_Restrict_T<Val> > operator&(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZValCondition_T<Val>& iValCondition)
	{ return new Expr_Rel_Unary_Restrict_T<Val>(iExpr_Rel, iValCondition); }

template <class Val>
ZRef<Expr_Rel_Unary_Restrict_T<Val> > operator&(
	const ZValCondition_T<Val>& iValCondition, const ZRef<Expr_Rel>& iExpr_Rel)
	{ return new Expr_Rel_Unary_Restrict_T<Val>(iExpr_Rel, iValCondition); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Rel_Unary_Restrict_T__
