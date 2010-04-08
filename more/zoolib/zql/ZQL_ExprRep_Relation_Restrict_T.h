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

#ifndef __ZQL_ExprRep_Relation_Restrict_T__
#define __ZQL_ExprRep_Relation_Restrict_T__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_ExprRep_Relation.h"
#include "zoolib/ZValCondition_T.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

template <class Val>
class Visitor_ExprRep_Relation_Restrict_T;

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Restrict_T

template <class Val>
class ExprRep_Relation_Restrict_T : public ExprRep_Relation
	{
public:
	ExprRep_Relation_Restrict_T(
		const ZValCondition_T<Val>& iValCondition, const ZRef<ExprRep_Relation>& iExprRep);

	virtual ~ExprRep_Relation_Restrict_T();

// From ExprRep_Relation
	virtual bool Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor);

	virtual ZRelHead GetRelHead();

// Our protocol
	virtual bool Accept_ExprRep_Relation_Restrict(Visitor_ExprRep_Relation_Restrict_T<Val>& iVisitor);

	ZValCondition_T<Val> GetValCondition();
	ZRef<ExprRep_Relation> GetExprRep();

private:
	const ZValCondition_T<Val> fValCondition;
	const ZRef<ExprRep_Relation> fExprRep;
	};

template <class Val>
ExprRep_Relation_Restrict_T<Val>::ExprRep_Relation_Restrict_T(
	const ZValCondition_T<Val>& iValCondition, const ZRef<ExprRep_Relation>& iExprRep)
:	fValCondition(iValCondition)
,	fExprRep(iExprRep)
	{}

template <class Val>
ExprRep_Relation_Restrict_T<Val>::~ExprRep_Relation_Restrict_T()
	{}

template <class Val>
bool ExprRep_Relation_Restrict_T<Val>::Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor)
	{
	if (Visitor_ExprRep_Relation_Restrict_T<Val>* theVisitor =
		dynamic_cast<Visitor_ExprRep_Relation_Restrict_T<Val>*>(&iVisitor))
		{
		return this->Accept_ExprRep_Relation_Restrict(*theVisitor);
		}
	else
		{
		return ExprRep_Relation::Accept_ExprRep_Relation(iVisitor);
		}
	}

template <class Val>
ZRelHead ExprRep_Relation_Restrict_T<Val>::GetRelHead()
	{ return fExprRep->GetRelHead() | fValCondition.GetRelHead(); }
	
template <class Val>
bool ExprRep_Relation_Restrict_T<Val>::Accept_ExprRep_Relation_Restrict(Visitor_ExprRep_Relation_Restrict_T<Val>& iVisitor)
	{ return iVisitor.Visit_ExprRep_Relation_Restrict(this); }

template <class Val>
ZValCondition_T<Val> ExprRep_Relation_Restrict_T<Val>::GetValCondition()
	{ return fValCondition; }

template <class Val>
ZRef<ExprRep_Relation> ExprRep_Relation_Restrict_T<Val>::GetExprRep()
	{ return fExprRep; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Restrict_T

template <class Val>
class Visitor_ExprRep_Relation_Restrict_T : public virtual Visitor_ExprRep_Relation
	{
public:
	virtual bool Visit_ExprRep_Relation_Restrict(ZRef<ExprRep_Relation_Restrict_T<Val> > iRep);
	};

template <class Val>
bool Visitor_ExprRep_Relation_Restrict_T<Val>::Visit_ExprRep_Relation_Restrict(ZRef<ExprRep_Relation_Restrict_T<Val> > iRep)
	{
	if (ZRef<ExprRep_Relation> theExprRep = iRep->GetExprRep())
		theExprRep->Accept(*this);

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Operators

template <class Val>
ZRef<ExprRep_Relation_Restrict_T<Val> > sRestrict_T(
	const ZValCondition_T<Val>& iValCondition, const ZRef<ExprRep_Relation>& iExprRep_Relation)
	{ return new ExprRep_Relation_Restrict_T<Val>(iValCondition, iExprRep_Relation); }

template <class Val>
ZRef<ExprRep_Relation_Restrict_T<Val> > operator&(
	const ZRef<ExprRep_Relation>& iExprRep_Relation, const ZValCondition_T<Val>& iValCondition)
	{ return new ExprRep_Relation_Restrict_T<Val>(iValCondition, iExprRep_Relation); }

template <class Val>
ZRef<ExprRep_Relation_Restrict_T<Val> > operator&(
	const ZValCondition_T<Val>& iValCondition, const ZRef<ExprRep_Relation>& iExprRep_Relation)
	{ return new ExprRep_Relation_Restrict_T<Val>(iValCondition, iExprRep_Relation); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_ExprRep_Relation_Restrict_T__
