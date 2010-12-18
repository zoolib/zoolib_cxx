#if 0
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

#include "zoolib/ZCompare_Vector.h"
#include "zoolib/ZExpr_Bool_ValPred.h"
#include "zoolib/ZVisitor_Expr_Op_DoTransform_T.h"

#include "zoolib/dataspace/ZDataspace_SearchSpec.h"

#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Explicit.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"
#include "zoolib/zra/ZRA_Expr_Rel_Select.h"

namespace ZooLib {
namespace ZDataspace {

using namespace ZRA;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * SearchSpec

bool operator==(const SearchSpec& l, const SearchSpec& r)
	{ return sCompare_T(l, r) == 0; }

bool operator<(const SearchSpec& l, const SearchSpec& r)
	{ return sCompare_T(l, r) < 0; }

// =================================================================================================
#pragma mark -
#pragma mark * DoRename (anonymous)

namespace { // anonymous

ZNameVal spRename(const ZNameVal& iNameVal, const RelName& iOld, const RelName& iNew)
	{
	if (iNameVal.first == iOld)
		return ZNameVal(iNew, iNameVal.second);
	return iNameVal;
	}

class DoRename
:	public virtual ZVisitor_Expr_Op_DoTransform_T<ZExpr_Bool>
,	public virtual ZVisitor_Expr_Bool_ValPred
	{
public:
	DoRename(const Rename_t& iRename);

	virtual void Visit_Expr_Bool_ValPred(ZRef<ZExpr_Bool_ValPred> iExpr);
private:
	const Rename_t& fRename;
	};

DoRename::DoRename(const Rename_t& iRename)
:	fRename(iRename)
	{}

void DoRename::Visit_Expr_Bool_ValPred(ZRef<ZExpr_Bool_ValPred> iExpr)
	{
	ZValPred result;
	if (iExpr->GetValPred().Renamed(fRename, result))
		this->pSetResult(new ZExpr_Bool_ValPred(result));
	else
		this->pSetResult(iExpr);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Gather_t (anonymous)

namespace { // anonymous

class Gather_t
	{
public:
	Gather_t();
	Gather_t(const ZNameVal& iNameVal);
	Gather_t(const RelHead& iRelHead);

	ZRef<ZExpr_Bool> fExpr_Bool;
	vector<NameMap> fNameMaps;
	vector<ZNameVal> fNameVals;
	};

Gather_t::Gather_t()
	{}

Gather_t::Gather_t(const ZNameVal& iNameVal)
:	fExpr_Bool(sTrue())
,	fNameVals(1, iNameVal)
	{}

Gather_t::Gather_t(const RelHead& iRelHead)
:	fExpr_Bool(sTrue())
,	fNameMaps(1, iRelHead)
	{}

class Gather
:	public virtual ZVisitor_Do_T<Gather_t>
,	public virtual ZRA::Visitor_Expr_Rel_Product
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
,	public virtual ZRA::Visitor_Expr_Rel_Project
,	public virtual ZRA::Visitor_Expr_Rel_Rename
,	public virtual ZRA::Visitor_Expr_Rel_Restrict
,	public virtual ZRA::Visitor_Expr_Rel_Select
,	public virtual ZRA::Visitor_Expr_Rel_Explicit
	{
public:
	virtual void Visit_Expr_Op0(ZRef<ZExpr_Op0_T<Expr_Rel> > iExpr);
	virtual void Visit_Expr_Op1(ZRef<ZExpr_Op1_T<Expr_Rel> > iExpr);
	virtual void Visit_Expr_Op2(ZRef<ZExpr_Op2_T<Expr_Rel> > iExpr);

	virtual void Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr);
	virtual void Visit_Expr_Rel_Product(ZRef<Expr_Rel_Product> iExpr);

	virtual void Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr);
	virtual void Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr);
	virtual void Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr);
	virtual void Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr);

	virtual void Visit_Expr_Rel_Explicit(ZRef<Expr_Rel_Explicit> iExpr);
	};

void Gather::Visit_Expr_Op0(ZRef<ZExpr_Op0_T<Expr_Rel> > iExpr)
	{ ZUnimplemented(); }

void Gather::Visit_Expr_Op1(ZRef<ZExpr_Op1_T<Expr_Rel> > iExpr)
	{ ZUnimplemented(); }

void Gather::Visit_Expr_Op2(ZRef<ZExpr_Op2_T<Expr_Rel> > iExpr)
	{ ZUnimplemented(); }

void Gather::Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr)
	{
	Gather_t g0 = this->Do(iExpr->GetOp0());
	const RelHead& theRH = iExpr->GetProjectRelHead();
	for (vector<NameMap>::iterator iterNM = g0.fNameMaps.begin();
		iterNM != g0.fNameMaps.end(); ++iterNM)
		{
		set<NameMap::Elem_t> newElems;
		set<NameMap::Elem_t>& elems = iterNM->GetElems();
		for (set<NameMap::Elem_t>::const_iterator iterElem = elems.begin();
			iterElem != elems.end(); ++iterElem)
			{
			if (theRH.Contains(iterElem->first))
				newElems.insert(*iterElem);
			}
		elems.swap(newElems);
		}

	this->pSetResult(g0);
	}

void Gather::Visit_Expr_Rel_Product(ZRef<Expr_Rel_Product> iExpr)
	{
	Gather_t g0 = this->Do(iExpr->GetOp0());
	const Gather_t g1 = this->Do(iExpr->GetOp1());

	g0.fNameVals.insert(g0.fNameVals.end(), g1.fNameVals.begin(), g1.fNameVals.end());
	g0.fNameMaps.insert(g0.fNameMaps.end(), g1.fNameMaps.begin(), g1.fNameMaps.end());
	g0.fExpr_Bool &= g1.fExpr_Bool;

	this->pSetResult(g0);
	}

void Gather::Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr)
	{ this->pSetResult(Gather_t(iExpr->GetConcreteRelHead())); }

void Gather::Visit_Expr_Rel_Explicit(ZRef<Expr_Rel_Explicit> iExpr)
	{ this->pSetResult(Gather_t(ZNameVal(iExpr->GetRelName(), iExpr->GetVal()))); }

void Gather::Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr)
	{
	Gather_t g0 = this->Do(iExpr->GetOp0());

	const RelName& oldName = iExpr->GetOld();
	const RelName& newName = iExpr->GetNew();

	for (vector<ZNameVal>::iterator i = g0.fNameVals.begin(); i != g0.fNameVals.end(); ++i)
		*i = spRename(*i, oldName, newName);

	for (vector<NameMap>::iterator i = g0.fNameMaps.begin(); i != g0.fNameMaps.end(); ++i)
		i->ApplyToFrom(newName, oldName);

	Rename_t theRename;
	theRename[oldName] = newName;

	g0.fExpr_Bool = DoRename(theRename).Do(g0.fExpr_Bool);

	this->pSetResult(g0);
	}

void Gather::Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr)
	{
	Gather_t g0 = this->Do(iExpr->GetOp0());
	g0.fExpr_Bool = g0.fExpr_Bool & iExpr->GetValPred();
	this->pSetResult(g0);
	}

void Gather::Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr)
	{
	Gather_t g0 = this->Do(iExpr->GetOp0());
	g0.fExpr_Bool = g0.fExpr_Bool & iExpr->GetExpr_Bool();
	this->pSetResult(g0);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * SearchSpec

SearchSpec sAsSearchSpec(ZRef<ZRA::Expr_Rel> iRel)
	{
	const Gather_t theG = Gather().Do(iRel);

	SearchSpec result;
	result.fNameVals = theG.fNameVals;
	result.fNameMaps = theG.fNameMaps;
	result.fPredCompound = sAsValPredCompound(theG.fExpr_Bool);

	return result;
	}

} // namespace ZDataspace
} // namespace ZooLib

namespace ZooLib {

template <> int sCompare_T(const ZDataspace::SearchSpec& iL, const ZDataspace::SearchSpec& iR)
	{
	if (int compare = sCompare_T(iL.fNameVals, iR.fNameVals))
		return compare;
	if (int compare = sCompare_T(iL.fNameMaps, iR.fNameMaps))
		return compare;
	return sCompare_T(iL.fPredCompound, iR.fPredCompound);
	}
} // namespace ZooLib
#endif