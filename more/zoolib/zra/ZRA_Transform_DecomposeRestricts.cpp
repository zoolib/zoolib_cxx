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

#include "zoolib/ZUtil_Expr_Bool_CNF.h"
#include "zoolib/zra/ZRA_Transform_DecomposeRestricts.h"

using std::set;

namespace ZooLib {
namespace ZRA {

// =================================================================================================
// MARK: - Transform_DecomposeRestricts

void Transform_DecomposeRestricts::Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr)
	{
	using Util_Expr_Bool::CNF;
	using Util_Expr_Bool::Clause;

	ZRef<Expr_Rel> theRel = this->Do(iExpr->GetOp0());
	const CNF theCNF = Util_Expr_Bool::sAsCNF(iExpr->GetExpr_Bool());
	for (CNF::const_iterator ii = theCNF.begin(); ii != theCNF.end(); ++ii)
		{
		ZRef<ZExpr_Bool> newBool;
		for (Clause::const_iterator jj = ii->begin(); jj != ii->end(); ++jj)
			newBool |= jj->Get();
		theRel &= newBool;
		}

	this->pSetResult(theRel);
	}

} // namespace ZRA
} // namespace ZooLib
