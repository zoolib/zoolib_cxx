// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Transform_PushDownRestricts.h"

#include "zoolib/Util_STL_set.h"
#include "zoolib/Util_STL_vector.h"
#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/ValPred/Util_Expr_Bool_ValPred_Rename.h"
#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_Do_GetNames.h"

#include "zoolib/Log.h"

namespace ZooLib {
namespace RelationalAlgebra {

using std::vector;
using namespace Util_STL;

// =================================================================================================
#pragma mark - Transform_PushDownRestricts

void Transform_PushDownRestricts::Visit_Expr_Rel_Calc(const ZP<Expr_Rel_Calc>& iExpr)
	{ this->pHandleIt(sRelHead(iExpr->GetColName()), iExpr->SelfOrClone(this->Do(iExpr->GetOp0()))); }

void Transform_PushDownRestricts::Visit_Expr_Rel_Concrete(const ZP<Expr_Rel_Concrete>& iExpr)
	{ this->pHandleIt(sRelHead(iExpr->GetConcreteHead()), iExpr); }

void Transform_PushDownRestricts::Visit_Expr_Rel_Const(const ZP<Expr_Rel_Const>& iExpr)
	{ this->pHandleIt(sRelHead(iExpr->GetColName()), iExpr); }

void Transform_PushDownRestricts::Visit_Expr_Rel_Embed(const ZP<Expr_Rel_Embed>& iExpr)
	{
	// I think this needs to work in a fashion akin to product, because we may have a restrict
	// in op1 that refers to a name from op0.

	ZP<Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	ZP<Expr_Rel> newOp1 = iExpr->GetOp1();

	this->pHandleIt(sRelHead(iExpr->GetColName()), iExpr->SelfOrClone(newOp0, newOp1));
	}

void Transform_PushDownRestricts::Visit_Expr_Rel_Product(const ZP<Expr_Rel_Product>& iExpr)
	{
	RelHead priorRelHead = fRelHead;

	ZP<Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());
	const RelHead leftRelHead = fRelHead;

	fRelHead = priorRelHead;

	ZP<Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());
	const RelHead rightRelHead = fRelHead;

	const RelHead combined = leftRelHead | rightRelHead;
	
	if (combined.size() != leftRelHead.size() + rightRelHead.size())
		{
		// The sets were not disjoint.
		ZAssert(false);
		}

	ZP<Expr_Rel> result = iExpr->SelfOrClone(newOp0, newOp1);

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

void Transform_PushDownRestricts::Visit_Expr_Rel_Rename(const ZP<Expr_Rel_Rename>& iExpr)
	{
	const ColName& oldName = iExpr->GetOld();
	const ColName& newName = iExpr->GetNew();

	Rename new2Old;
	new2Old[newName] = oldName;

	vector<ZP<Expr_Bool>> priorRestrictions;
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

	vector<ZP<Expr_Bool>>::const_iterator iterPriorRestrictions = priorRestrictions.begin();
	vector<RelHead>::const_iterator iterPriorNames = priorNames.begin();
	foreacha (entry, fRestricts)
		{
		entry->fExpr_Bool = *iterPriorRestrictions++;
		entry->fNames = *iterPriorNames++;
		}

	if (sQErase(fRelHead, oldName))
		fRelHead |= newName;
	}

void Transform_PushDownRestricts::Visit_Expr_Rel_Restrict(const ZP<Expr_Rel_Restrict>& iExpr)
	{
	Restrict theRestrict;
	theRestrict.fExpr_Bool = iExpr->GetExpr_Bool();
	theRestrict.fNames = sGetNames(theRestrict.fExpr_Bool);
	theRestrict.fCountTouching = 0;
	theRestrict.fCountSubsuming = 0;
	fRestricts.push_back(&theRestrict);

	ZP<Expr_Rel> oldOp0 = iExpr->GetOp0();
	ZP<Expr_Rel> newOp0 = this->Do(oldOp0);

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

void Transform_PushDownRestricts::Visit_Expr_Rel_Union(const ZP<Expr_Rel_Union>& iExpr)
	{
	const RelHead priorRelHead = fRelHead;

	ZP<Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());
	const RelHead leftRelHead = fRelHead;

	fRelHead = priorRelHead;

	ZP<Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());
	const RelHead rightRelHead = fRelHead;

	ZAssert(leftRelHead == rightRelHead);

	this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
	}

void Transform_PushDownRestricts::pHandleIt(const RelHead& iRH, const ZP<Expr_Rel>& iExpr)
	{
	fRelHead |= iRH;
	ZP<Expr_Rel> result = iExpr;
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
