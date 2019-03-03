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

#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_Any_ToStrim.h"

#include "zoolib/ValPred/Util_Strim_ValPred_Any.h"

#include "zoolib/pdesc.h"
#if defined(ZMACRO_pdesc)
	#include "zoolib/StdIO.h"
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_ValPred_ToStrim

void Visitor_Expr_Bool_ValPred_Any_ToStrim::Visit_Expr_Bool_ValPred(
	const ZRef<Expr_Bool_ValPred>& iRep)
	{ Util_Strim_ValPred_Any::sToStrim(iRep->GetValPred(), pStrimW()); }

} // namespace ZooLib

// =================================================================================================
#pragma mark - pdesc

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const ZRef<Expr_Bool>& iExpr)
	{
	Visitor_Expr_Bool_ValPred_Any_ToStrim().ToStrim(sDefault(), StdIO::sChan_UTF_Err, iExpr);
	}

#endif // defined(ZMACRO_pdesc)
