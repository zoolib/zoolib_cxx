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

#include "zoolib/tql/ZTQL_Optimize.h"

//#include "zoolib/zql/ZQL_Expr_Query.h"
#include "zoolib/zql/ZQL_Expr_Restrict.h"
#include "zoolib/zql/ZQL_Visitor_ExprRep_Select_Transform.h"
#include "zoolib/ZExpr_ValCondition.h"

NAMESPACE_ZOOLIB_BEGIN
using namespace ZQL;

// =================================================================================================
#pragma mark -
#pragma mark * ZQL, spConvertSelect

// Turns a Select into a tree of Restrict and Union.

using std::vector;

typedef vector<ZValCondition> CondSect;
typedef vector<CondSect> CondUnion;

static void spCrossMultiply(const CondUnion& iLeft, const CondUnion& iRight, CondUnion& oResult)
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

static void spGather(ZRef<ZExprRep_Logic> iRep, CondUnion& oResult)
	{
	ZAssert(oResult.empty());

	if (ZRef<ZExprRep_Logic_And> lo = ZRefDynamicCast<ZExprRep_Logic_And>(iRep))
		{
		CondUnion left;
		spGather(lo->GetLHS(), left);
		CondUnion right;
		spGather(lo->GetRHS(), right);
		spCrossMultiply(left, right, oResult);
		}
	else if (ZRef<ZExprRep_Logic_Or> lo = ZRefDynamicCast<ZExprRep_Logic_Or>(iRep))
		{
		CondUnion left;
		spGather(lo->GetLHS(), left);
		CondUnion right;
		spGather(lo->GetRHS(), right);
		oResult = left;
		oResult.insert(oResult.end(), right.begin(), right.end());
		}
	else if (ZRef<ZExprRep_ValCondition> lo = ZRefDynamicCast<ZExprRep_ValCondition>(iRep))
		{
		oResult.resize(1);
		oResult[0].push_back(lo->GetValCondition());
		}
	else if (ZRef<ZExprRep_Logic_True> lo = ZRefDynamicCast<ZExprRep_Logic_True>(iRep))
		{
		oResult.resize(1);
		}
	else if (ZRef<ZExprRep_Logic_False> lo = ZRefDynamicCast<ZExprRep_Logic_False>(iRep))
		{
		// Do nothing.
		}
	else if (iRep)
		{
		// Unknown LogOp
		ZUnimplemented();
		}
	else
		{
		// Nil LogOp is equivalent to false.
		}
	}

static ZRef<ExprRep_Relation> spConvertSelect(
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
			current = new ExprRep_Restrict(*iterSect, current);
			}

		if (resultRelation)
			resultRelation = new ExprRep_Relation_Union(current, resultRelation);
		else
			resultRelation = current;
		}
	return resultRelation;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZQL

namespace ZANONYMOUS {

class Optimize
:	public virtual Visitor_ExprRep_Select_Transform
	{
public:
// From Visitor_ExprRep_Select
	virtual bool Visit_Select(ZRef<ExprRep_Select> iRep);
	};

bool Optimize::Visit_Select(ZRef<ExprRep_Select> iRep)
	{
	ZRef<ExprRep_Relation> newRep = this->Transform(iRep->GetExprRep_Relation()).DynamicCast<ExprRep_Relation>();
	fResult = spConvertSelect(newRep, iRep->GetExprRep_Logic());
	return true;
	}

} // anonymous namespace

namespace ZQL {

ZRef<ExprRep_Relation> sOptimize(ZRef<ExprRep_Relation> iRep)
	{ return Optimize().Transform(iRep).DynamicCast<ExprRep_Relation>(); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
