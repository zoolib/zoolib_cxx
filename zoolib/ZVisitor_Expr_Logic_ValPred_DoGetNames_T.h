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

#ifndef __ZVisitor_Expr_Logic_ValPred_DoGetNames_T__
#define __ZVisitor_Expr_Logic_ValPred_DoGetNames_T__
#include "zconfig.h"

#include "zoolib/ZExpr_Logic_ValPred_T.h"
#include "zoolib/ZVisitor_Do_T.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_ValPred_DoGetNames_T

template <class Val>
class ZVisitor_Expr_Logic_ValPred_DoGetNames_T
:	public virtual ZVisitor_Do_T<std::set<std::string> >
,	public virtual ZVisitor_Expr_Logic_ValPred_T<Val>
,	public virtual ZVisitor_Expr_Op1_T<ZExpr_Logic>
,	public virtual ZVisitor_Expr_Op2_T<ZExpr_Logic>
	{
public:
// From ZVisitor_Expr_Logic_ValPred_T
	virtual void Visit_Expr_Logic_ValPred(ZRef<ZExpr_Logic_ValPred_T<Val> > iExpr);

// From ZVisitor_Expr_Op1_T
	virtual void Visit_Expr_Op1(ZRef<ZExpr_Op1_T<ZExpr_Logic> > iExpr);

// From ZVisitor_Expr_Op2_T
	virtual void Visit_Expr_Op2(ZRef<ZExpr_Op2_T<ZExpr_Logic> > iExpr);
	};

template <class Val>
void ZVisitor_Expr_Logic_ValPred_DoGetNames_T<Val>::Visit_Expr_Logic_ValPred(
	ZRef<ZExpr_Logic_ValPred_T<Val> > iExpr)
	{ this->pSetResult(iExpr->GetValPred().GetNames()); }

template <class Val>
void ZVisitor_Expr_Logic_ValPred_DoGetNames_T<Val>::Visit_Expr_Op1(
	ZRef<ZExpr_Op1_T<ZExpr_Logic> > iExpr)
	{ this->pSetResult(this->Do(iExpr->GetOp0())); }

template <class Val>
void ZVisitor_Expr_Logic_ValPred_DoGetNames_T<Val>::Visit_Expr_Op2(
	ZRef<ZExpr_Op2_T<ZExpr_Logic> > iExpr)
	{ this->pSetResult(ZUtil_STL_set::sOr(this->Do(iExpr->GetOp0()), this->Do(iExpr->GetOp0()))); }

} // namespace ZooLib

#endif // __ZVisitor_Expr_Logic_ValPred_DoToStrim__
