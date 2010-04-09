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

#ifndef __ZQL_ExprRep_Relation_Unary_Restrict_T__
#define __ZQL_ExprRep_Relation_Unary_Restrict_T__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_ExprRep_Relation_Unary.h"
#include "zoolib/ZValCondition_T.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

template <class Val>
class Visitor_ExprRep_Relation_Unary_Restrict_T;

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Unary_Restrict_T

template <class Val>
class ExprRep_Relation_Unary_Restrict_T : public ExprRep_Relation_Unary
	{
public:
	ExprRep_Relation_Unary_Restrict_T(
		const ZRef<ExprRep_Relation>& iExprRep_Relation, const ZValCondition_T<Val>& iValCondition);

	virtual ~ExprRep_Relation_Unary_Restrict_T();

// From ExprRep_Relation via ExprRep_Relation_Unary
	virtual ZRelHead GetRelHead();

// From ExprRep_Relation_Unary
	virtual void Accept_ExprRep_Relation_Unary(Visitor_ExprRep_Relation_Unary& iVisitor);

	virtual ZRef<ExprRep_Relation_Unary> Clone(ZRef<ExprRep_Relation> iExprRep_Relation);

// Our protocol
	virtual void Accept_ExprRep_Relation_Unary_Restrict(
		Visitor_ExprRep_Relation_Unary_Restrict_T<Val>& iVisitor);

	ZValCondition_T<Val> GetValCondition();

private:
	const ZValCondition_T<Val> fValCondition;
	};

template <class Val>
ExprRep_Relation_Unary_Restrict_T<Val>::ExprRep_Relation_Unary_Restrict_T(
	const ZRef<ExprRep_Relation>& iExprRep_Relation, const ZValCondition_T<Val>& iValCondition)
:	ExprRep_Relation_Unary(iExprRep_Relation)
,	fValCondition(iValCondition)
	{}

template <class Val>
ExprRep_Relation_Unary_Restrict_T<Val>::~ExprRep_Relation_Unary_Restrict_T()
	{}

template <class Val>
ZRelHead ExprRep_Relation_Unary_Restrict_T<Val>::GetRelHead()
	{ return this->GetExprRep_Relation()->GetRelHead() | fValCondition.GetRelHead(); }
	
template <class Val>
void ExprRep_Relation_Unary_Restrict_T<Val>::Accept_ExprRep_Relation_Unary(
	Visitor_ExprRep_Relation_Unary& iVisitor)
	{
	if (Visitor_ExprRep_Relation_Unary_Restrict_T<Val>* theVisitor =
		dynamic_cast<Visitor_ExprRep_Relation_Unary_Restrict_T<Val>*>(&iVisitor))
		{
		this->Accept_ExprRep_Relation_Unary_Restrict(*theVisitor);
		}
	else
		{
		ExprRep_Relation_Unary::Accept_ExprRep_Relation_Unary(iVisitor);
		}
	}

template <class Val>
ZRef<ExprRep_Relation_Unary> ExprRep_Relation_Unary_Restrict_T<Val>::Clone(
	ZRef<ExprRep_Relation> iExprRep_Relation)
	{ return new ExprRep_Relation_Unary_Restrict_T<Val>(iExprRep_Relation, fValCondition); }

template <class Val>
void ExprRep_Relation_Unary_Restrict_T<Val>::Accept_ExprRep_Relation_Unary_Restrict(
	Visitor_ExprRep_Relation_Unary_Restrict_T<Val>& iVisitor)
	{ iVisitor.Visit_ExprRep_Relation_Unary_Restrict(this); }

template <class Val>
ZValCondition_T<Val> ExprRep_Relation_Unary_Restrict_T<Val>::GetValCondition()
	{ return fValCondition; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Unary_Restrict_T

template <class Val>
class Visitor_ExprRep_Relation_Unary_Restrict_T : public virtual Visitor_ExprRep_Relation_Unary
	{
public:
	virtual void Visit_ExprRep_Relation_Unary_Restrict(
		ZRef<ExprRep_Relation_Unary_Restrict_T<Val> > iRep);
	};

template <class Val>
void Visitor_ExprRep_Relation_Unary_Restrict_T<Val>::Visit_ExprRep_Relation_Unary_Restrict(
	ZRef<ExprRep_Relation_Unary_Restrict_T<Val> > iRep)
	{ Visitor_ExprRep_Relation_Unary::Visit_ExprRep_Relation_Unary(iRep); }

// =================================================================================================
#pragma mark -
#pragma mark * Operators

template <class Val>
ZRef<ExprRep_Relation_Unary_Restrict_T<Val> > sRestrict_T(
	const ZValCondition_T<Val>& iValCondition, const ZRef<ExprRep_Relation>& iExprRep_Relation)
	{ return new ExprRep_Relation_Unary_Restrict_T<Val>(iExprRep_Relation, iValCondition); }

template <class Val>
ZRef<ExprRep_Relation_Unary_Restrict_T<Val> > operator&(
	const ZRef<ExprRep_Relation>& iExprRep_Relation, const ZValCondition_T<Val>& iValCondition)
	{ return new ExprRep_Relation_Unary_Restrict_T<Val>(iExprRep_Relation, iValCondition); }

template <class Val>
ZRef<ExprRep_Relation_Unary_Restrict_T<Val> > operator&(
	const ZValCondition_T<Val>& iValCondition, const ZRef<ExprRep_Relation>& iExprRep_Relation)
	{ return new ExprRep_Relation_Unary_Restrict_T<Val>(iExprRep_Relation, iValCondition); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_ExprRep_Relation_Unary_Restrict_T__
