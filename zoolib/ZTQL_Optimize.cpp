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

#include "zoolib/ZTQL_Optimize.h"

NAMESPACE_ZOOLIB_USING

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL, sConvertSelect

// Turns a Node_Select into a tree of Node_Restrict and Node_Union.

using ZTQL::Condition;
using ZTQL::LogOp;
using ZTQL::RelHead;

using std::vector;

typedef vector<Condition> CondSect;
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

static void sGather(ZRef<LogOp> iLogOp, CondUnion& oResult)
	{
	using ZTQL::LogOp_True;
	using ZTQL::LogOp_False;
	using ZTQL::LogOp_And;
	using ZTQL::LogOp_Or;
	using ZTQL::LogOp_Condition;

	ZAssert(oResult.empty());

	if (ZRef<LogOp_And> lo = ZRefDynamicCast<LogOp_And>(iLogOp))
		{
		CondUnion left;
		sGather(lo->GetLHS(), left);
		CondUnion right;
		sGather(lo->GetRHS(), right);
		sCrossMultiply(left, right, oResult);
		}
	else if (ZRef<LogOp_Or> lo = ZRefDynamicCast<LogOp_Or>(iLogOp))
		{
		CondUnion left;
		sGather(lo->GetLHS(), left);
		CondUnion right;
		sGather(lo->GetRHS(), right);
		oResult = left;
		oResult.insert(oResult.end(), right.begin(), right.end());
		}
	else if (ZRef<LogOp_Condition> lo = ZRefDynamicCast<LogOp_Condition>(iLogOp))
		{
		oResult.resize(1);
		oResult[0].push_back(lo->GetCondition());
		}
	else if (ZRef<LogOp_True> lo = ZRefDynamicCast<LogOp_True>(iLogOp))
		{
		oResult.resize(1);
		}
	else if (ZRef<LogOp_False> lo = ZRefDynamicCast<LogOp_False>(iLogOp))
		{
		// Do nothing.
		}
	else if (iLogOp)
		{
		// Unknown LogOp
		ZUnimplemented();
		}
	else
		{
		// Nil LogOp is equivalent to false.
		}
	}

static ZRef<ZTQL::Node> sConvertSelect(ZRef<ZTQL::Node> iNode, ZRef<LogOp> iLogOp)
	{
	using ZTQL::Node;
	using ZTQL::Node_Restrict;
	using ZTQL::Node_Union;

	if (!iNode)
		return ZRef<Node>();

	CondUnion result;
	sGather(iLogOp, result);

	ZRef<Node> resultNode;
	for (CondUnion::const_iterator iterUnion = result.begin();
		iterUnion != result.end(); ++iterUnion)
		{
		ZRef<Node> currentNode = iNode;
		for (CondSect::const_iterator iterSect = iterUnion->begin();
			iterSect != iterUnion->end(); ++iterSect)
			{
			currentNode = new Node_Restrict(currentNode, *iterSect);
			}

		if (resultNode)
			resultNode = new Node_Union(resultNode, currentNode);
		else
			resultNode = currentNode;
		}
	return resultNode;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL

static ZRef<ZTQL::Node> sBreakUpSelect(ZRef<ZTQL::Node> iNode)
	{
	if (ZRef<ZTQL::Node_All> theNode =
		ZRefDynamicCast<ZTQL::Node_All>(iNode))
		{
		return theNode;
		}
	else if (ZRef<ZTQL::Node_Difference> theNode =
		ZRefDynamicCast<ZTQL::Node_Difference>(iNode))
		{
		return new ZTQL::Node_Difference(
			ZTQL::sOptimize(theNode->GetNodeA()),
			ZTQL::sOptimize(theNode->GetNodeB()));
		}
	else if (ZRef<ZTQL::Node_Explicit> theNode =
		ZRefDynamicCast<ZTQL::Node_Explicit>(iNode))
		{
		return theNode;
		}
	else if (ZRef<ZTQL::Node_Intersect> theNode =
		ZRefDynamicCast<ZTQL::Node_Intersect>(iNode))
		{
		return new ZTQL::Node_Intersect(
			ZTQL::sOptimize(theNode->GetNodeA()),
			ZTQL::sOptimize(theNode->GetNodeB()));
		}
	else if (ZRef<ZTQL::Node_Join> theNode =
		ZRefDynamicCast<ZTQL::Node_Join>(iNode))
		{
		return new ZTQL::Node_Join(
			ZTQL::sOptimize(theNode->GetNodeA()),
			ZTQL::sOptimize(theNode->GetNodeB()));
		}
	else if (ZRef<ZTQL::Node_Project> theNode =
		ZRefDynamicCast<ZTQL::Node_Project>(iNode))
		{
		return new ZTQL::Node_Project(
			ZTQL::sOptimize(theNode->GetNode()),
			theNode->GetRelHead());
		}
	else if (ZRef<ZTQL::Node_Rename> theNode =
		ZRefDynamicCast<ZTQL::Node_Rename>(iNode))
		{
		return new ZTQL::Node_Rename(
			ZTQL::sOptimize(theNode->GetNode()),
			theNode->GetOld(), theNode->GetNew());
		}
	else if (ZRef<ZTQL::Node_Restrict> theNode =
		ZRefDynamicCast<ZTQL::Node_Restrict>(iNode))
		{
		return new ZTQL::Node_Restrict(
			ZTQL::sOptimize(theNode->GetNode()),
			theNode->GetCondition());
		}
	else if (ZRef<ZTQL::Node_Select> theNode =
		ZRefDynamicCast<ZTQL::Node_Select>(iNode))
		{
		return sConvertSelect(
			ZTQL::sOptimize(theNode->GetNode()),
			theNode->GetLogOp());
		}
	else if (ZRef<ZTQL::Node_Union> theNode =
		ZRefDynamicCast<ZTQL::Node_Union>(iNode))
		{
		return new ZTQL::Node_Union(
			ZTQL::sOptimize(theNode->GetNodeA()),
			ZTQL::sOptimize(theNode->GetNodeB()));
		}
	else
		{
		return theNode;
		}
	}

ZRef<ZTQL::Node> ZTQL::sOptimize(ZRef<ZTQL::Node> iNode)
	{
	return sBreakUpSelect(iNode);
	}
