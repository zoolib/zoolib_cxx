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

#include "zoolib/ZExpr_Logic_ValPred.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZVisitor_Expr_Op_DoTransform_T.h"
#include "zoolib/ZVisitor_Expr_Logic_ValPred_DoToStrim.h"

#include "zoolib/dataspace/ZDataspace_Source.h"

#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"
#include "zoolib/zra/ZRA_Expr_Rel_Select.h"

#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

namespace ZooLib {
namespace ZDataspace {
using std::set;
using namespace ZRA;

static ZRef<Expr_Rel> spConcrete(
	ZRef<ConcreteDomain> iConcreteDomain, const string8& iName, const NameMap& iNameMap)
	{
	ZRef<Expr_Rel> theRel = sConcrete(iConcreteDomain, iName, iNameMap.GetRelHead_From());
	const set<NameMap::Elem_t>& theElems = iNameMap.GetElems();
	for (set<NameMap::Elem_t>::const_iterator i = theElems.begin(); i != theElems.end(); ++i)
		{
		if ((*i).first != (*i).second)
			theRel = sRename(theRel, (*i).first, (*i).second);
		}
	return theRel;
	}

const ZStrimW& operator<<(const ZStrimW& w, const SearchThing& iST)
	{
	w << "(";
	bool isSubsequent = false;
	for (vector<NameMap>::const_iterator i = iST.fNameMaps.begin(); i != iST.fNameMaps.end(); ++i)
		{
		if (isSubsequent)
			w << ", ";
		isSubsequent = true;
		w << *i;
		}
	w << ")\n";
	ZVisitor_Expr_Logic_ValPred_DoToStrim().DoToStrim(ZVisitor_DoToStrim::Options(), w, sAsExpr_Logic(iST.fPredCompound));
	return w;
	}

const ZStrimW& operator<<(const ZStrimW& w, const RelHead& iRH)
	{
	ZRA::Util_Strim_RelHead::sWrite_RelHead(iRH, w);
	return w;
	}

const ZStrimW& operator<<(const ZStrimW& w, const set<RelHead>& iSet)
	{
	bool isSubsequent = false;
	for (set<RelHead>::const_iterator i = iSet.begin(); i != iSet.end(); ++i)
		{
		if (isSubsequent)
			w << ", ";
		w << *i;
		}
	return w;
	}

// =================================================================================================
#pragma mark -
#pragma mark * DoRename (anonymous)

namespace { // anonymous

class DoRename
:	public virtual ZVisitor_Expr_Op_DoTransform_T<ZExpr_Logic>
,	public virtual ZVisitor_Expr_Logic_ValPred
	{
public:
	DoRename(const Rename_t& iRename);

	virtual void Visit_Expr_Logic_ValPred(ZRef<ZExpr_Logic_ValPred> iExpr);
private:
	const Rename_t& fRename;
	};

DoRename::DoRename(const Rename_t& iRename)
:	fRename(iRename)
	{}

void DoRename::Visit_Expr_Logic_ValPred(ZRef<ZExpr_Logic_ValPred> iExpr)
	{
	ZValPred result;
	if (iExpr->GetValPred().Renamed(fRename, result))
		this->pSetResult(new ZExpr_Logic_ValPred(result));
	else
		this->pSetResult(iExpr);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Gather (anonymous)

namespace { // anonymous

class Gather_t
	{
public:
	Gather_t();
	Gather_t(ZRef<ZExpr_Logic> iExpr_Logic, const RelHead& iRelHead);
	Gather_t(ZRef<ZExpr_Logic> iExpr_Logic, const vector<NameMap>& iNameMaps);

	ZRef<ZExpr_Logic> fExpr_Logic;
	vector<NameMap> fNameMaps;
	};

Gather_t::Gather_t()
	{}

Gather_t::Gather_t(ZRef<ZExpr_Logic> iExpr_Logic, const RelHead& iRelHead)
:	fExpr_Logic(iExpr_Logic)
,	fNameMaps(1, iRelHead)
	{}

Gather_t::Gather_t(ZRef<ZExpr_Logic> iExpr_Logic, const vector<NameMap>& iNameMaps)
:	fExpr_Logic(iExpr_Logic)
,	fNameMaps(iNameMaps)
	{}

class Gather
:	public virtual ZVisitor_Do_T<Gather_t>
,	public virtual ZRA::Visitor_Expr_Rel_Product
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
//,	public virtual ZRA::Visitor_Expr_Rel_Project
,	public virtual ZRA::Visitor_Expr_Rel_Rename
,	public virtual ZRA::Visitor_Expr_Rel_Restrict
,	public virtual ZRA::Visitor_Expr_Rel_Select
	{
public:
	virtual void Visit_Expr_Op0(ZRef<ZExpr_Op0_T<ZRef<Expr_Rel> > > iExpr);
	virtual void Visit_Expr_Op1(ZRef<ZExpr_Op1_T<ZRef<Expr_Rel> > > iExpr);
	virtual void Visit_Expr_Op2(ZRef<ZExpr_Op2_T<ZRef<Expr_Rel> > > iExpr);

	virtual void Visit_Expr_Rel_Product(ZRef<Expr_Rel_Product> iExpr);

	virtual void Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr);
	virtual void Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr);
	virtual void Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr);
	virtual void Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr);
	};

void Gather::Visit_Expr_Op0(ZRef<ZExpr_Op0_T<ZRef<Expr_Rel> > > iExpr)
	{ ZUnimplemented(); }

void Gather::Visit_Expr_Op1(ZRef<ZExpr_Op1_T<ZRef<Expr_Rel> > > iExpr)
	{ ZUnimplemented(); }

void Gather::Visit_Expr_Op2(ZRef<ZExpr_Op2_T<ZRef<Expr_Rel> > > iExpr)
	{ ZUnimplemented(); }

void Gather::Visit_Expr_Rel_Product(ZRef<Expr_Rel_Product> iExpr)
	{
	Gather_t g0 = this->Do(iExpr->GetOp0());
	const Gather_t g1 = this->Do(iExpr->GetOp1());

	g0.fNameMaps.insert(g0.fNameMaps.end(), g1.fNameMaps.begin(), g1.fNameMaps.end());
	g0.fExpr_Logic &= g1.fExpr_Logic;

	this->pSetResult(g0);
	}

void Gather::Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr)
	{ this->pSetResult(Gather_t(sTrue(), iExpr->GetRelHead())); }

void Gather::Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr)
	{
	Gather_t g0 = this->Do(iExpr->GetOp0());

	const RelName& oldName = iExpr->GetOld();
	const RelName& newName = iExpr->GetNew();

	for (vector<NameMap>::iterator i = g0.fNameMaps.begin(); i != g0.fNameMaps.end(); ++i)
		(*i).ApplyToFrom(newName, oldName);

	Rename_t theRename;
	theRename[oldName] = newName;

	g0.fExpr_Logic = DoRename(theRename).Do(g0.fExpr_Logic);

	this->pSetResult(g0);
	}

void Gather::Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr)
	{
	Gather_t g0 = this->Do(iExpr->GetOp0());
	g0.fExpr_Logic = g0.fExpr_Logic & iExpr->GetValPred();
	this->pSetResult(g0);
	}

void Gather::Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr)
	{
	Gather_t g0 = this->Do(iExpr->GetOp0());
	g0.fExpr_Logic = g0.fExpr_Logic & iExpr->GetExpr_Logic();
	this->pSetResult(g0);
	}

} // anonymous namespace

SearchThing sAsSearchThing(ZRef<ZRA::Expr_Rel> iRel)
	{
	const Gather_t theG = Gather().Do(iRel);

	SearchThing result;
	result.fNameMaps = theG.fNameMaps;
	result.fPredCompound = sAsValPredCompound(theG.fExpr_Logic);

	return result;
	}

ZRef<Expr_Rel> sAsRel(const RelHead& iRelHead)
	{ return sConcrete(null, "Fake", iRelHead); }

ZRef<Expr_Rel> sAsRel(const NameMap& iNameMap)
	{ return spConcrete(null, "FakeRenamed", iNameMap); }

ZRef<Expr_Rel> sAsRel(const vector<NameMap>& iNameMaps)
	{
	ZRef<Expr_Rel> result;
	for (vector<NameMap>::const_iterator i = iNameMaps.begin();
		i != iNameMaps.end(); ++i)
		{
		result = result * sAsRel(*i);
		}
	return result;
	}

ZRef<Expr_Rel> sAsRelFrom(const vector<NameMap>& iNameMaps)
	{
	ZRef<Expr_Rel> result;
	for (vector<NameMap>::const_iterator i = iNameMaps.begin();
		i != iNameMaps.end(); ++i)
		{
		result = result * sConcrete(null, "FakeProduct", (*i).GetRelHead_From());
		}
	return result;
	}

ZRef<Expr_Rel> sAsRel(const SearchThing& iSearchThing)
	{
	return sAsRel(iSearchThing.fNameMaps) & sAsExpr_Logic(iSearchThing.fPredCompound);
	}

// =================================================================================================
#pragma mark -
#pragma mark *

AddedSearch::AddedSearch(int64 iRefcon, const SearchThing& iSearchThing)
:	fRefcon(iRefcon)
,	fSearchThing(iSearchThing)
	{}

// =================================================================================================
#pragma mark -
#pragma mark *

Source::Source()
	{}

Source::~Source()
	{}

void Source::Register(ZRef<Callable> iCallable)
	{ fCallables.Register(iCallable); }

void Source::Unregister(ZRef<Callable> iCallable)
	{ fCallables.Unregister(iCallable); }

void Source::pInvokeCallables()
	{ fCallables.Invoke(this); }

} // namespace ZDataspace
} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark *

#if 0

class DeltaReadable
	{
	vector<ReadableBy> fReadableBy;
	vector<ZData> fInserted;
	vector<DataHash> fErased;
	};

class DeltaPack
	{
	Timestamp fTimestamp;
	WrittenBy fWrittenBy;
	vector<DeltaReadable> fDRs;
	};

Source
	{
	void Register(ZRef<Searcher>, Rel iRel);

	void Update();
	void Commit();
	};

// Synced from elsewhere,, but no writes possible.
// Some readonly datasets will actually fetch *everything* from elsewhere,
// which they can then pass on to others, but will only make a subset
// usable by the database layer.
Source_ReadOnly
	{
	};

class Source_Client
	{
	// Specify the list of ReadableBys we care about for this connection.
	// If we're readwrite on the connection we'll also need to specify our WrittenBy thing.
	};

Source_ReadWrite
	{
	// Specify WrittenBy thing
	void Insert(ReadableSet iRS, ZData iData);
	void Erase(ReadableSet iRS, ZData iData);
	};

#endif
