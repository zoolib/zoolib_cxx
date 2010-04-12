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

#include "zoolib/ZExpr_Logic_ValCondition.h"
#include "zoolib/ZVisitor_Expr_Logic_DoTransform.h"
#include "zoolib/tql/ZTQL_Optimize.h"
#include "zoolib/zql/ZQL_Expr_Rel_Binary_Union.h"
#include "zoolib/zql/ZQL_Expr_Rel_Concrete.h"
#include "zoolib/zql/ZQL_Expr_Rel_Unary_Restrict.h"
#include "zoolib/zql/ZQL_Expr_Rel_Unary_Select.h"
#include "zoolib/zql/ZQL_Visitor_Expr_Rel_Binary_DoTransform.h"
#include "zoolib/zql/ZQL_Visitor_Expr_Rel_Unary_DoTransform.h"

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

void spGather(ZRef<ZExpr_Logic> iExpr, CondUnion& oResult);

class Gather
:	public virtual ZVisitor_Expr_Logic_DoTransform
,	public virtual ZVisitor_Expr_Logic_ValCondition
	{
public:
	Gather(CondUnion& oResult);

//	From ZVisitor_Expr_Logic_DoTransform
	virtual void Visit_Logic_True(ZRef<ZExpr_Logic_True> iExpr);
	virtual void Visit_Logic_False(ZRef<ZExpr_Logic_False> iExpr);
	virtual void Visit_Logic_Not(ZRef<ZExpr_Logic_Not> iExpr);
	virtual void Visit_Logic_And(ZRef<ZExpr_Logic_And> iExpr);
	virtual void Visit_Logic_Or(ZRef<ZExpr_Logic_Or> iExpr);	

// From ZVisitor_Expr_Logic_ValCondition
	virtual void Visit_Logic_ValCondition(ZRef<ZExpr_Logic_ValCondition> iExpr);

private:
	CondUnion& fResult;
	};

Gather::Gather(CondUnion& oResult)
:	fResult(oResult)
	{}

void Gather::Visit_Logic_True(ZRef<ZExpr_Logic_True> iExpr)
	{ fResult.resize(1); }

void Gather::Visit_Logic_False(ZRef<ZExpr_Logic_False> iExpr)
	{
	ZAssert(fResult.empty());
//	fResult.clear();
	}

void Gather::Visit_Logic_Not(ZRef<ZExpr_Logic_Not> iExpr)
	{ ZUnimplemented(); }

void Gather::Visit_Logic_And(ZRef<ZExpr_Logic_And> iExpr)
	{
	CondUnion left;
	spGather(iExpr->GetLHS(), left);
	CondUnion right;
	spGather(iExpr->GetRHS(), right);
	spCrossMultiply(left, right, fResult);
	}

void Gather::Visit_Logic_Or(ZRef<ZExpr_Logic_Or> iExpr)
	{
	CondUnion left;
	spGather(iExpr->GetLHS(), left);
	CondUnion right;
	spGather(iExpr->GetRHS(), right);
	fResult.swap(left);
	fResult.insert(fResult.end(), right.begin(), right.end());
	}

void Gather::Visit_Logic_ValCondition(ZRef<ZExpr_Logic_ValCondition> iExpr)
	{
	fResult.resize(1);
	fResult[0].push_back(iExpr->GetValCondition());
	}

void spGather(ZRef<ZExpr_Logic> iExpr, CondUnion& oResult)
	{
	ZAssert(oResult.empty());
	Gather theGather(oResult);
	iExpr->Accept(theGather);
	}

ZRef<Expr_Rel> spConvertSelect(
	ZRef<Expr_Rel> iRelation, ZRef<ZExpr_Logic> iLogical)
	{
	if (!iRelation)
		return ZRef<Expr_Rel>();

	CondUnion resultLogical;
	spGather(iLogical, resultLogical);

	ZRef<Expr_Rel> resultRelation;
	for (CondUnion::const_iterator iterUnion = resultLogical.begin();
		iterUnion != resultLogical.end(); ++iterUnion)
		{
		ZRef<Expr_Rel> current = iRelation;
		for (CondSect::const_iterator iterSect = iterUnion->begin();
			iterSect != iterUnion->end(); ++iterSect)
			{
			current = new Expr_Rel_Unary_Restrict(current, *iterSect);
			}

		if (resultRelation)
			resultRelation = new Expr_Rel_Binary_Union(current, resultRelation);
		else
			resultRelation = current;
		}
	return resultRelation;
	}

class Optimize
:	public virtual Visitor_Expr_Rel_Binary_DoTransform
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Unary_DoTransform
,	public virtual Visitor_Expr_Rel_Unary_Select
	{
public:
// From Visitor_Expr_Rel_Concrete
	virtual void Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr);

// From Visitor_Expr_Rel_Unary_Select
	virtual void Visit_Expr_Rel_Unary_Select(ZRef<Expr_Rel_Unary_Select> iExpr);
	};

void Optimize::Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr)
	{ fResult = iExpr; }

void Optimize::Visit_Expr_Rel_Unary_Select(ZRef<Expr_Rel_Unary_Select> iExpr)
	{
	ZRef<Expr_Rel> newExpr =
		this->DoTransform(iExpr->GetExpr_Rel()).DynamicCast<Expr_Rel>();

	fResult = spConvertSelect(newExpr, iExpr->GetExpr_Logic());
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZQL::sOptimize

namespace ZQL {

ZRef<Expr_Rel> sOptimize(ZRef<Expr_Rel> iExpr)
	{ return Optimize().DoTransform(iExpr).DynamicCast<Expr_Rel>(); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
