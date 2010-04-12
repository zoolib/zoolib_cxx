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

#ifndef __ZVisitor_Expr_Logic_ValCondition_DoGetNames_T__
#define __ZVisitor_Expr_Logic_ValCondition_DoGetNames_T__
#include "zconfig.h"

#include "zoolib/ZExpr_Logic_ValCondition_T.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_ValCondition_DoGetNames_T

template <class Val>
class ZVisitor_Expr_Logic_ValCondition_DoGetNames_T
:	public ZVisitor_Expr_Logic_ValCondition_T<Val>
	{
public:
// From ZVisitor_Expr_Logic via ZVisitor_Expr_Logic_ValCondition_T
	virtual void Visit_Logic_Not(ZRef<ZExpr_Logic_Not> iExpr);
	virtual void Visit_Logic_And(ZRef<ZExpr_Logic_And> iExpr);
	virtual void Visit_Logic_Or(ZRef<ZExpr_Logic_Or> iExpr);

// From ZVisitor_Expr_Logic_ValCondition_T
	virtual void Visit_Logic_ValCondition(ZRef<ZExpr_Logic_ValCondition_T<Val> > iExpr);

// Our protocol
	std::set<std::string> DoGetNames(ZRef<ZExpr_Logic> iExpr);

protected:
	std::set<std::string> fResult;
	};

template <class Val>
void ZVisitor_Expr_Logic_ValCondition_DoGetNames_T<Val>::Visit_Logic_Not(
	ZRef<ZExpr_Logic_Not> iExpr)
	{ fResult = this->DoGetNames(iExpr); }

template <class Val>
void ZVisitor_Expr_Logic_ValCondition_DoGetNames_T<Val>::Visit_Logic_And(
	ZRef<ZExpr_Logic_And> iExpr)
	{
	std::set<std::string> result;
	ZUtil_STL_set::sOr(this->DoGetNames(iExpr->GetLHS()), this->DoGetNames(iExpr->GetRHS()), result);
	fResult.swap(result);
	}

template <class Val>
void ZVisitor_Expr_Logic_ValCondition_DoGetNames_T<Val>::Visit_Logic_Or(
	ZRef<ZExpr_Logic_Or> iExpr)
	{
	std::set<std::string> result;
	ZUtil_STL_set::sOr(this->DoGetNames(iExpr->GetLHS()), this->DoGetNames(iExpr->GetRHS()), result);
	fResult.swap(result);
	}

template <class Val>
void ZVisitor_Expr_Logic_ValCondition_DoGetNames_T<Val>::Visit_Logic_ValCondition(
	ZRef<ZExpr_Logic_ValCondition_T<Val> > iExpr)
	{ fResult = iExpr->GetValCondition().GetNames(); }

template <class Val>
std::set<std::string> ZVisitor_Expr_Logic_ValCondition_DoGetNames_T<Val>::DoGetNames(
	ZRef<ZExpr_Logic> iExpr)
	{
	std::set<std::string> theResult;
	if (iExpr)
		{
		iExpr->Accept(*this);
		std::swap(theResult, fResult);
		}
	return theResult;
	}

NAMESPACE_ZOOLIB_END

#endif // __ZVisitor_Expr_Logic_ValCondition_DoToStrim__
