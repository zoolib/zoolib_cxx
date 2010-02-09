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

#include "zoolib/ZExpr_Query.h"
#include "zoolib/ZExpr_ValCondition.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL, sConvertSelect

// Turns a Select into a tree of Restrict and Union.

using std::vector;

typedef vector<ZValCondition> CondSect;
typedef vector<CondSect> CondUnion;

static void sCrossMultiply(const CondUnion& iLeft, const CondUnion& iRight, CondUnion& oResult)
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

static void sGather(ZRef<ZExprRep_Logical> iRep, CondUnion& oResult)
	{
	ZAssert(oResult.empty());

	if (ZRef<ZExprRep_Logical_And> lo = ZRefDynamicCast<ZExprRep_Logical_And>(iRep))
		{
		CondUnion left;
		sGather(lo->GetLHS(), left);
		CondUnion right;
		sGather(lo->GetRHS(), right);
		sCrossMultiply(left, right, oResult);
		}
	else if (ZRef<ZExprRep_Logical_Or> lo = ZRefDynamicCast<ZExprRep_Logical_Or>(iRep))
		{
		CondUnion left;
		sGather(lo->GetLHS(), left);
		CondUnion right;
		sGather(lo->GetRHS(), right);
		oResult = left;
		oResult.insert(oResult.end(), right.begin(), right.end());
		}
	else if (ZRef<ZExprRep_ValCondition> lo = ZRefDynamicCast<ZExprRep_ValCondition>(iRep))
		{
		oResult.resize(1);
		oResult[0].push_back(lo->GetValCondition());
		}
	else if (ZRef<ZExprRep_Logical_True> lo = ZRefDynamicCast<ZExprRep_Logical_True>(iRep))
		{
		oResult.resize(1);
		}
	else if (ZRef<ZExprRep_Logical_False> lo = ZRefDynamicCast<ZExprRep_Logical_False>(iRep))
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

static ZRef<ZExprRep_Relation> sConvertSelect(
	ZRef<ZExprRep_Relation> iRelational, ZRef<ZExprRep_Logical> iLogical)
	{
	if (!iRelational)
		return ZRef<ZExprRep_Relation>();

	CondUnion resultLogical;
	sGather(iLogical, resultLogical);

	ZRef<ZExprRep_Relation> resultRelational;
	for (CondUnion::const_iterator iterUnion = resultLogical.begin();
		iterUnion != resultLogical.end(); ++iterUnion)
		{
		ZRef<ZExprRep_Relation> current = iRelational;
		for (CondSect::const_iterator iterSect = iterUnion->begin();
			iterSect != iterUnion->end(); ++iterSect)
			{
			current = new ZExprRep_Restrict(*iterSect, current);
			}

		if (resultRelational)
			resultRelational = new ZExprRep_Relation_Union(current, resultRelational);
		else
			resultRelational = current;
		}
	return resultRelational;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL

namespace ZANONYMOUS {

class Optimize : public ZQueryTransformer
	{
public:
	virtual ZRef<ZExprRep_Relation> Transform_Select(ZRef<ZExprRep_Select> iRep);
	};

ZRef<ZExprRep_Relation> Optimize::Transform_Select(ZRef<ZExprRep_Select> iRep)
	{
	ZRef<ZExprRep_Relation> newRep = this->Transform(iRep->GetExpr_Relation());
	return sConvertSelect(newRep, iRep->GetExpr_Logical());
	}

} // anonymous namespace

ZRef<ZExprRep_Relation> ZTQL::sOptimize(ZRef<ZExprRep_Relation> iRep)
	{ return Optimize().Transform(iRep); }

NAMESPACE_ZOOLIB_END
