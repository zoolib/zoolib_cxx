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

#include "zoolib/Util_STL_map.h"

#include "zoolib/Expr/Visitor_Expr_Op_Do_Transform_T.h"

//#include "zoolib/RelationalAlgebra/Expr_Rel_Embed.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"
#include "zoolib/RelationalAlgebra/Transform_ConsolidateRenames.h"

namespace ZooLib {
namespace RelationalAlgebra {

using namespace Util_STL;

// =================================================================================================
#pragma mark -
#pragma mark Transform_ConsolidateRenames

class Transform_ConsolidateRenames
:	public virtual Visitor_Expr_Op_Do_Transform_T<Expr_Rel>
//##,	public virtual Visitor_Expr_Rel_Embed
,	public virtual Visitor_Expr_Rel_Rename
	{
	typedef Visitor_Expr_Op_Do_Transform_T<Expr_Rel> inherited;
public:
// From Visitor_Expr_OpX_T via Visitor_Expr_Op_DoTransform_T
	virtual void Visit_Expr_Op0(const ZRef<Expr_Op0_T<Expr_Rel> >& iExpr);
	virtual void Visit_Expr_Op1(const ZRef<Expr_Op1_T<Expr_Rel> >& iExpr);
	virtual void Visit_Expr_Op2(const ZRef<Expr_Op2_T<Expr_Rel> >& iExpr);

// From Visitor_Expr_Rel_XXX
	virtual void Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr);
//##	virtual void Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>& iExpr);

	Rename fRename;
	};

void Transform_ConsolidateRenames::Visit_Expr_Op0(const ZRef<Expr_Op0_T<Expr_Rel> >& iExpr)
	{
	SaveSetRestore<Rename> ssr(fRename, Rename());
	inherited::Visit_Expr_Op0(iExpr);
	}

void Transform_ConsolidateRenames::Visit_Expr_Op1(const ZRef<Expr_Op1_T<Expr_Rel> >& iExpr)
	{
	SaveSetRestore<Rename> ssr(fRename, Rename());
	inherited::Visit_Expr_Op1(iExpr);
	}

void Transform_ConsolidateRenames::Visit_Expr_Op2(const ZRef<Expr_Op2_T<Expr_Rel> >& iExpr)
	{
	SaveSetRestore<Rename> ssr(fRename, Rename());
	inherited::Visit_Expr_Op2(iExpr);
	}

void Transform_ConsolidateRenames::Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
	{
	string8 newName = iExpr->GetNew();

	if (ZQ<string8> theQ = sQGetErase(fRename, newName))
		newName = *theQ;

	const string8 oldName = iExpr->GetOld();

	sInsertMust(fRename, oldName, newName);

	ZRef<Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	if (ZQ<string8> theQ = sQGet(fRename, oldName))
		{
		if (*theQ == newName)
			{
			if (newName != oldName)
				newOp0 = sRename(newOp0, newName, oldName);
			}
		}
	this->pSetResult(newOp0);
	}

// =================================================================================================
#pragma mark -
#pragma mark RelationalAlgebra::sTransform_ConsolidateRenames
ZRef<Expr_Rel> sTransform_ConsolidateRenames(const ZRef<Expr_Rel>& iRel)
	{
	return Transform_ConsolidateRenames().Do(iRel);
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
