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
#include "zoolib/Util_STL_vector.h"
#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/ValPred/Util_Expr_Bool_ValPred_Rename.h"
#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_Do_GetNames.h"

#include "zoolib/RelationalAlgebra/Transform_PushDownRestricts.h"

#include "zoolib/Log.h"

namespace ZooLib {
namespace RelationalAlgebra {

using std::vector;
using namespace Util_STL;

// =================================================================================================
#pragma mark - Transform_PushDownRestricts

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
	RelHead priorRelHead = fRelHead;

	ZRef<Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());
	const RelHead leftRelHead = fRelHead;

	fRelHead = priorRelHead;

	ZRef<Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());
	const RelHead rightRelHead = fRelHead;

	const RelHead combined = leftRelHead | rightRelHead;
	
	if (combined.size() != leftRelHead.size() + rightRelHead.size())
		{
		// The sets were not disjoint.
		ZAssert(false);
		}

	ZRef<Expr_Rel> result = iExpr->SelfOrClone(newOp0, newOp1);

	// Examine restricts, see which were touched
	foreachv (Restrict* theRestrictPtr, fRestricts)
		{
		Restrict& theRestrict = *theRestrictPtr;
		if (theRestrict.fExpr_Bool)
			{
			if (theRestrict.fCountTouching != theRestrict.fCountSubsuming)
				{
				// Our children touched, but did not individually subsume this rel.
				const RelHead& exprNames = theRestrict.fNames;
				const RelHead intersection = exprNames & combined;

				// [Alternate version of this test:
				// [if (theRelHead.Contains(sGetNames(theRestrict.fExpr_Bool))
				if (intersection.size() && intersection.size() == exprNames.size())
					{
					// This product as a *whole* touches and subsumes the rel. Override whatever
					// values were in the counts with an arbitrary (but distinctive) matching pair,
					// so that parents can tell that we consumed this restrict. Also preserve
					// the restriction in the result.
					theRestrict.fCountTouching = 8888;
					theRestrict.fCountSubsuming = 8888;
					result = result & theRestrict.fExpr_Bool;
					}
				}
			}
		else
			{
			ZLOGTRACE(eDebug);
			}
		}
	this->pSetResult(result);

	fRelHead = combined;
	}

void Transform_PushDownRestricts::Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
	{
	const ColName& oldName = iExpr->GetOld();
	const ColName& newName = iExpr->GetNew();

	Rename new2Old;
	new2Old[newName] = oldName;

	vector<ZRef<Expr_Bool>> priorRestrictions;
	vector<RelHead> priorNames;

	foreacha (theRestrictPtr, fRestricts)
		{
		Restrict& theRestrict = *theRestrictPtr;

		priorRestrictions.push_back(theRestrict.fExpr_Bool);
		priorNames.push_back(theRestrict.fNames);

		const RelHead newNames = sRenamed(new2Old, theRestrict.fNames);
		if (newNames.size() != theRestrict.fNames.size())
			{
			// We've collapsed names, so the expression can no longer make sense.
			theRestrict.fExpr_Bool = null;
			}
		else
			{
			theRestrict.fExpr_Bool = Util_Expr_Bool::sRenamed(new2Old, theRestrict.fExpr_Bool);
			}
		theRestrict.fNames = newNames;
		}

	Visitor_Expr_Rel_Rename::Visit_Expr_Rel_Rename(iExpr);

	Rename old2New;
	old2New[oldName] = newName;

	vector<ZRef<Expr_Bool>>::const_iterator iterPriorRestrictions = priorRestrictions.begin();
	vector<RelHead>::const_iterator iterPriorNames = priorNames.begin();
	foreacha (entry, fRestricts)
		{
		entry->fExpr_Bool = *iterPriorRestrictions++;
		entry->fNames = *iterPriorNames++;
		}

	if (sQErase(fRelHead, oldName))
		fRelHead |= newName;
	}

void Transform_PushDownRestricts::Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr)
	{
	Restrict theRestrict;
	theRestrict.fExpr_Bool = iExpr->GetExpr_Bool();
	theRestrict.fNames = sGetNames(theRestrict.fExpr_Bool);
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

void Transform_PushDownRestricts::Visit_Expr_Rel_Union(const ZRef<Expr_Rel_Union>& iExpr)
	{
	const RelHead priorRelHead = fRelHead;

	ZRef<Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());
	const RelHead leftRelHead = fRelHead;

	fRelHead = priorRelHead;

	ZRef<Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());
	const RelHead rightRelHead = fRelHead;

	ZAssert(leftRelHead == rightRelHead);

	this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
	}

void Transform_PushDownRestricts::pHandleIt(const RelHead& iRH, const ZRef<Expr_Rel>& iExpr)
	{
	fRelHead |= iRH;
	ZRef<Expr_Rel> result = iExpr;
	foreacha (entryPtr, fRestricts)
		{
		Restrict& theRestrict = *entryPtr;
		const RelHead& exprNames = theRestrict.fNames;
		const RelHead intersection = exprNames & iRH;
		if (intersection.size())
			{
			// We overlap with the expression. That is, we provide a value the expression uses.
			++theRestrict.fCountTouching;
			if (theRestrict.fExpr_Bool)
				{
				// The expression is valid -- it didn't get made nonsensical by a name clash
				// between a rename and original data value. So we can use it.
				if (intersection.size() == exprNames.size())
					{
					++theRestrict.fCountSubsuming;
					result = result & theRestrict.fExpr_Bool;
					}
				}
			}
		}
	this->pSetResult(result);
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
