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

#ifndef __ZExpr_Bool_ValPred_Any__
#define __ZExpr_Bool_ValPred_Any__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Bool_ValPred_T.h"
#include "zoolib/ZValPred_Any.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_DoEval_Matches_T.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_DoGetNames_T.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool_ValPred

typedef ZExpr_Bool_ValPred_T<ZVal_Any> ZExpr_Bool_ValPred_Any;

typedef ZVisitor_Expr_Bool_ValPred_T<ZVal_Any> ZVisitor_Expr_Bool_ValPred_Any;


inline std::set<std::string> sGetNames(const ZRef<ZExpr_Bool>& iExpr)
	{ return ZVisitor_Expr_Bool_ValPred_DoGetNames_T<ZVal_Any>().Do(iExpr); }

inline bool sMatches(const ZRef<ZExpr_Bool>& iExpr, const ZVal_Any& iVal)
	{ return ZVisitor_Expr_Bool_ValPred_DoEval_Matches_T<ZVal_Any>(iVal).Do(iExpr); }

inline bool sMatches(const ZValPred_Any& iValPred, const ZVal_Any& iVal)
	{ return sMatches(new ZExpr_Bool_ValPred_Any(iValPred), iVal); }

} // namespace ZooLib

#endif // __ZExpr_Bool_ValPred_Any__
