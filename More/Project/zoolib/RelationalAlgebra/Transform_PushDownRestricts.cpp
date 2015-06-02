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

#include "zoolib/Util_STL_set.h"
#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/ValPred/Util_Expr_Bool_ValPred_Rename.h"
#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_Do_GetNames.h"

#include "zoolib/RelationalAlgebra/Transform_PushDownRestricts.h"

namespace ZooLib {
namespace RelationalAlgebra {

//##using std::vector;

using namespace Util_STL;

// =================================================================================================
#pragma mark -
#pragma mark Transform_PushDownRestricts

void Transform_PushDownRestricts::Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr)
	{ this->pHandleIt(sRelHead(iExpr->GetColName()), iExpr->SelfOrClone(this->Do(iExpr->GetOp0()))); }

void Transform_PushDownRestricts::Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr)
	{ this->pHandleIt(sRelHead(iExpr->GetConcreteHead()), iExpr); }

void Transform_PushDownRestricts::Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr)
	{ this->pHandleIt(sRelHead(iExpr->GetColName()), iExpr); }

void Transform_PushDownRestricts::Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>& iExpr)
	{
	// I think this needs to work in a fashion akin to product, because we may have a restrict
	// in op1 that refers to a name from op0.

	ZRef<Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	ZRef<Expr_Rel> newOp1 = iExpr->GetOp1();

	this->pHandleIt(sRelHead(iExpr->GetColName()), iExpr->SelfOrClone(newOp0, newOp1));
	}

void Transform_PushDownRestricts::Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr)
	{
	RelHead theRelHead;

	{ // Scope for ssr
	SaveSetRestore<RelHead> ssr(fRelHead, RelHead());
	ZRef<Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());
	ZRef<Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());

	theRelHead = fRelHead;
	ZRef<Expr_Rel> result = iExpr->SelfOrClone(newOp0, newOp1);

	// Examine restricts, see which were touched
	foreachi (iter, fRestricts)
		{
		Restrict& theRestrict = **iter;

		if (theRestrict.fCountTouching != theRestrict.fCountSubsuming)
			{
			// Our children touched, but did not individually subsume this rel.
			const RelHead exprNames = sGetNames(theRestrict.fExpr_Bool);
			const RelHead intersection = exprNames & theRelHead;
			if (intersection.size() && intersection.size() == exprNames.size())
				{
				// The product as a whole does touch and subsume the rel. Override whatever
				// values were in the counts with an arbitrary matching pair.
				theRestrict.fCountTouching = 8888;
				theRestrict.fCountSubsuming = 8888;
				result = result & theRestrict.fExpr_Bool;
				}
			}

		}
	this->pSetResult(result);
	}

	fRelHead |= theRelHead;
	}

void Transform_PushDownRestricts::Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
	{
	const ColName& oldName = iExpr->GetOld();
	const ColName& newName = iExpr->GetNew();

	Rename new2Old;
	new2Old[newName] = oldName;

	foreachi (iter, fRestricts)
		(*iter)->fExpr_Bool = Util_Expr_Bool::sRenamed(new2Old, (*iter)->fExpr_Bool);

	Visitor_Expr_Rel_Rename::Visit_Expr_Rel_Rename(iExpr);

	Rename old2New;
	old2New[oldName] = newName;

	foreachi (iter, fRestricts)
		(*iter)->fExpr_Bool = Util_Expr_Bool::sRenamed(old2New, (*iter)->fExpr_Bool);

	if (sQErase(fRelHead, oldName))
		fRelHead |= newName;
	}

void Transform_PushDownRestricts::Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr)
	{
	Restrict theRestrict;
	theRestrict.fExpr_Bool = iExpr->GetExpr_Bool();
	theRestrict.fCountTouching = 0;
	theRestrict.fCountSubsuming = 0;
	fRestricts.push_back(&theRestrict);

	ZRef<Expr_Rel> oldOp0 = iExpr->GetOp0();
	ZRef<Expr_Rel> newOp0 = this->Do(oldOp0);

	ZAssertStop(1, fRestricts.back() == &theRestrict);
	fRestricts.pop_back();

	if (theRestrict.fCountTouching == 0)
		{
		// Superfluous condition? Should have been pulled up the tree
		// and then possibly excised. Or we should return null, as nothing can
		// satisfy the criteria.
		this->pSetResult(oldOp0);
		}
	else if (theRestrict.fCountTouching == theRestrict.fCountSubsuming)
		{
		// Every descendant that touches the restriction handles it completely
		// and we can drop this restriction out of the tree.
		this->pSetResult(newOp0);
		}
	else
		{
		// Descendants may have added our restriction into the
		// tree, but they didn't all do so.
		this->pSetResult(iExpr->SelfOrClone(newOp0));
		}
	}

void Transform_PushDownRestricts::pHandleIt(const RelHead& iRH, const ZRef<Expr_Rel>& iExpr)
	{
	fRelHead |= iRH;
	ZRef<Expr_Rel> result = iExpr;
	foreachi (iter, fRestricts)
		{
		Restrict& theRestrict = **iter;
		const RelHead exprNames = sGetNames(theRestrict.fExpr_Bool);
		const RelHead intersection = exprNames & iRH;
		if (intersection.size())
			{
			++theRestrict.fCountTouching;
			if (intersection.size() == exprNames.size())
				{
				++theRestrict.fCountSubsuming;
				result = result & theRestrict.fExpr_Bool;
				}
			}
		}
	this->pSetResult(result);
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
