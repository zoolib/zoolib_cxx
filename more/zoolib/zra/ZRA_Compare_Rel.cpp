/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZVisitor_Do_T.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_Any_DoCompare.h"
#include "zoolib/zra/ZRA_Compare_Rel.h"
#include "zoolib/zra/ZRA_Expr_Rel_Calc.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Const.h"
#include "zoolib/zra/ZRA_Expr_Rel_Difference.h"
#include "zoolib/zra/ZRA_Expr_Rel_Embed.h"
#include "zoolib/zra/ZRA_Expr_Rel_Intersect.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Union.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict_Any.h"
#include "zoolib/zra/ZRA_Expr_Rel_Select.h"

namespace ZooLib {

using namespace ZRA;

// =================================================================================================
#pragma mark -
#pragma mark * Comparer and default derivations (anonymous)

namespace { // anonymous

class Comparer
:	public virtual ZVisitor_Do_T<int>
,	public virtual Visitor_Expr_Rel_Calc
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Const
,	public virtual Visitor_Expr_Rel_Difference
,	public virtual Visitor_Expr_Rel_Embed
,	public virtual Visitor_Expr_Rel_Intersect
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict_Any
,	public virtual Visitor_Expr_Rel_Select
,	public virtual Visitor_Expr_Rel_Union
	{
public:
	virtual void Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>&) { pSetResult(-1); } 
	virtual void Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>&) { pSetResult(-1); }
	virtual void Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>&) { pSetResult(-1); }
	virtual void Visit_Expr_Rel_Difference(const ZRef<Expr_Rel_Difference>&) { pSetResult(-1); }
	virtual void Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>&) { pSetResult(-1); }
	virtual void Visit_Expr_Rel_Intersect(const ZRef<Expr_Rel_Intersect>& iExpr) { pSetResult(-1); }
	virtual void Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>&) { pSetResult(-1); }
	virtual void Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>&) { pSetResult(-1); }
	virtual void Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>&) { pSetResult(-1); }
	virtual void Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict_Any>&) { pSetResult(-1); }
	virtual void Visit_Expr_Rel_Select(const ZRef<Expr_Rel_Select>&) { pSetResult(-1); }
	virtual void Visit_Expr_Rel_Union(const ZRef<Expr_Rel_Union>&) { pSetResult(-1); }

	int CompareUnary(ZRef<ZExpr_Op1_T<Expr_Rel> > iLHS, ZRef<ZExpr_Op1_T<Expr_Rel> > iRHS);
	int CompareBinary(ZRef<ZExpr_Op2_T<Expr_Rel> > iLHS, ZRef<ZExpr_Op2_T<Expr_Rel> > iRHS);
	};

int Comparer::CompareUnary(ZRef<ZExpr_Op1_T<Expr_Rel> > iLHS, ZRef<ZExpr_Op1_T<Expr_Rel> > iRHS)
	{
	return sCompare_T(iLHS->GetOp0(), iRHS->GetOp0());
	}

int Comparer::CompareBinary(ZRef<ZExpr_Op2_T<Expr_Rel> > iLHS, ZRef<ZExpr_Op2_T<Expr_Rel> > iRHS)
	{
	if (int compare = sCompare_T(iLHS->GetOp0(), iRHS->GetOp0()))
		return compare;
	return sCompare_T(iLHS->GetOp1(), iRHS->GetOp1());
	}

struct Comparer_GT_Calc : public Comparer
	{ virtual void Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>&) { pSetResult(1); } };

struct Comparer_GT_Concrete : public Comparer_GT_Calc
	{ virtual void Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>&) { pSetResult(1); } };

struct Comparer_GT_Const : public Comparer_GT_Concrete
	{ virtual void Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>&) { pSetResult(1); } };

struct Comparer_GT_Difference : public Comparer_GT_Const
	{ virtual void Visit_Expr_Rel_Difference(const ZRef<Expr_Rel_Difference>&) { pSetResult(1); } };

struct Comparer_GT_Embed : public Comparer_GT_Difference
	{ virtual void Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>&) { pSetResult(1); } };

struct Comparer_GT_Intersect : public Comparer_GT_Embed
	{ virtual void Visit_Expr_Rel_Intersect(const ZRef<Expr_Rel_Intersect>&) { pSetResult(1); } };

struct Comparer_GT_Product : public Comparer_GT_Intersect
	{ virtual void Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>&) { pSetResult(1); } };

struct Comparer_GT_Project : public Comparer_GT_Product
	{ virtual void Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>&) { pSetResult(1); } };

struct Comparer_GT_Rename : public Comparer_GT_Project
	{ virtual void Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>&) { pSetResult(1); } };

struct Comparer_GT_Restrict : public Comparer_GT_Rename
	{ virtual void Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict_Any>&) { pSetResult(1); } };

struct Comparer_GT_Select : public Comparer_GT_Restrict
	{ virtual void Visit_Expr_Rel_Select(const ZRef<Expr_Rel_Select>&) { pSetResult(1); } };

struct Comparer_GT_Union : public Comparer_GT_Select
	{ virtual void Visit_Expr_Rel_Union(const ZRef<Expr_Rel_Union>&) { pSetResult(1); } };

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Type-specific derivations (anonymous)

namespace { // anonymous

struct Comparer_Calc : public Comparer
	{
	ZRef<Expr_Rel_Calc> fExpr;
	Comparer_Calc(ZRef<Expr_Rel_Calc> iExpr) : fExpr(iExpr) {}
	virtual void Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr)
		{
		if (int compare = sCompare_T(fExpr->GetRelName(), iExpr->GetRelName()))
			pSetResult(compare);
		else if (fExpr->GetCallable() < iExpr->GetCallable())
			pSetResult(-1);
		else if (iExpr->GetCallable() < fExpr->GetCallable())
			pSetResult(1);
		else
			pSetResult(CompareUnary(fExpr, iExpr));
		}
	};

struct Comparer_Concrete : public Comparer_GT_Calc
	{
	ZRef<Expr_Rel_Concrete> fExpr;
	Comparer_Concrete(ZRef<Expr_Rel_Concrete> iExpr) : fExpr(iExpr) {}
	virtual void Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr)
		{ pSetResult(sCompare_T(fExpr->GetConcreteRelHead(), iExpr->GetConcreteRelHead())); }
	};

struct Comparer_Const : public Comparer_GT_Concrete
	{
	ZRef<Expr_Rel_Const> fExpr;
	Comparer_Const(ZRef<Expr_Rel_Const> iExpr) : fExpr(iExpr) {}
	virtual void Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr)
		{
		if (int compare = sCompare_T(fExpr->GetRelName(), iExpr->GetRelName()))
			pSetResult(compare);
		else if (int compare = sCompare_T(fExpr->GetVal(), iExpr->GetVal()))
			pSetResult(compare);
		else
			pSetResult(CompareUnary(fExpr, iExpr));
		}
	};

struct Comparer_Difference : public Comparer_GT_Const
	{
	ZRef<Expr_Rel_Difference> fExpr;
	Comparer_Difference(ZRef<Expr_Rel_Difference> iExpr) : fExpr(iExpr) {}
	virtual void Visit_Expr_Rel_Difference(const ZRef<Expr_Rel_Difference>& iExpr)
		{ pSetResult(CompareBinary(fExpr, iExpr)); }
	};

struct Comparer_Embed : public Comparer_GT_Difference
	{
	ZRef<Expr_Rel_Embed> fExpr;
	Comparer_Embed(ZRef<Expr_Rel_Embed> iExpr) : fExpr(iExpr) {}
	virtual void Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>& iExpr)
		{
		if (int compare = sCompare_T(fExpr->GetRelName(), iExpr->GetRelName()))
			pSetResult(compare);
		else
			pSetResult(CompareBinary(fExpr, iExpr));
		}
	};

struct Comparer_Intersect : public Comparer_GT_Embed
	{
	ZRef<Expr_Rel_Intersect> fExpr;
	Comparer_Intersect(ZRef<Expr_Rel_Intersect> iExpr) : fExpr(iExpr) {}
	virtual void Visit_Expr_Rel_Intersect(const ZRef<Expr_Rel_Intersect>& iExpr)
		{ pSetResult(CompareBinary(fExpr, iExpr)); }
	};

struct Comparer_Product : public Comparer_GT_Intersect
	{
	ZRef<Expr_Rel_Product> fExpr;
	Comparer_Product(ZRef<Expr_Rel_Product> iExpr) : fExpr(iExpr) {}
	virtual void Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr)
		{ pSetResult(CompareBinary(fExpr, iExpr)); }
	};

struct Comparer_Project : public Comparer_GT_Product
	{
	ZRef<Expr_Rel_Project> fExpr;
	Comparer_Project(ZRef<Expr_Rel_Project> iExpr) : fExpr(iExpr) {}
	virtual void Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr)
		{
		if (int compare = sCompare_T(fExpr->GetProjectRelHead(), iExpr->GetProjectRelHead()))
			pSetResult(compare);
		else
			pSetResult(CompareUnary(fExpr, iExpr));
		}
	};

struct Comparer_Rename : public Comparer_GT_Project
	{
	ZRef<Expr_Rel_Rename> fExpr;
	Comparer_Rename(ZRef<Expr_Rel_Rename> iExpr) : fExpr(iExpr) {}
	virtual void Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
		{
		if (int compare = sCompare_T(fExpr->GetOld(), iExpr->GetOld()))
			pSetResult(compare);
		else if (int compare = sCompare_T(fExpr->GetNew(), iExpr->GetNew()))
			pSetResult(compare);
		else
			pSetResult(CompareUnary(fExpr, iExpr));
		}
	};

struct Comparer_Restrict : public Comparer_GT_Rename
	{
	ZRef<Expr_Rel_Restrict_Any> fExpr;
	Comparer_Restrict(ZRef<Expr_Rel_Restrict_Any> iExpr) : fExpr(iExpr) {}
	virtual void Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict_Any>& iExpr)
		{
		if (int compare = sCompare_T(fExpr->GetValPred(), iExpr->GetValPred()))
			pSetResult(compare);
		else
			pSetResult(CompareUnary(fExpr, iExpr));
		}
	};

struct Comparer_Select : public Comparer_GT_Restrict
	{
	ZRef<Expr_Rel_Select> fExpr;
	Comparer_Select(ZRef<Expr_Rel_Select> iExpr) : fExpr(iExpr) {}
	virtual void Visit_Expr_Rel_Select(const ZRef<Expr_Rel_Select>& iExpr)
		{
		if (int compare = Visitor_Expr_Bool_ValPred_Any_DoCompare::Comparer_Bootstrap()
			.Compare(fExpr->GetExpr_Bool(), iExpr->GetExpr_Bool()))
			{
			pSetResult(compare);
			}
		else
			{
			pSetResult(CompareUnary(fExpr, iExpr));
			}
		}
	};

struct Comparer_Union : public Comparer_GT_Select
	{
	ZRef<Expr_Rel_Union> fExpr;
	Comparer_Union(ZRef<Expr_Rel_Union> iExpr) : fExpr(iExpr) {}
	virtual void Visit_Expr_Rel_Union(const ZRef<Expr_Rel_Union>& iExpr)
		{ pSetResult(CompareBinary(fExpr, iExpr)); }
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Bootstrap

namespace { // anonymous

struct Comparer_Bootstrap
:	public virtual ZVisitor_Do_T<int>
,	public virtual Visitor_Expr_Rel_Calc
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Const
,	public virtual Visitor_Expr_Rel_Difference
,	public virtual Visitor_Expr_Rel_Embed
,	public virtual Visitor_Expr_Rel_Intersect
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict_Any
,	public virtual Visitor_Expr_Rel_Select
,	public virtual Visitor_Expr_Rel_Union
	{
	ZRef<Expr_Rel> fExpr;
	Comparer_Bootstrap(ZRef<Expr_Rel> iExpr) : fExpr(iExpr) {}

	virtual void Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr)
		{ pSetResult(Comparer_Calc(iExpr).Do(fExpr)); } 

	virtual void Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr)
		{ pSetResult(Comparer_Concrete(iExpr).Do(fExpr)); } 

	virtual void Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr)
		{ pSetResult(Comparer_Const(iExpr).Do(fExpr)); } 

	virtual void Visit_Expr_Rel_Difference(const ZRef<Expr_Rel_Difference>& iExpr)
		{ pSetResult(Comparer_Difference(iExpr).Do(fExpr)); } 

	virtual void Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>& iExpr)
		{ pSetResult(Comparer_Embed(iExpr).Do(fExpr)); } 

	virtual void Visit_Expr_Rel_Intersect(const ZRef<Expr_Rel_Intersect>& iExpr)
		{ pSetResult(Comparer_Intersect(iExpr).Do(fExpr)); } 

	virtual void Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr)
		{ pSetResult(Comparer_Product(iExpr).Do(fExpr)); } 

	virtual void Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr)
		{ pSetResult(Comparer_Project(iExpr).Do(fExpr)); } 

	virtual void Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
		{ pSetResult(Comparer_Rename(iExpr).Do(fExpr)); } 

	virtual void Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict_Any>& iExpr)
		{ pSetResult(Comparer_Restrict(iExpr).Do(fExpr)); } 

	virtual void Visit_Expr_Rel_Select(const ZRef<Expr_Rel_Select>& iExpr)
		{ pSetResult(Comparer_Select(iExpr).Do(fExpr)); } 

	virtual void Visit_Expr_Rel_Union(const ZRef<Expr_Rel_Union>& iExpr)
		{ pSetResult(Comparer_Union(iExpr).Do(fExpr)); } 
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * sCompare_T<ZRef<ZRA::Expr_Rel> >

template <>
int sCompare_T(const ZRef<ZRA::Expr_Rel>& iLHS, const ZRef<ZRA::Expr_Rel>& iRHS)
	{ return Comparer_Bootstrap(iLHS).Do(iRHS); }

} // namespace ZooLib
