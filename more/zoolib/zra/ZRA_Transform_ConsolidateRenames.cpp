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

// =================================================================================================
#pragma mark -
#pragma mark * Transform_ConsolidateRenames

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
	ZRef<Expr_Rel> oldOp0 = iExpr->GetOp0();
	const string8 oldName = iExpr->GetOld();
	const string8 newName = iExpr->GetNew();
	if (ZQ<string8> theQ = ZUtil_STL::sGetIfContains(fRename, newName))
		{
		if (theQ.Get() == oldName)
			{
			// Parent has the inverse. Erase it.
			ZUtil_STL::sEraseMustContain(1, fRename, newName);
			// And our result is a transform of what we wrapped.
			ZRef<Expr_Rel> newOp0 = this->Do(oldOp0);
			this->pSetResult(newOp0);
			return;
			}			
		}

	// Record what rename we're doing, for a child to look for.
	ZUtil_STL::sInsertMustNotContain(1, fRename, oldName, newName);

	// Apply the transform to our wrapped expr.
	ZRef<Expr_Rel> newOp0 = this->Do(oldOp0);

	// If no child removed our mapping, then we should return 
	if (!ZUtil_STL::sContains(fRename, oldName))
		{
		// A child removed our mapping and dropped itself from the chain, so we do the same.
		this->pSetResult(newOp0);
		}
	else
		{
		// No child had a compensating rename.
		if (oldOp0 == newOp0)
			this->pSetResult(iExpr);
		else
			this->pSetResult(iExpr->Clone(newOp0));
		}
	}

} // namespace ZRA
} // namespace ZooLib
