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

#include "zoolib/ZExpr_Bool_ValPred.h"
#include "zoolib/ZSetRestore_T.h"
#include "zoolib/ZUniSet_T.h"
#include "zoolib/ZUtil_Expr_Bool_ValPred_Rename.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZValPred_Any.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_Do_GetNames.h"
#include "zoolib/ZVisitor_Expr_Op_Do_Transform_T.h"

#include "zoolib/zqe/ZQE_Expr_Rel_Search.h"
#include "zoolib/zqe/ZQE_Transform_Search.h"

#include "zoolib/zra/ZRA_Expr_Rel_Calc.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Const.h"
#include "zoolib/zra/ZRA_Expr_Rel_Dee.h"
#include "zoolib/zra/ZRA_Expr_Rel_Dum.h"
#include "zoolib/zra/ZRA_Expr_Rel_Embed.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"

namespace ZooLib {
namespace ZQE {

using ZRA::Expr_Rel;
using ZRA::RelName;
using ZRA::RelHead;
using ZRA::Rename;

using std::set;

// =================================================================================================
// MARK: - HasConst (anonymous)

namespace { // anonymous

bool spHasConst(const ZRef<ZValComparand>& iComparand)
	{
	if (iComparand.DynamicCast<ZValComparand_Const_Any>())
		return true;
	return false;
	}

bool spHasConst(const ZValPred& iValPred)
	{ return spHasConst(iValPred.GetLHS()) || spHasConst(iValPred.GetRHS()); }

class HasConst
:	public virtual ZVisitor_Do_T<bool>
,	public virtual ZVisitor_Expr_Bool_ValPred
,	public virtual ZVisitor_Expr_Op1_T<ZExpr_Bool>
,	public virtual ZVisitor_Expr_Op2_T<ZExpr_Bool>
	{
public:
// From ZVisitor_Expr_Bool_ValPred
	virtual void Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred>& iExpr)
		{ this->pSetResult(spHasConst(iExpr->GetValPred())); }

// From ZVisitor_Expr_Op1_T
	virtual void Visit_Expr_Op1(const ZRef<ZExpr_Op1_T<ZExpr_Bool> >& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0())); }

// From ZVisitor_Expr_Op2_T
	virtual void Visit_Expr_Op2(const ZRef<ZExpr_Op2_T<ZExpr_Bool> >& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) || this->Do(iExpr->GetOp1())); }
	};

} // anonymous namespace

// =================================================================================================
// MARK: - Transform_Search (anonymous)

namespace { // anonymous

/*
Transform_Search

Accumulate the rename, project and restriction on a branch, and when a concrete
is encountered instead return a search incorporating the accumulated info.
*/

class Transform_Search
:	public virtual ZVisitor_Expr_Op_Do_Transform_T<ZRA::Expr_Rel>
,	public virtual ZRA::Visitor_Expr_Rel_Calc
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
,	public virtual ZRA::Visitor_Expr_Rel_Const
,	public virtual ZRA::Visitor_Expr_Rel_Dee
,	public virtual ZRA::Visitor_Expr_Rel_Dum
,	public virtual ZRA::Visitor_Expr_Rel_Embed
,	public virtual ZRA::Visitor_Expr_Rel_Product
,	public virtual ZRA::Visitor_Expr_Rel_Project
,	public virtual ZRA::Visitor_Expr_Rel_Rename
,	public virtual ZRA::Visitor_Expr_Rel_Restrict
	{
	typedef ZVisitor_Expr_Op_Do_Transform_T<ZRA::Expr_Rel> inherited;
public:
	Transform_Search()
	:	fRestriction(sTrue())
	,	fProjection(ZUniSet_T<RelName>::sUniversal())
		{}

	virtual void Visit(const ZRef<ZVisitee>& iRep)
		{ ZUnimplemented(); }

	virtual void Visit_Expr_Rel_Calc(const ZRef<ZRA::Expr_Rel_Calc>& iExpr)
		{
		const RelName& theName = ZRA::sRenamed(fRename, iExpr->GetRelName());

		// The restriction may reference the name we introduce, so don't pass it down the tree.
		const ZRef<ZExpr_Bool> priorRestriction = fRestriction;
		fRestriction = sTrue();

		// Similarly with projection -- we don't know what names we'll be
		// referencing from our descendants.
		const ZUniSet_T<RelName> priorProjection = fProjection;
		fProjection = ZUniSet_T<RelName>::sUniversal();

		ZRef<ZRA::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());
		ZRef<ZRA::Expr_Rel> newCalc = new ZRA::Expr_Rel_Calc(newOp0, theName, iExpr->GetCallable());

		fRestriction = priorRestriction;
		fProjection = priorProjection;
		fRename.clear();
		this->pApplyRestrictProject(newCalc);
		}

	virtual void Visit_Expr_Rel_Concrete(const ZRef<ZRA::Expr_Rel_Concrete>& iExpr)
		{
		// fRestriction by now is written in terms used by the concrete itself,
		// and fRename maps from the concrete's terms to those used at the top of
		// the containing branch. We'll pass a rename that includes only those
		// resulting names that are in fProjection, so the passed rename is
		// effectively a project/rename descriptor.

		Rename newRename;
		for (Rename::iterator iterRename = fRename.begin();
			iterRename != fRename.end(); ++iterRename)
			{
			if (fProjection.Contains(iterRename->first))
				newRename.insert(*iterRename);
			}

		// Add missing entries
		const RelHead& theRH = iExpr->GetConcreteRelHead();
		for (RelHead::const_iterator iterRH = theRH.begin();
			iterRH != theRH.end(); ++iterRH)
			{
			const RelName& theRelName = *iterRH;
			if (fProjection.Contains(theRelName))
				ZUtil_STL::sInsertIfNotContains(newRename, theRelName, theRelName);
			}

		if (fRestriction == sTrue())
			{
			fLikelySize = 100;
			}
		else if (HasConst().Do(fRestriction))
			{
			fLikelySize = 1;
			}
		else
			{
			fLikelySize = 100;
			}

		ZRef<Expr_Rel_Search> theRel = new Expr_Rel_Search(newRename, fRestriction);
		this->pSetResult(theRel);
		}

	virtual void Visit_Expr_Rel_Const(const ZRef<ZRA::Expr_Rel_Const>& iExpr)
		{
		fLikelySize = 1;
		this->pApplyRestrictProject(iExpr->GetRelName(), iExpr);
		}

	virtual void Visit_Expr_Rel_Dee(const ZRef<ZRA::Expr_Rel_Dee>& iExpr)
		{
		fLikelySize = 1;
		this->pApplyRestrictProject(RelHead(), iExpr);
		}

	virtual void Visit_Expr_Rel_Dum(const ZRef<ZRA::Expr_Rel_Dum>& iExpr)
		{
		fLikelySize = 0;
		this->pApplyRestrictProject(RelHead(), iExpr);
		}

	virtual void Visit_Expr_Rel_Embed(const ZRef<ZRA::Expr_Rel_Embed>& iExpr)
		{
		ZRef<Expr_Rel> newOp1;

		{
		ZSetRestore_T<ZRef<ZExpr_Bool> > sr0(fRestriction, sTrue());
		ZSetRestore_T<ZUniSet_T<RelName> > sr1(fProjection, ZUniSet_T<RelName>::sUniversal());
		ZSetRestore_T<Rename> sr2(fRename);
		newOp1 = this->Do(iExpr->GetOp1());
		}

		const RelName& theName = ZRA::sRenamed(fRename, iExpr->GetRelName());

		ZRef<ZRA::Expr_Rel> newEmbed;
		{
		ZSetRestore_T<ZRef<ZExpr_Bool> > sr0(fRestriction, sTrue());
		ZSetRestore_T<ZUniSet_T<RelName> > sr1(fProjection, ZUniSet_T<RelName>::sUniversal());
		ZRef<ZRA::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());
		newEmbed = new ZRA::Expr_Rel_Embed(newOp0, theName, newOp1);
		}

		fRename.clear();
		this->pApplyRestrictProject(newEmbed);
		}

	virtual void Visit_Expr_Rel_Product(const ZRef<ZRA::Expr_Rel_Product>& iExpr)
		{
		// Remember curent state
		const ZRef<ZExpr_Bool> priorRestriction = fRestriction;
		const ZUniSet_T<RelName> priorProjection = fProjection;
		const Rename priorRename = fRename;

		// We leave rename in place to be used by children,
		// but reset the restriction and projection -- children will see only any
		// restriction/projection that exists on their own branch.
		fRestriction = sTrue();
		fProjection = ZUniSet_T<RelName>::sUniversal();

		// Process the left branch.
		ZRef<ZRA::Expr_Rel> op0 = this->Do(iExpr->GetOp0());
		const double leftLikelySize = fLikelySize.Get();
		fLikelySize.Clear();

		// Projection, rename and restriction may have been touched, so reset things
		// to the same state for the right branch as for the left.
		fRestriction = sTrue();
		fProjection = ZUniSet_T<RelName>::sUniversal();
		fRename = priorRename;

		// Process the right branch.
		ZRef<ZRA::Expr_Rel> op1 = this->Do(iExpr->GetOp1());
		const double rightLikelySize = fLikelySize.Get();

		fLikelySize = leftLikelySize * rightLikelySize;

		// Restore the restriction and projection.
		fRestriction = priorRestriction;
		fProjection = priorProjection;
		// But rename is now superfluous -- our children will have done whatever they need
		// with it. pApplyRestrictProject will apply any restriction/projection that remains.
		fRename.clear();

		if (rightLikelySize < leftLikelySize)
			this->pApplyRestrictProject(new ZRA::Expr_Rel_Product(op1, op0));
		else
			this->pApplyRestrictProject(new ZRA::Expr_Rel_Product(op0, op1));
		}

	virtual void Visit_Expr_Rel_Project(const ZRef<ZRA::Expr_Rel_Project>& iExpr)
		{
		ZSetRestore_T<ZUniSet_T<RelName> > sr(fProjection, iExpr->GetProjectRelHead());
		this->pSetResult(this->Do(iExpr->GetOp0()));
		}

	virtual void Visit_Expr_Rel_Rename(const ZRef<ZRA::Expr_Rel_Rename>& iExpr)
		{
		const RelName& oldName = iExpr->GetOld();
		RelName newName = iExpr->GetNew();

		Rename new2Old;
		new2Old[newName] = oldName;

		fRestriction = Util_Expr_Bool::sRenamed(new2Old, fRestriction);

		if (fProjection.QErase(newName))
			fProjection.Insert(oldName);

		if (ZQ<string8> theQ = ZUtil_STL::sQErase(fRename, newName))
			newName = *theQ;

		ZUtil_STL::sInsertMustNotContain(1, fRename, oldName, newName);

		this->pSetResult(this->Do(iExpr->GetOp0()));
		}

	virtual void Visit_Expr_Rel_Restrict(const ZRef<ZRA::Expr_Rel_Restrict>& iExpr)
		{
		ZRef<ZExpr_Bool> theRestriction = iExpr->GetExpr_Bool();
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

		for (Rename::iterator iterRename = fRename.begin();
			iterRename != fRename.end(); ++iterRename)
			{
			if (iterRename->first != iterRename->second)
				iRel = sRename(iRel, iterRename->first, iterRename->second);
			}

		this->pSetResult(iRel);
		}

	ZRef<ZExpr_Bool> fRestriction;
	ZUniSet_T<RelName> fProjection;
	Rename fRename;
	ZQ<double> fLikelySize;
	};

} // anonymous namespace

ZRef<ZRA::Expr_Rel> sTransform_Search(const ZRef<ZRA::Expr_Rel>& iExpr)
	{
	return Transform_Search().Do(iExpr);
	}

} // namespace ZQE
} // namespace ZooLib
