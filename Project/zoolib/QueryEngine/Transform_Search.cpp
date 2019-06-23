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

#include "zoolib/QueryEngine/Transform_Search.h"

#include "zoolib/Log.h"
#include "zoolib/UniSet.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h"
#include "zoolib/ValPred/Util_Expr_Bool_ValPred_Rename.h"
#include "zoolib/ValPred/ValPred_Any.h"
#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_Do_GetNames.h"

#include "zoolib/Expr/Expr_Bool.h"
#include "zoolib/Expr/Util_Expr_Bool_CNF.h"
#include "zoolib/Expr/Visitor_Expr_Op_Do_Transform_T.h"

#include "zoolib/QueryEngine/Expr_Rel_Search.h"

#include "zoolib/RelationalAlgebra/Expr_Rel_Calc.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Const.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Dee.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Dum.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Embed.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Product.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Project.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Restrict.h"

#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"

namespace ZooLib {
namespace QueryEngine {

namespace RA = RelationalAlgebra;

using RA::Expr_Rel;
using RA::ColName;
using RA::RelHead;
using RA::sRelHead;
using RA::Rename;

using std::set;

using namespace Util_STL;

// =================================================================================================
#pragma mark - Visitor_Analyze (anonymous)

namespace { // anonymous

struct Analysis_t
	{
	Analysis_t()
	:	fAnyIsConst(false)
	,	fAnyNameUnknown(false)
		{}

	Analysis_t(bool iAnyIsConst, bool iAnyNameUnknown)
	:	fAnyIsConst(iAnyIsConst)
	,	fAnyNameUnknown(iAnyNameUnknown)
		{}

	Analysis_t operator|(const Analysis_t& r)
		{
		return Analysis_t(
			this->fAnyIsConst || r.fAnyIsConst,
			this->fAnyNameUnknown || r.fAnyNameUnknown);
		}

	bool fAnyIsConst;
	bool fAnyNameUnknown;
	};

class Visitor_Analyze
:	public virtual Visitor_Do_T<Analysis_t>
,	public virtual Visitor_Expr_Bool_ValPred
,	public virtual Visitor_Expr_Op1_T<Expr_Bool>
,	public virtual Visitor_Expr_Op2_T<Expr_Bool>
	{
public:
	Visitor_Analyze(const RelHead& iNames)
	:	fNames(iNames)
		{}
// From Visitor_Expr_Bool_ValPred
	virtual void Visit_Expr_Bool_ValPred(const ZP<Expr_Bool_ValPred>& iExpr)
		{ this->pSetResult(sAnalyze(iExpr->GetValPred())); }

// From Visitor_Expr_Op1_T
	virtual void Visit_Expr_Op1(const ZP<Expr_Op1_T<Expr_Bool> >& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0())); }

// From Visitor_Expr_Op2_T
	virtual void Visit_Expr_Op2(const ZP<Expr_Op2_T<Expr_Bool> >& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) | this->Do(iExpr->GetOp1())); }

private:
	Analysis_t sAnalyze(const ZP<ValComparand>& iComparand)
		{
		Analysis_t result;
		if (iComparand.DynamicCast<ValComparand_Const_Any>())
			result.fAnyIsConst = true;

		if (ZP<ValComparand_Name> theComparand_Name = iComparand.DynamicCast<ValComparand_Name>())
			{
			if (not sContains(fNames, theComparand_Name->GetName()))
				result.fAnyNameUnknown = true;
			}

		return result;
		}

	Analysis_t sAnalyze(const ValPred& iValPred)
		{ return sAnalyze(iValPred.GetLHS()) | sAnalyze(iValPred.GetRHS()); }

	const RelHead fNames;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - Transform_Search (anonymous)

namespace { // anonymous

/*
Transform_Search

Accumulate the rename, project and restriction on a branch, and when a concrete
is encountered instead return a search incorporating the accumulated info.
*/

class Transform_Search
:	public virtual Visitor_Expr_Op_Do_Transform_T<RA::Expr_Rel>
,	public virtual RA::Visitor_Expr_Rel_Calc
,	public virtual RA::Visitor_Expr_Rel_Concrete
,	public virtual RA::Visitor_Expr_Rel_Const
,	public virtual RA::Visitor_Expr_Rel_Dee
,	public virtual RA::Visitor_Expr_Rel_Dum
,	public virtual RA::Visitor_Expr_Rel_Embed
,	public virtual RA::Visitor_Expr_Rel_Product
,	public virtual RA::Visitor_Expr_Rel_Project
,	public virtual RA::Visitor_Expr_Rel_Rename
,	public virtual RA::Visitor_Expr_Rel_Restrict
,	public virtual RA::Visitor_Expr_Rel_Union
//,	public virtual RA::Visitor_Expr_Rel_Search //??
	{
	typedef Visitor_Expr_Op_Do_Transform_T<RA::Expr_Rel> inherited;
public:
	Transform_Search()
	:	fRestriction(sTrue())
	,	fProjection(UniSet<ColName>::sUniversal())
		{}

	virtual void Visit(const ZP<Visitee>& iRep)
		{ ZUnimplemented(); }

	virtual void Visit_Expr_Rel_Calc(const ZP<RA::Expr_Rel_Calc>& iExpr)
		{
		const ColName& theName = RA::sRenamed(fRename_LeafToRoot, iExpr->GetColName());

		// The restriction may reference the name we introduce, so don't pass it down the tree.
		const ZP<Expr_Bool> priorRestriction = fRestriction;
		fRestriction = sTrue();

		// Similarly with projection -- we don't know what names we'll be
		// referencing from our descendants.
		const UniSet<ColName> priorProjection = fProjection;
		fProjection = UniSet<ColName>::sUniversal();

		ZP<RA::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());
		ZP<RA::Expr_Rel> newCalc = sCalc(newOp0, theName, iExpr->GetCallable());

		this->pSetResultWithRestrictProjectRename(
			newCalc, priorRestriction, priorProjection, Rename(), null);
		}

	virtual void Visit_Expr_Rel_Concrete(const ZP<RA::Expr_Rel_Concrete>& iExpr)
		{
		// fRestriction by now is written in terms used by the concrete itself,
		// and fRename_LeafToRoot maps from the concrete's terms to those used at the top of
		// the containing branch.

		Rename newRename;
		foreacha (entry, fRename_LeafToRoot)
			{
			if (fProjection.Contains(entry.first))
				newRename.insert(entry);
			}

		// Build list of all the names a condition can rely on.
		RelHead theRH_InnerAndBound = fBoundNames;
		RelHead theRH_Optional;
		foreacha (entry, iExpr->GetConcreteHead())
			{
			const ColName& theColName = entry.first;
			if (fProjection.Contains(theColName))
				sQInsert(newRename, theColName, theColName);

			// TODO: What do we do about overlaps between bound names, and those referenced
			// inside? It's a semantic error at a higher level, so we shouldn't see it here,
			// and it doesn't actually affect what's happening here, but it *will* affect the
			// processing of conjunctionSearch. I suppose the inner actually shadows/hides the
			// bound name, which is reasonable, which Walkers will need to handle when they're
			// setting up offsets.
			sQInsert(theRH_InnerAndBound, theColName);

			if (not entry.second)
				sQInsert(theRH_Optional, theColName);
			}

		// TODO: Manipulate fLikelySizeQ in some fashion.

		// However, fRestriction may well also reference names *not* in the concrete, if we're
		// part of the embeddee of an embed. The simplest solution for now is to pull up any
		// terms referencing names *not* in the concrete. So, get fRestriction into CNF. Separate
		// it into stuff that references names in the concrete, which we'll stuff into a search.
		// Any referencing names not in the concrete are stuffed into a restrict, wrapped around
		// the search.

		Visitor_Analyze theVisitor(theRH_InnerAndBound);

		ZP<Expr_Bool> conjunctionRestrict, conjunctionSearch;

		foreacha (clause, Util_Expr_Bool::sAsCNF(fRestriction))
			{
			bool referencesInnerAndBoundOnly = true;

			ZP<Expr_Bool> newClause;

			foreachv (const ZP<Expr_Bool>& disjunction, clause)
				{
				newClause |= disjunction;
				const Analysis_t theAn = theVisitor.Do(disjunction);
				if (theAn.fAnyNameUnknown)
					referencesInnerAndBoundOnly = false;
				}

			if (referencesInnerAndBoundOnly)
				{
				if (conjunctionSearch)
					conjunctionSearch &= newClause;
				else
					conjunctionSearch = newClause;
				}
			else
				{
				if (conjunctionRestrict)
					conjunctionRestrict &= newClause;
				else
					conjunctionRestrict = newClause;
				}
			}

		ZP<Expr_Rel> theRel = new Expr_Rel_Search(fBoundNames,
			newRename, theRH_Optional, conjunctionSearch);

		if (conjunctionRestrict)
			theRel &= conjunctionRestrict;

		this->pSetResult(theRel);
		}

	virtual void Visit_Expr_Rel_Const(const ZP<RA::Expr_Rel_Const>& iExpr)
		{
		fLikelySizeQ = 1;
		this->pSetResultWithRestrictProjectRename(iExpr, iExpr->GetColName());
		}

	virtual void Visit_Expr_Rel_Dee(const ZP<RA::Expr_Rel_Dee>& iExpr)
		{
		fLikelySizeQ = 1;
		this->pSetResultWithRestrictProjectRename(iExpr, null);
		}

	virtual void Visit_Expr_Rel_Dum(const ZP<RA::Expr_Rel_Dum>& iExpr)
		{
		fLikelySizeQ = 0;
		this->pSetResultWithRestrictProjectRename(iExpr, null);
		}

	virtual void Visit_Expr_Rel_Embed(const ZP<RA::Expr_Rel_Embed>& iExpr)
		{
		const ZP<Expr_Bool> priorRestriction = fRestriction;
		const UniSet<ColName> priorProjection = fProjection;
		const Rename priorRename_LeafToRoot = fRename_LeafToRoot;

		fRestriction = sTrue();
		fProjection = UniSet<ColName>::sUniversal();
		ZP<Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

		fRestriction = sTrue();
		fProjection = UniSet<ColName>::sUniversal();
		fRename_LeafToRoot.clear();
		fBoundNames = iExpr->GetBoundNames();
		ZP<Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());

		// Restore this stuff -- we don't want anything from the embedee itself to leak up to here.
		fRestriction = priorRestriction;
		fProjection = priorProjection;
		fRename_LeafToRoot = priorRename_LeafToRoot;

		const RelHead& theBoundNames = RA::sRenamed(priorRename_LeafToRoot, iExpr->GetBoundNames());
		const ColName& theName = RA::sRenamed(priorRename_LeafToRoot, iExpr->GetColName());

		ZP<RA::Expr_Rel> newEmbed = sEmbed(newOp0, theBoundNames, theName, newOp1);

		this->pSetResultWithRestrictProjectRename(newEmbed, null);
		}

	virtual void Visit_Expr_Rel_Product(const ZP<RA::Expr_Rel_Product>& iExpr)
		{
		// Remember current state, to use and to restore
		const ZP<Expr_Bool> priorRestriction = fRestriction;
		const UniSet<ColName> priorProjection = fProjection;
		const Rename priorRename_LeafToRoot = fRename_LeafToRoot;

		// We leave rename in place to be used by children,
		// but reset the restriction and projection -- children will see only any
		// restriction/projection that exists on their own branch.
		fRestriction = sTrue();
		fProjection = UniSet<ColName>::sUniversal();

		// Process the left branch.
		ZP<RA::Expr_Rel> op0 = this->Do(iExpr->GetOp0());
		const double leftLikelySize = fLikelySizeQ.Get();
		fLikelySizeQ.Clear();
		const RelHead namesOnLeft = RA::sNamesTo(fRename_LeafToRoot);

		// Projection, rename and restriction may have been touched, so reset things
		// to the same state for the right branch as for the left.
		fRestriction = sTrue();
		fProjection = UniSet<ColName>::sUniversal();
		fRename_LeafToRoot = priorRename_LeafToRoot;

		// Can we treat the left branch names as bound for the right?

		// Process the right branch.
		fBoundNames = namesOnLeft;
		ZP<RA::Expr_Rel> op1 = this->Do(iExpr->GetOp1());
		const double rightLikelySize = fLikelySizeQ.Get();

		fLikelySizeQ = leftLikelySize * rightLikelySize;

//		if (rightLikelySize < leftLikelySize)
//			{
//			this->pSetResultWithRestrictProjectRename(sProduct(op1, op0));
//			}
//		else
			{
//			this->pSetResult(sProduct(op0, op1));
			this->pSetResultWithRestrictProjectRename(
				sProduct(op0, op1), priorRestriction, priorProjection, Rename(), null);
			}
		}

	virtual void Visit_Expr_Rel_Project(const ZP<RA::Expr_Rel_Project>& iExpr)
		{
		SaveSetRestore<UniSet<ColName> > ssr(fProjection, iExpr->GetProjectRelHead());
		this->pSetResult(this->Do(iExpr->GetOp0()));
		}

	virtual void Visit_Expr_Rel_Rename(const ZP<RA::Expr_Rel_Rename>& iExpr)
		{
		const ColName& oldName = iExpr->GetOld();
		ColName newName = iExpr->GetNew();

		Rename new2Old;
		new2Old[newName] = oldName;

		fRestriction = Util_Expr_Bool::sRenamed(new2Old, fRestriction);

		if (fProjection.QErase(newName))
			fProjection.Insert(oldName);

		if (ZQ<string8> theQ = sQGetErase(fRename_LeafToRoot, newName))
			newName = *theQ;

		sInsertMust(fRename_LeafToRoot, oldName, newName);

		ZP<Expr_Rel> result = this->Do(iExpr->GetOp0());

		this->pSetResult(result);
		}

	virtual void Visit_Expr_Rel_Restrict(const ZP<RA::Expr_Rel_Restrict>& iExpr)
		{
		ZP<Expr_Bool> theRestriction = iExpr->GetExpr_Bool();
		if (theRestriction != sTrue())
			{
			if (fRestriction != sTrue())
				fRestriction &= theRestriction;
			else
				fRestriction = theRestriction;
			}

		this->pSetResult(this->Do(iExpr->GetOp0()));
		}

	virtual void Visit_Expr_Rel_Union(const ZP<RA::Expr_Rel_Union>& iExpr)
		{
		const ZP<Expr_Bool> priorRestriction = fRestriction;
		const UniSet<ColName> priorProjection = fProjection;
		const Rename priorRename_LeafToRoot = fRename_LeafToRoot;

		// Process the left branch.
		ZP<RA::Expr_Rel> op0 = this->Do(iExpr->GetOp0());

		// Restore state and then process the right
		fRestriction = priorRestriction;
		fProjection = priorProjection;
		fRename_LeafToRoot = priorRename_LeafToRoot;

		ZP<RA::Expr_Rel> op1 = this->Do(iExpr->GetOp1());

//		this->pSetResultWithRestrictProjectRename(
//			sUnion(op0, op1), priorRestriction, priorProjection, null);
		this->pSetResult(sUnion(op0, op1));
		}

	void pSetResultWithRestrictProjectRename(const ZP<Expr_Rel>& iRel, const ZQ<ColName>& iNameQ)
		{
		ZP<Expr_Rel> theResult = spGetResultWithRestrictProjectRename(
			iRel, fRestriction, fProjection, fRename_LeafToRoot, iNameQ);
		this->pSetResult(theResult);
		}

	void pSetResultWithRestrictProjectRename(const ZP<Expr_Rel>& iRel,
		const ZP<Expr_Bool>& iRestriction,
		const UniSet<ColName>& iProjection,
		const Rename& iRename_LeafToRoot,
		const ZQ<ColName>& iNameQ)
		{
		ZP<Expr_Rel> theResult = spGetResultWithRestrictProjectRename(
			iRel, iRestriction, iProjection, iRename_LeafToRoot, iNameQ);
		this->pSetResult(theResult);
		}

	static ZP<Expr_Rel> spGetResultWithRestrictProjectRename(
		const ZP<Expr_Rel>& iRel,
		const ZP<Expr_Bool>& iRestriction,
		const UniSet<ColName>& iProjection,
		const Rename& iRename_LeafToRoot,
		const ZQ<ColName>& iNameQ)
		{
		ZP<Expr_Rel> theRel = iRel;

		// Apply any restriction that remains.
		if (iRestriction != sTrue())
			theRel = theRel & iRestriction;

		bool isUniversal;
		const RelHead& projectElems = iProjection.GetElems(isUniversal);

		if (isUniversal)
			{
			// No actual project has been put in place -- we're just
			// recording the effect of some rename.
			}
		else if (iNameQ)
			{
			if (not sContains(projectElems, *iNameQ))
				ZLOGTRACE(eWarning);
// ##			theRel = sProject(theRel, sRelHead(*iNameQ));
			}
		else
			{
			theRel = sProject(theRel, projectElems);
			}

		if (iNameQ)
			{
			if (ZQ<ColName> theRootNameQ = sQGet(iRename_LeafToRoot, *iNameQ))
				theRel = sRename(theRel, *theRootNameQ, *iNameQ);

			}
		else
			{
			foreacha (entry, iRename_LeafToRoot)
				{
				if (entry.first != entry.second)
					theRel = sRename(theRel, entry.second, entry.first);
				}
			}

		return theRel;
		}

	RelHead fBoundNames;
	ZP<Expr_Bool> fRestriction;
	UniSet<ColName> fProjection;
	Rename fRename_LeafToRoot;
	ZQ<double> fLikelySizeQ;
	};

} // anonymous namespace

ZP<RA::Expr_Rel> sTransform_Search(const ZP<RA::Expr_Rel>& iExpr)
	{
	if (ZP<RA::Expr_Rel> result = Transform_Search().Do(iExpr))
		return result;

	return iExpr;
	}

} // namespace QueryEngine
} // namespace ZooLib
