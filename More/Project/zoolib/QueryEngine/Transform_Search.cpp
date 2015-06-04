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
#include "zoolib/QueryEngine/Transform_Search.h"

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


//##
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"
#include "zoolib/ValPred/Util_Strim_Expr_Bool_ValPred.h"
//##

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
#pragma mark -
#pragma mark Visitor_Analyze (anonymous)

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

	bool fAnyIsConst;
	bool fAnyNameUnknown;
	};

Analysis_t operator|(const Analysis_t& l, const Analysis_t& r)
	{ return Analysis_t(l.fAnyIsConst || r.fAnyIsConst, l.fAnyNameUnknown || r.fAnyNameUnknown); }

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
	virtual void Visit_Expr_Bool_ValPred(const ZRef<Expr_Bool_ValPred>& iExpr)
		{ this->pSetResult(sAnalyze(iExpr->GetValPred())); }

// From Visitor_Expr_Op1_T
	virtual void Visit_Expr_Op1(const ZRef<Expr_Op1_T<Expr_Bool> >& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0())); }

// From Visitor_Expr_Op2_T
	virtual void Visit_Expr_Op2(const ZRef<Expr_Op2_T<Expr_Bool> >& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) | this->Do(iExpr->GetOp1())); }


private:
	Analysis_t sAnalyze(const ZRef<ValComparand>& iComparand)
		{
		Analysis_t result;
		if (iComparand.DynamicCast<ValComparand_Const_Any>())
			result.fAnyIsConst = true;

		if (ZRef<ValComparand_Name> theComparand_Name = iComparand.DynamicCast<ValComparand_Name>())
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
#pragma mark -
#pragma mark Transform_Search (anonymous)

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
//,	public virtual RA::Visitor_Expr_Rel_Search //??
	{
	typedef Visitor_Expr_Op_Do_Transform_T<RA::Expr_Rel> inherited;
public:
	Transform_Search()
	:	fRestriction(sTrue())
	,	fProjection(UniSet<ColName>::sUniversal())
		{}

	virtual void Visit(const ZRef<Visitee>& iRep)
		{ ZUnimplemented(); }

	virtual void Visit_Expr_Rel_Calc(const ZRef<RA::Expr_Rel_Calc>& iExpr)
		{
		const ColName& theName = RA::sRenamed(fRename, iExpr->GetColName());

		// The restriction may reference the name we introduce, so don't pass it down the tree.
		const ZRef<Expr_Bool> priorRestriction = fRestriction;
		fRestriction = sTrue();

		// Similarly with projection -- we don't know what names we'll be
		// referencing from our descendants.
		const UniSet<ColName> priorProjection = fProjection;
		fProjection = UniSet<ColName>::sUniversal();

		ZRef<RA::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());
		ZRef<RA::Expr_Rel> newCalc = new RA::Expr_Rel_Calc(newOp0, theName, iExpr->GetCallable());

		fRestriction = priorRestriction;
		fProjection = priorProjection;
		fRename.clear();
		this->pApplyRestrictProject(newCalc);
		}

	virtual void Visit_Expr_Rel_Concrete(const ZRef<RA::Expr_Rel_Concrete>& iExpr)
		{
		// fRestriction by now is written in terms used by the concrete itself,
		// and fRename maps from the concrete's terms to those used at the top of
		// the containing branch.

		Rename newRename;
		foreachi (iterRename, fRename)
			{
			if (fProjection.Contains(iterRename->first))
				newRename.insert(*iterRename);
			}

		// Add missing entries
		RelHead theRH_All;
		RelHead theRH_Optional;
		foreachi (iter, iExpr->GetConcreteHead())
			{
			const ColName& theColName = iter->first;
			if (fProjection.Contains(theColName))
				sQInsert(newRename, theColName, theColName);

			sQInsert(theRH_All, theColName);

			if (not iter->second)
				sQInsert(theRH_Optional, theColName);
			}

		fLikelySize = 1;

//		if (fRestriction == sTrue())
//			{
//			fLikelySize = 100;
//			}
//		else if (HasConst().Do(fRestriction))
//			{
//			fLikelySize = 1;
//			}
//		else
//			{
//			fLikelySize = 100;
//			}

		// However, fRestriction may well also reference names *not* in the concrete, if we're
		// part of the embeddee of an embed. The simplest solution for now is to pull up any
		// terms referencing names *not* in the concrete. So, get fRestriction into CNF. Separate it
		// into stuff that references names in the concrete, which we'll stuff into a search. Any
		// referencing names not in the concrete are stuffed into a restrict, wrapped around the search.

		ZRef<Expr_Bool> conjunctionRestrict, conjunctionSearch;

		Visitor_Analyze theVisitor(theRH_All);

		foreachi (clause, Util_Expr_Bool::sAsCNF(fRestriction))
			{
			bool referencesInnerOnly = true;

			ZRef<Expr_Bool> newClause;

			foreachv (const ZRef<Expr_Bool> disjunction, *clause)
				{
				newClause |= disjunction;
				const Analysis_t theAn = theVisitor.Do(disjunction);
				if (theAn.fAnyNameUnknown)
					referencesInnerOnly = false;
				}

			if (referencesInnerOnly)
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

		ZRef<Expr_Rel> theRel = new Expr_Rel_Search(newRename, theRH_Optional, conjunctionSearch);
		if (conjunctionRestrict)
			theRel &= conjunctionRestrict;

		this->pSetResult(theRel);
		}

	virtual void Visit_Expr_Rel_Const(const ZRef<RA::Expr_Rel_Const>& iExpr)
		{
		fLikelySize = 1;
		this->pApplyRestrictProject(sRelHead(iExpr->GetColName()), iExpr);
		}

	virtual void Visit_Expr_Rel_Dee(const ZRef<RA::Expr_Rel_Dee>& iExpr)
		{
		fLikelySize = 1;
		this->pApplyRestrictProject(RelHead(), iExpr);
		}

	virtual void Visit_Expr_Rel_Dum(const ZRef<RA::Expr_Rel_Dum>& iExpr)
		{
		fLikelySize = 0;
		this->pApplyRestrictProject(RelHead(), iExpr);
		}

	virtual void Visit_Expr_Rel_Embed(const ZRef<RA::Expr_Rel_Embed>& iExpr)
		{
		ZRef<Expr_Rel> newOp0, newOp1;

		{
		SaveSetRestore<ZRef<Expr_Bool> > ssr0(fRestriction, sTrue());
		SaveSetRestore<UniSet<ColName> > ssr1(fProjection, UniSet<ColName>::sUniversal());
		newOp0 = this->Do(iExpr->GetOp0());
		}

		{
		SaveSetRestore<ZRef<Expr_Bool> > ssr0(fRestriction, sTrue());
		SaveSetRestore<UniSet<ColName> > ssr1(fProjection, UniSet<ColName>::sUniversal());
		SaveSetRestore<Rename> ssr2(fRename, Rename());
		newOp1 = this->Do(iExpr->GetOp1());
		}

		const ColName& theName = RA::sRenamed(fRename, iExpr->GetColName());
		ZRef<RA::Expr_Rel> newEmbed = new RA::Expr_Rel_Embed(newOp0, theName, newOp1);

		// But rename is now superfluous -- our children will have done whatever they need
		// with it. pApplyRestrictProject will apply any restriction/projection that remains.
		fRename.clear();

		this->pApplyRestrictProject(newEmbed);
		}

	virtual void Visit_Expr_Rel_Product(const ZRef<RA::Expr_Rel_Product>& iExpr)
		{
		// Remember curent state
		const ZRef<Expr_Bool> priorRestriction = fRestriction;
		const UniSet<ColName> priorProjection = fProjection;
		const Rename priorRename = fRename;

		// We leave rename in place to be used by children,
		// but reset the restriction and projection -- children will see only any
		// restriction/projection that exists on their own branch.
		fRestriction = sTrue();
		fProjection = UniSet<ColName>::sUniversal();

		// Process the left branch.
		ZRef<RA::Expr_Rel> op0 = this->Do(iExpr->GetOp0());
		const double leftLikelySize = fLikelySize.Get();
		fLikelySize.Clear();

		// Projection, rename and restriction may have been touched, so reset things
		// to the same state for the right branch as for the left.
		fRestriction = sTrue();
		fProjection = UniSet<ColName>::sUniversal();
		fRename = priorRename;

		// Process the right branch.
		ZRef<RA::Expr_Rel> op1 = this->Do(iExpr->GetOp1());
		const double rightLikelySize = fLikelySize.Get();

		fLikelySize = leftLikelySize * rightLikelySize;

		// Restore the restriction and projection.
		fRestriction = priorRestriction;
		fProjection = priorProjection;
		// But rename is now superfluous -- our children will have done whatever they need
		// with it. pApplyRestrictProject will apply any restriction/projection that remains.
		fRename.clear();

		if (rightLikelySize < leftLikelySize)
			this->pApplyRestrictProject(new RA::Expr_Rel_Product(op1, op0));
		else
			this->pApplyRestrictProject(new RA::Expr_Rel_Product(op0, op1));
		}

	virtual void Visit_Expr_Rel_Project(const ZRef<RA::Expr_Rel_Project>& iExpr)
		{
		SaveSetRestore<UniSet<ColName> > ssr(fProjection, iExpr->GetProjectRelHead());
		this->pSetResult(this->Do(iExpr->GetOp0()));
		}

	virtual void Visit_Expr_Rel_Rename(const ZRef<RA::Expr_Rel_Rename>& iExpr)
		{
		const ColName& oldName = iExpr->GetOld();
		ColName newName = iExpr->GetNew();

		Rename new2Old;
		new2Old[newName] = oldName;

		fRestriction = Util_Expr_Bool::sRenamed(new2Old, fRestriction);

		if (fProjection.QErase(newName))
			fProjection.Insert(oldName);

		if (ZQ<string8> theQ = sQGetErase(fRename, newName))
			newName = *theQ;

		sInsertMust(fRename, oldName, newName);

		this->pSetResult(this->Do(iExpr->GetOp0()));
		}

	virtual void Visit_Expr_Rel_Restrict(const ZRef<RA::Expr_Rel_Restrict>& iExpr)
		{
		ZRef<Expr_Bool> theRestriction = iExpr->GetExpr_Bool();
		if (theRestriction != sTrue())
			{
			if (fRestriction != sTrue())
				fRestriction &= theRestriction;
			else
				fRestriction = theRestriction;
			}

		this->pSetResult(this->Do(iExpr->GetOp0()));
		}

	void pApplyRestrictProject(const ZRef<Expr_Rel>& iRel)
		{ this->pApplyRestrictProject(nullptr, iRel); }

	void pApplyRestrictProject(const RelHead& iRH, const ZRef<Expr_Rel>& iRel)
		{ this->pApplyRestrictProject(&iRH, iRel); }

	void pApplyRestrictProject(const RelHead* iRH, ZRef<Expr_Rel> iRel)
		{
		// Apply any restriction that remains.
		if (fRestriction != sTrue())
			{
			iRel = iRel & fRestriction;
			fRestriction = sTrue();
			}

		bool isUniversal;
		const RelHead& projectElems = fProjection.GetElems(isUniversal);

		if (isUniversal)
			{
			// No actual project has been put in place -- we're just
			// recording the effect of some rename.
			}
		else if (iRH)
			{
			const RelHead theRH = projectElems & *iRH;
			if (theRH.size() != iRH->size())
				iRel = sProject(iRel, theRH);
			}
		else
			{
			iRel = sProject(iRel, projectElems);
			}

		foreachi (iterRename, fRename)
			{
			if (iterRename->first != iterRename->second)
				iRel = sRename(iRel, iterRename->first, iterRename->second);
			}

		this->pSetResult(iRel);
		}

	ZRef<Expr_Bool> fRestriction;
	UniSet<ColName> fProjection;
	Rename fRename;
	ZQ<double> fLikelySize;
	};

} // anonymous namespace

ZRef<RA::Expr_Rel> sTransform_Search(const ZRef<RA::Expr_Rel>& iExpr)
	{
	if (ZRef<RA::Expr_Rel> result = Transform_Search().Do(iExpr))
		{
		return result;
		}
	return iExpr;
	}

} // namespace QueryEngine
} // namespace ZooLib
