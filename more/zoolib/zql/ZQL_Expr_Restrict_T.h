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

#ifndef __ZQL_Expr_Restrict_T__
#define __ZQL_Expr_Restrict_T__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_Expr_Relation.h"
#include "zoolib/ZValCondition_T.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

template <class Val>
class Visitor_ExprRep_Restrict_T;

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Restrict_T

template <class Val>
class ExprRep_Restrict_T : public ExprRep_Relation
	{
public:
	ExprRep_Restrict_T(
		const ZValCondition_T<Val>& iValCondition, const ZRef<ExprRep_Relation>& iExprRep);

	virtual ~ExprRep_Restrict_T();

// From ExprRep_Relation
	virtual bool Accept(Visitor_ExprRep_Relation& iVisitor);

	virtual ZRelHead GetRelHead();

// Our protocol
	virtual bool Accept(Visitor_ExprRep_Restrict_T<Val>& iVisitor);

	ZValCondition_T<Val> GetValCondition();
	ZRef<ExprRep_Relation> GetExprRep();

private:
	const ZValCondition_T<Val> fValCondition;
	const ZRef<ExprRep_Relation> fExprRep;
	};

template <class Val>
ExprRep_Restrict_T<Val>::ExprRep_Restrict_T(
	const ZValCondition_T<Val>& iValCondition, const ZRef<ExprRep_Relation>& iExprRep)
:	fValCondition(iValCondition)
,	fExprRep(iExprRep)
	{}

template <class Val>
ExprRep_Restrict_T<Val>::~ExprRep_Restrict_T()
	{}

template <class Val>
bool ExprRep_Restrict_T<Val>::Accept(Visitor_ExprRep_Relation& iVisitor)
	{
	if (Visitor_ExprRep_Restrict_T<Val>* theVisitor =
		dynamic_cast<Visitor_ExprRep_Restrict_T<Val>*>(&iVisitor))
		{
		return this->Accept(*theVisitor);
		}
	else
		{
		return ExprRep_Relation::Accept(iVisitor);
		}
	}

template <class Val>
ZRelHead ExprRep_Restrict_T<Val>::GetRelHead()
	{ return fExprRep->GetRelHead() | fValCondition.GetRelHead(); }
	
template <class Val>
bool ExprRep_Restrict_T<Val>::Accept(Visitor_ExprRep_Restrict_T<Val>& iVisitor)
	{ return iVisitor.Visit_Restrict(this); }

template <class Val>
ZValCondition_T<Val> ExprRep_Restrict_T<Val>::GetValCondition()
	{ return fValCondition; }

template <class Val>
ZRef<ExprRep_Relation> ExprRep_Restrict_T<Val>::GetExprRep()
	{ return fExprRep; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Restrict_T

template <class Val>
class Visitor_ExprRep_Restrict_T : public virtual Visitor_ExprRep_Relation
	{
public:
	virtual bool Visit_Restrict(ZRef<ExprRep_Restrict_T<Val> > iRep);
	};

template <class Val>
bool Visitor_ExprRep_Restrict_T<Val>::Visit_Restrict(ZRef<ExprRep_Restrict_T<Val> > iRep)
	{
	if (!Visitor_ExprRep_Relation::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ExprRep_Relation> theExprRep = iRep->GetExprRep())
		{
		if (!theExprRep->Accept(*this))
			return false;
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Restrict_T

template <class Val>
class Expr_Restrict_T : public Expr_Relation
	{
	typedef Expr_Relation inherited;

	Expr_Restrict_T operator=(const Expr_Relation&);
	Expr_Restrict_T operator=(const ZRef<ExprRep_Relation>&);

public:
	Expr_Restrict_T();
	Expr_Restrict_T(const Expr_Restrict_T& iOther);
	~Expr_Restrict_T();
	Expr_Restrict_T& operator=(const Expr_Restrict_T& iOther);

	Expr_Restrict_T(const ZRef<ExprRep_Restrict_T<Val> >& iRep);

	operator ZRef<ExprRep_Restrict_T<Val> >() const;
	};

template <class Val>
Expr_Restrict_T<Val>::Expr_Restrict_T()
	{}

template <class Val>
Expr_Restrict_T<Val>::Expr_Restrict_T(const Expr_Restrict_T& iOther)
:	inherited(iOther)
	{}

template <class Val>
Expr_Restrict_T<Val>::~Expr_Restrict_T()
	{}

template <class Val>
Expr_Restrict_T<Val>& Expr_Restrict_T<Val>::operator=(const Expr_Restrict_T& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

template <class Val>
Expr_Restrict_T<Val>::Expr_Restrict_T(const ZRef<ExprRep_Restrict_T<Val> >& iRep)
:	inherited(iRep)
	{}

template <class Val>
Expr_Restrict_T<Val>::operator ZRef<ExprRep_Restrict_T<Val> >() const
	{ return this->StaticCast<ExprRep_Restrict_T<Val> >(); }

// =================================================================================================
#pragma mark -
#pragma mark *

template <class Val>
Expr_Restrict_T<Val> sRestrict_T(
	const ZValCondition_T<Val>& iValCondition, const Expr_Relation& iExpr_Relation)
	{ return Expr_Restrict_T<Val>(new ExprRep_Restrict_T<Val>(iValCondition, iExpr_Relation)); }

template <class Val>
Expr_Restrict_T<Val> operator&(
	const Expr_Relation& iExpr_Relation, const ZValCondition_T<Val>& iValCondition)
	{ return Expr_Restrict_T<Val>(new ExprRep_Restrict_T<Val>(iValCondition, iExpr_Relation)); }

template <class Val>
Expr_Restrict_T<Val> operator&(
	const ZValCondition_T<Val>& iValCondition, const Expr_Relation& iExpr_Relation)
	{ return Expr_Restrict_T<Val>(new ExprRep_Restrict_T<Val>(iValCondition, iExpr_Relation)); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Restrict_T__
