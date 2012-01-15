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

#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/zra/ZRA_Transform_ConsolidateRenames.h"

namespace ZooLib {
namespace ZRA {

using namespace ZUtil_STL;

// =================================================================================================
// MARK: - Transform_ConsolidateRenames

void Transform_ConsolidateRenames::Visit_Expr_Op0(const ZRef<ZExpr_Op0_T<Expr_Rel> >& iExpr)
	{
	ZSetRestore_T<Rename> sr(fRename);
	inherited::Visit_Expr_Op0(iExpr);
	}

void Transform_ConsolidateRenames::Visit_Expr_Op1(const ZRef<ZExpr_Op1_T<Expr_Rel> >& iExpr)
	{
	ZSetRestore_T<Rename> sr(fRename);
	inherited::Visit_Expr_Op1(iExpr);
	}

void Transform_ConsolidateRenames::Visit_Expr_Op2(const ZRef<ZExpr_Op2_T<Expr_Rel> >& iExpr)
	{
	ZSetRestore_T<Rename> sr(fRename);
	inherited::Visit_Expr_Op2(iExpr);
	}

void Transform_ConsolidateRenames::Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
	{
	string8 newName = iExpr->GetNew();

	if (ZQ<string8> theQ = sQErase(fRename, newName))
		newName = theQ.Get();

	const string8 oldName = iExpr->GetOld();

	sInsertMustNotContain(1, fRename, oldName, newName);

	ZRef<Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	if (ZQ<string8> theQ = sQGet(fRename, oldName))
		{
		if (theQ.Get() == newName)
			{
			if (newName != oldName)
				newOp0 = sRename(newOp0, newName, oldName);
			}
		}
	this->pSetResult(newOp0);
	}

} // namespace ZRA
} // namespace ZooLib
