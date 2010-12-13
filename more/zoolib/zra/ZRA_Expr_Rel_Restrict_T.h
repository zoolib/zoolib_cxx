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

#ifndef __ZRA_Expr_Rel_Restrict_T__
#define __ZRA_Expr_Rel_Restrict_T__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Op_T.h"
#include "zoolib/ZValPred_T.h"
#include "zoolib/zra/ZRA_Expr_Rel.h"

namespace ZooLib {
namespace ZRA {

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
	Expr_Rel_Restrict_T(ZRef<Expr_Rel> iOp0, const ZValPred_T<Val>& iValPred);
	virtual ~Expr_Rel_Restrict_T();

// From ZExpr_Op1_T<Expr_Rel>
	virtual void Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor);

	virtual ZRef<Expr_Rel> Self();
	virtual ZRef<Expr_Rel> Clone(ZRef<Expr_Rel> iOp0);

// From Expr_Rel
	virtual RelHead GetRelHead();

// Our protocol
	virtual void Accept_Expr_Rel_Restrict(Visitor_Expr_Rel_Restrict_T<Val>& iVisitor);

	ZValPred_T<Val> GetValPred();

private:
	const ZValPred_T<Val> fValPred;
	};

template <class Val>
Expr_Rel_Restrict_T<Val>::Expr_Rel_Restrict_T(
	ZRef<Expr_Rel> iOp0, const ZValPred_T<Val>& iValPred)
:	inherited(iOp0)
,	fValPred(iValPred)
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
	{ return new Expr_Rel_Restrict_T<Val>(iOp0, fValPred); }

template <class Val>
RelHead Expr_Rel_Restrict_T<Val>::GetRelHead()
	{ return this->GetOp0()->GetRelHead(); }

template <class Val>
void Expr_Rel_Restrict_T<Val>::Accept_Expr_Rel_Restrict(Visitor_Expr_Rel_Restrict_T<Val>& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Restrict(this); }

template <class Val>
ZValPred_T<Val> Expr_Rel_Restrict_T<Val>::GetValPred()
	{ return fValPred; }

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

// The order of parameters here differes from that in sSelect precisely to keep them distinct.
template <class Val>
ZRef<Expr_Rel_Restrict_T<Val> > sRestrict_T(
	const ZValPred_T<Val>& iValPred, const ZRef<Expr_Rel>& iExpr_Rel)
	{ return new Expr_Rel_Restrict_T<Val>(iExpr_Rel, iValPred); }

template <class Val>
ZRef<Expr_Rel> operator&(const ZRef<Expr_Rel>& iExpr_Rel, const ZValPred_T<Val>& iValPred)
	{ return sRestrict_T<Val>(iValPred, iExpr_Rel); }

template <class Val>
ZRef<Expr_Rel> operator&(const ZValPred_T<Val>& iValPred, const ZRef<Expr_Rel>& iExpr_Rel)
	{ return sRestrict_T<Val>(iValPred, iExpr_Rel); }

template <class Val>
ZRef<Expr_Rel>& operator&=(ZRef<Expr_Rel>& ioExpr_Rel, const ZValPred_T<Val>& iValPred)
	{ return ioExpr_Rel = ioExpr_Rel & iValPred; }

} // namespace ZRA
} // namespace ZooLib

#endif // __ZRA_Expr_Rel_Restrict_T__
