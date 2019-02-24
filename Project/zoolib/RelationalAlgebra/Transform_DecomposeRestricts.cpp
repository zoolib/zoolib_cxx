/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/Expr/Util_Expr_Bool_CNF.h"

#include "zoolib/RelationalAlgebra/Transform_DecomposeRestricts.h"

using std::set;

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Transform_DecomposeRestricts

void Transform_DecomposeRestricts::Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr)
	{
	ZRef<Expr_Rel> theRel = this->Do(iExpr->GetOp0());

	foreacha (entry, Util_Expr_Bool::sAsCNF(iExpr->GetExpr_Bool()))
		{
		ZRef<Expr_Bool> newBool;
		foreacha (entryj, entry)
			newBool |= entryj.Get();
		theRel &= newBool;
		}

	this->pSetResult(theRel);
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
