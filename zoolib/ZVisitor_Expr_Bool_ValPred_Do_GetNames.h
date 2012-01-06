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

#ifndef __ZVisitor_Expr_Bool_ValPred_Do_GetNames_h__
#define __ZVisitor_Expr_Bool_ValPred_Do_GetNames_h__
#include "zconfig.h"

#include "zoolib/ZExpr_Bool_ValPred.h"
#include "zoolib/ZVisitor_Do_T.h"

#include <set>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_ValPred_Do_GetNames

class ZVisitor_Expr_Bool_ValPred_Do_GetNames
:	public virtual ZVisitor_Do_T<std::set<std::string> >
,	public virtual ZVisitor_Expr_Bool_ValPred
,	public virtual ZVisitor_Expr_Op1_T<ZExpr_Bool>
,	public virtual ZVisitor_Expr_Op2_T<ZExpr_Bool>
	{
public:
// From ZVisitor_Expr_Bool_ValPred
	virtual void Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred>& iExpr);

// From ZVisitor_Expr_Op1_T
	virtual void Visit_Expr_Op1(const ZRef<ZExpr_Op1_T<ZExpr_Bool> >& iExpr);

// From ZVisitor_Expr_Op2_T
	virtual void Visit_Expr_Op2(const ZRef<ZExpr_Op2_T<ZExpr_Bool> >& iExpr);
	};

std::set<std::string> sGetNames(const ZRef<ZExpr_Bool>& iExpr);

} // namespace ZooLib

#endif // __ZVisitor_Expr_Bool_ValPred_Do_GetNames_h__
