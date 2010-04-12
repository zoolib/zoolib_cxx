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

#ifndef __ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T__
#define __ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T__
#include "zconfig.h"

#include "zoolib/ZExpr_Logic_ValCondition_T.h"
#include "zoolib/ZVisitor_Expr_Logic_DoEval.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T

template <class Val>
class ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T
:	public virtual ZVisitor_Expr_Logic_DoEval
,	public virtual ZVisitor_Expr_Logic_ValCondition_T<Val>
	{
	typedef ZVisitor_Expr_Logic_ValCondition_T<Val> inherited;
public:

	ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T(const Val& iVal);

// From ZVisitor_Expr_Logic_ValCondition_T
	virtual void Visit_Logic_ValCondition(ZRef<ZExpr_Logic_ValCondition_T<Val> > iExpr);

private:
	const Val& fVal;
	ZValContext fValContext;
	};

template <class Val>
ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T<Val>::
ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T(const Val& iVal)
:	fVal(iVal)
	{}

template <class Val>
void ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T<Val>::Visit_Logic_ValCondition(
	ZRef<ZExpr_Logic_ValCondition_T<Val> > iExpr)
	{ fResult = iExpr->GetValCondition().Matches(fValContext, fVal); }

template <class Val>
bool sMatches_T(const ZRef<ZExpr_Logic>& iExpr, const Val& iVal)
	{ return ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T<Val>(iVal).DoEval(iExpr); }

template <class Val>
bool sMatches_T(const ZValCondition_T<Val>& iValCondition, const Val& iVal)
	{ return sMatches_T<Val>(new ZExpr_Logic_ValCondition_T<Val>(iValCondition), iVal); }


NAMESPACE_ZOOLIB_END

#endif // __ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T__
