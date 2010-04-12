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

#ifndef __ZExpr_Logic_ValCondition__
#define __ZExpr_Logic_ValCondition__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Logic_ValCondition_T.h"
#include "zoolib/ZValCondition.h"
#include "zoolib/ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T.h"
#include "zoolib/ZVisitor_Expr_Logic_ValCondition_DoGetNames_T.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_ValCondition

typedef ZExpr_Logic_ValCondition_T<ZVal_Expr> ZExpr_Logic_ValCondition;

typedef ZVisitor_Expr_Logic_ValCondition_T<ZVal_Expr> ZVisitor_Expr_Logic_ValCondition;


inline std::set<std::string> sGetNames(const ZRef<ZExpr_Logic>& iExpr)
	{ return ZVisitor_Expr_Logic_ValCondition_DoGetNames_T<ZVal_Expr>().DoGetNames(iExpr); }

inline bool sMatches(const ZRef<ZExpr_Logic>& iExpr, const ZVal_Expr& iVal)
	{ return ZVisitor_Expr_Logic_ValCondition_DoEval_Matches_T<ZVal_Expr>(iVal).DoEval(iExpr); }

inline bool sMatches(const ZValCondition& iValCondition, const ZVal_Expr& iVal)
	{ return sMatches(new ZExpr_Logic_ValCondition(iValCondition), iVal); }

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Logic_ValCondition__
