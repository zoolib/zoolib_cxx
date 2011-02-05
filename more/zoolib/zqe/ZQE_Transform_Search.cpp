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
#include "zoolib/ZVisitor_Expr_Op_Do_Transform_T.h"

#include "zoolib/zqe/ZQE_Expr_Rel_Search.h"
#include "zoolib/zqe/ZQE_Transform_Search.h"

#include "zoolib/zra/ZRA_Expr_Rel_Calc.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Const.h"
#include "zoolib/zra/ZRA_Expr_Rel_Dee.h"
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
#pragma mark -
#pragma mark * HasConst (anonymous)

namespace { // anonymous

bool spHasConst(const ZRef<ZValComparand>& iComparand)
	{
	if (ZRef<ZValComparand_Const_Any> asConst = iComparand.DynamicCast<ZValComparand_Const_Any>())
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
#pragma mark -
#pragma mark * Transform_Search (anonymous)

namespace { // anonymous

class Transform_Search
:	public virtual ZVisitor_Expr_Op_Do_Transform_T<ZRA::Expr_Rel>
,	public virtual ZRA::Visitor_Expr_Rel_Product
,	public virtual ZRA::Visitor_Expr_Rel_Embed
,	public virtual ZRA::Visitor_Expr_Rel_Project
,	public virtual ZRA::Visitor_Expr_Rel_Rename
,	public virtual ZRA::Visitor_Expr_Rel_Restrict
,	public virtual ZRA::Visitor_Expr_Rel_Calc
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
,	public virtual ZRA::Visitor_Expr_Rel_Const
,	public virtual ZRA::Visitor_Expr_Rel_Dee
	{
	typedef ZVisitor_Expr_Op_Do_Transform_T<ZRA::Expr_Rel> inherited;
public:
	Transform_Search()
	:	fExpr_Bool(sTrue())
	,	fProject(ZUniSet_T<RelName>::sUniversal())
		{}

	virtual void Visit(const ZRef<ZVisitee>& iRep)
		{ ZUnimplemented(); }

	virtual void Visit_Expr_Rel_Product(const ZRef<ZRA::Expr_Rel_Product>& iExpr)
		{
		// Remember curent state
		const ZRef<ZExpr_Bool> priorExpr_Bool = fExpr_Bool;
		const ZUniSet_T<RelName> priorProject = fProject;
		const Rename priorRename = fRename;

		// We leave rename in place to be used by children,
		// but reset the restriction and projection -- children will see only any
		// restriction/projection that exists on their own branch.
		fExpr_Bool = sTrue();
		fProject = ZUniSet_T<RelName>::sUniversal();
		
		// Process the left branch.
		ZRef<ZRA::Expr_Rel> op0 = this->Do(iExpr->GetOp0());
		double leftLikelySize = fLikelySize.Get();
		fLikelySize.Clear();

		// Projection, rename and restriction may have been touched, so reset things
		// to the same state for the right branch as for the left.
		fProject = ZUniSet_T<RelName>::sUniversal();
		fRename = priorRename;
		fExpr_Bool = sTrue();
		
		// Process the right branch.
		ZRef<ZRA::Expr_Rel> op1 = this->Do(iExpr->GetOp1());
		double rightLikelySize = fLikelySize.Get();
		
		bool canReorder = true;
		if (leftLikelySize < 0)
			{
			leftLikelySize = -leftLikelySize;
			canReorder = false;
			}

		if (rightLikelySize < 0)
			{
			rightLikelySize = -rightLikelySize;
			canReorder = false;
			}

		fLikelySize = leftLikelySize * rightLikelySize;
		
		// Restore the restriction.
		fExpr_Bool = priorExpr_Bool;
		fProject = priorProject;
		// But rename is now superfluous -- our children will have
		// done whatever they need with them. pHandleIt will simply
		// apply any restriction that remains.
		fRename.clear();

		if (canReorder && rightLikelySize < leftLikelySize)
			this->pHandleIt(new ZRA::Expr_Rel_Product(op1, op0));
		else
			this->pHandleIt(new ZRA::Expr_Rel_Product(op0, op1));
		}

	virtual void Visit_Expr_Rel_Embed(const ZRef<ZRA::Expr_Rel_Embed>& iExpr)
		{
		ZRef<Expr_Rel> newOp0;
		
		{
		ZSetRestore_T<ZRef<ZExpr_Bool> > sr0(fExpr_Bool, sTrue());
		ZSetRestore_T<ZUniSet_T<RelName> > sr1(fProject, ZUniSet_T<RelName>::sUniversal());
		ZSetRestore_T<Rename> sr2(fRename);
		newOp0 = this->Do(iExpr->GetOp0());
		}

		this->pHandleIt(iExpr->GetRelName(), iExpr->SelfOrClone(newOp0));
		}

	virtual void Visit_Expr_Rel_Project(const ZRef<ZRA::Expr_Rel_Project>& iExpr)
		{
		ZSetRestore_T<ZUniSet_T<RelName> > sr(fProject, iExpr->GetProjectRelHead());
		this->pSetResult(this->Do(iExpr->GetOp0()));
		}

	virtual void Visit_Expr_Rel_Rename(const ZRef<ZRA::Expr_Rel_Rename>& iExpr)
		{
		const RelName& oldName = iExpr->GetOld();
		RelName newName = iExpr->GetNew();

		Rename new2Old;
		new2Old[newName] = oldName;

		fExpr_Bool = Util_Expr_Bool::sRenamed(new2Old, fExpr_Bool);

		if (fProject.Contains(newName))
			{
			fProject.Erase(newName);
			fProject.Insert(oldName);
			}

		if (ZQ<string8> theQ = ZUtil_STL::sQErase(fRename, newName))
			newName = theQ.Get();

		ZUtil_STL::sInsertMustNotContain(1, fRename, oldName, newName);

		this->pSetResult(this->Do(iExpr->GetOp0()));		
		}

	virtual void Visit_Expr_Rel_Restrict(const ZRef<ZRA::Expr_Rel_Restrict>& iExpr)
		{
		ZRef<ZExpr_Bool> theExpr_Bool = iExpr->GetExpr_Bool();
		if (theExpr_Bool != sTrue())
			{
			if (fExpr_Bool != sTrue())
				fExpr_Bool &= theExpr_Bool;
			else
				fExpr_Bool = theExpr_Bool;
			}

		this->pSetResult(this->Do(iExpr->GetOp0()));
		}

	virtual void Visit_Expr_Rel_Calc(const ZRef<ZRA::Expr_Rel_Calc>& iExpr)
		{
		fLikelySize = -1;
		this->pHandleIt(iExpr->GetRelName(), iExpr);
		}

	virtual void Visit_Expr_Rel_Concrete(const ZRef<ZRA::Expr_Rel_Concrete>& iExpr)
		{
		// fExpr_Bool by now is written in terms used by the concrete itself,
		// and fRename maps from the concrete's terms to those used at the top of
		// the containg branch. We'll pass a rename that includes only those
		// resulting names that are in fProject, so the passed rename is
		// effectively a project/rename descriptor.

		Rename newRename;
		for (Rename::iterator iterRename = fRename.begin();
			iterRename != fRename.end(); ++iterRename)
			{
			if (fProject.Contains(iterRename->first))
				newRename.insert(*iterRename);
			}

		// Add missing entries
		const RelHead& theRH = iExpr->GetConcreteRelHead();
		for (RelHead::const_iterator iterRH = theRH.begin();
			iterRH != theRH.end(); ++iterRH)
			{
			const RelName& theRelName = *iterRH;
			if (fProject.Contains(theRelName))
				ZUtil_STL::sInsertIfNotContains(newRename, theRelName, theRelName);
			}

		if (fExpr_Bool == sTrue())
			{
			fLikelySize = 100;
			}
		else if (HasConst().Do(fExpr_Bool))
			{
			fLikelySize = 1;
			}
		else
			{
			fLikelySize = 100;
			}

		ZRef<Expr_Rel_Search> theRel = new Expr_Rel_Search(newRename, fExpr_Bool);
		this->pSetResult(theRel);
		}

	virtual void Visit_Expr_Rel_Const(const ZRef<ZRA::Expr_Rel_Const>& iExpr)
		{
		fLikelySize = 1;
		this->pHandleIt(iExpr->GetRelName(), iExpr);
		}

	virtual void Visit_Expr_Rel_Dee(const ZRef<ZRA::Expr_Rel_Dee>& iExpr)
		{
		fLikelySize = 1;
		this->pHandleIt(RelHead(), iExpr);
		}

	void pHandleIt(ZRef<Expr_Rel> iRel)
		{ this->pHandleIt(nullptr, iRel); }

	void pHandleIt(const RelHead& iRH, ZRef<Expr_Rel> iRel)
		{ this->pHandleIt(&iRH, iRel); }

	void pHandleIt(const RelHead* iRH, ZRef<Expr_Rel> iRel)
		{
		if (fExpr_Bool && fExpr_Bool != sTrue())
			iRel = iRel & fExpr_Bool;
			
		bool isUniversal;
		const RelHead projectElems = fProject.GetElems(isUniversal);

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

	ZRef<ZExpr_Bool> fExpr_Bool;
	ZUniSet_T<RelName> fProject;
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
