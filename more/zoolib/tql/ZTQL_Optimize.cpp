/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZExprRep_Logic_ValCondition.h"
#include "zoolib/ZVisitor_ExprRep_Logic_DoTransform.h"
#include "zoolib/tql/ZTQL_Optimize.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Binary_Union.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Unary_Restrict.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Unary_Select.h"
#include "zoolib/zql/ZQL_Visitor_ExprRep_Relation_Binary_DoTransform.h"
#include "zoolib/zql/ZQL_Visitor_ExprRep_Relation_Unary_DoTransform.h"

NAMESPACE_ZOOLIB_BEGIN
using namespace ZQL;

// Turns a Select into a tree of Restrict and Union.

// =================================================================================================
#pragma mark -
#pragma mark * Local stuff

using std::vector;

namespace ZANONYMOUS {

typedef vector<ZValCondition> CondSect;
typedef vector<CondSect> CondUnion;

void spCrossMultiply(const CondUnion& iLeft, const CondUnion& iRight, CondUnion& oResult)
	{
	for (CondUnion::const_iterator iterLeft = iLeft.begin();
		iterLeft != iLeft.end(); ++iterLeft)
		{
		for (CondUnion::const_iterator iterRight = iRight.begin();
			iterRight != iRight.end(); ++iterRight)
			{
			oResult.push_back(*iterLeft);
			CondSect& temp = oResult.back();
			temp.insert(temp.end(), iterRight->begin(), iterRight->end());
			}
		}
	}

void spGather(ZRef<ZExprRep_Logic> iRep, CondUnion& oResult);

class Gather
:	public virtual ZVisitor_ExprRep_Logic_DoTransform
,	public virtual ZVisitor_ExprRep_Logic_ValCondition
	{
public:
	Gather(CondUnion& oResult);

//	From ZVisitor_ExprRep_Logic_DoTransform
	virtual void Visit_Logic_True(ZRef<ZExprRep_Logic_True> iRep);
	virtual void Visit_Logic_False(ZRef<ZExprRep_Logic_False> iRep);
	virtual void Visit_Logic_Not(ZRef<ZExprRep_Logic_Not> iRep);
	virtual void Visit_Logic_And(ZRef<ZExprRep_Logic_And> iRep);
	virtual void Visit_Logic_Or(ZRef<ZExprRep_Logic_Or> iRep);	

// From ZVisitor_ExprRep_Logic_ValCondition
	virtual void Visit_Logic_ValCondition(ZRef<ZExprRep_Logic_ValCondition> iRep);

private:
	CondUnion& fResult;
	};

Gather::Gather(CondUnion& oResult)
:	fResult(oResult)
	{}

void Gather::Visit_Logic_True(ZRef<ZExprRep_Logic_True> iRep)
	{ fResult.resize(1); }

void Gather::Visit_Logic_False(ZRef<ZExprRep_Logic_False> iRep)
	{
	ZAssert(fResult.empty());
//	fResult.clear();
	}

void Gather::Visit_Logic_Not(ZRef<ZExprRep_Logic_Not> iRep)
	{ ZUnimplemented(); }

void Gather::Visit_Logic_And(ZRef<ZExprRep_Logic_And> iRep)
	{
	CondUnion left;
	spGather(iRep->GetLHS(), left);
	CondUnion right;
	spGather(iRep->GetRHS(), right);
	spCrossMultiply(left, right, fResult);
	}

void Gather::Visit_Logic_Or(ZRef<ZExprRep_Logic_Or> iRep)
	{
	CondUnion left;
	spGather(iRep->GetLHS(), left);
	CondUnion right;
	spGather(iRep->GetRHS(), right);
	fResult.swap(left);
	fResult.insert(fResult.end(), right.begin(), right.end());
	}

void Gather::Visit_Logic_ValCondition(ZRef<ZExprRep_Logic_ValCondition> iRep)
	{
	fResult.resize(1);
	fResult[0].push_back(iRep->GetValCondition());
	}

void spGather(ZRef<ZExprRep_Logic> iRep, CondUnion& oResult)
	{
	ZAssert(oResult.empty());
	Gather theGather(oResult);
	iRep->Accept(theGather);
	}

ZRef<ExprRep_Relation> spConvertSelect(
	ZRef<ExprRep_Relation> iRelation, ZRef<ZExprRep_Logic> iLogical)
	{
	if (!iRelation)
		return ZRef<ExprRep_Relation>();

	CondUnion resultLogical;
	spGather(iLogical, resultLogical);

	ZRef<ExprRep_Relation> resultRelation;
	for (CondUnion::const_iterator iterUnion = resultLogical.begin();
		iterUnion != resultLogical.end(); ++iterUnion)
		{
		ZRef<ExprRep_Relation> current = iRelation;
		for (CondSect::const_iterator iterSect = iterUnion->begin();
			iterSect != iterUnion->end(); ++iterSect)
			{
			current = new ExprRep_Relation_Unary_Restrict(current, *iterSect);
			}

		if (resultRelation)
			resultRelation = new ExprRep_Relation_Binary_Union(current, resultRelation);
		else
			resultRelation = current;
		}
	return resultRelation;
	}

class Optimize
:	public virtual Visitor_ExprRep_Relation_Binary_DoTransform
,	public virtual Visitor_ExprRep_Relation_Unary_DoTransform
	{
public:
// From Visitor_ExprRep_Relation_Unary_Select
	virtual void Visit_ExprRep_Relation_Unary_Select(ZRef<ExprRep_Relation_Unary_Select> iRep);
	};

void Optimize::Visit_ExprRep_Relation_Unary_Select(ZRef<ExprRep_Relation_Unary_Select> iRep)
	{
	ZRef<ExprRep_Relation> newRep =
		this->DoTransform(iRep->GetExprRep_Relation()).DynamicCast<ExprRep_Relation>();

	fResult = spConvertSelect(newRep, iRep->GetExprRep_Logic());
	}

} // anonymous namespace

namespace ZQL {

ZRef<ExprRep_Relation> sOptimize(ZRef<ExprRep_Relation> iRep)
	{ return Optimize().DoTransform(iRep).DynamicCast<ExprRep_Relation>(); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
