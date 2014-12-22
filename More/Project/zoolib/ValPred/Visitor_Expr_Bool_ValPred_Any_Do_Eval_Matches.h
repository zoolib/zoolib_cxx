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

#ifndef __ZooLib_ValPred_Visitor_Expr_Bool_ValPred_Any_Do_Eval_Matches_h__
#define __ZooLib_ValPred_Visitor_Expr_Bool_ValPred_Any_Do_Eval_Matches_h__ 1
#include "zconfig.h"

#include "zoolib/Val_Any.h"

#include "zoolib/Expr/Visitor_Expr_Bool_Do_Eval.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h"

#include <set>

namespace ZooLib {

// =================================================================================================
// MARK: - Visitor_Expr_Bool_ValPred_Any_Do_Eval_Matches

class Visitor_Expr_Bool_ValPred_Any_Do_Eval_Matches
:	public virtual Visitor_Expr_Bool_Do_Eval
,	public virtual Visitor_Expr_Bool_ValPred
	{
public:
	Visitor_Expr_Bool_ValPred_Any_Do_Eval_Matches(const Val_Any& iVal);

// From Visitor_Expr_Bool_ValPred
	virtual void Visit_Expr_Bool_ValPred(const ZRef<Expr_Bool_ValPred>& iExpr);

private:
	const Val_Any& fVal;
	};

bool sMatches(const ZRef<Expr_Bool>& iExpr, const Val_Any& iVal);

} // namespace ZooLib

#endif // __ZooLib_ValPred_Visitor_Expr_Bool_ValPred_Any_Do_Eval_Matches_h__
