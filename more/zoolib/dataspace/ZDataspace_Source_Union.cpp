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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZExpr_Bool_ValPred.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_vector.h"
#include "zoolib/ZValPred_GetNames.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_Do_GetNames.h"
#include "zoolib/ZVisitor_Expr_Op_Do_Transform_T.h"

#include "zoolib/dataspace/ZDataspace_Source_Union.h"
#include "zoolib/dataspace/ZDataspace_Util_Strim.h"

#include "zoolib/zqe/ZQE_Search.h"
#include "zoolib/zqe/ZQE_Visitor_DoMakeWalker.h"

#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Embed.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

#include "zoolib/zra/ZRA_Transform_ConsolidateRenames.h"
#include "zoolib/zra/ZRA_Transform_Thing.h"

namespace ZooLib {
namespace ZDataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * InsertPrefix (anonymous)

namespace { // anonymous

class InsertPrefix
:	public virtual ZVisitor_Expr_Op_Do_Transform_T<ZRA::Expr_Rel>
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
	{
	typedef ZVisitor_Expr_Op_Do_Transform_T<ZRA::Expr_Rel> inherited;
public:
	InsertPrefix(const string8& iPrefix);

// From ZRA::Visitor_Expr_Rel_Concrete
	virtual void Visit_Expr_Rel_Concrete(const ZRef<ZRA::Expr_Rel_Concrete>& iExpr);

	const string8 fPrefix;
	};

InsertPrefix::InsertPrefix(const string8& iPrefix)
:	fPrefix(iPrefix)
	{}

void InsertPrefix::Visit_Expr_Rel_Concrete(const ZRef<ZRA::Expr_Rel_Concrete>& iExpr)
	{
	const RelHead& theRelHead = iExpr->GetConcreteRelHead();
	
	const RelHead newRelHead = ZRA::sPrefixErase(fPrefix, theRelHead);
	ZRef<ZRA::Expr_Rel> theRel = ZRA::sConcrete(newRelHead);
	for (RelHead::const_iterator i = newRelHead.begin(); i != newRelHead.end(); ++i)
		theRel = sRename(theRel, ZRA::sPrefixInsert(fPrefix, *i), *i);
	
	this->pSetResult(theRel);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::PIP

class Source_Union::DLink_PIP_InProxy
:	public DListLink<PIP, DLink_PIP_InProxy, kDebug>
	{};

class Source_Union::DLink_PIP_NeedsWork
:	public DListLink<PIP, DLink_PIP_NeedsWork, kDebug>
	{};

class Source_Union::PIP
:	public DLink_PIP_InProxy
,	public DLink_PIP_NeedsWork
	{
public:
	Proxy* fProxy;
	PSource* fPSource;
	bool fNeedsAdd;
	int64 fRefcon;
	ZRef<ZQE::Result> fResult;
	ZRef<Event> fEvent;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Proxy declaration

class Source_Union::Proxy
:	public virtual ZRA::Expr_Rel
,	public virtual ZExpr_Op0_T<ZRA::Expr_Rel>
	{
	typedef ZExpr_Op0_T<Expr_Rel> inherited;
public:
	Proxy(Source_Union* iSource);

// From ZCounted
	virtual void Finalize();

// From ZVisitee
	virtual void Accept(ZVisitor& iVisitor);

// From ZExpr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZRef<ZRA::Expr_Rel> Self();
	virtual ZRef<ZRA::Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Proxy(Visitor_Proxy& iVisitor);

	Source_Union* fSource;
	ZRef<ZRA::Expr_Rel> fRel;
	ZRA::RelHead fResultRelHead;
	set<PSearch*> fDependentPSearches;

	// Something that ties this proxy to each Source.
	DListHead<DLink_PIP_InProxy> fPIP_InProxy;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Visitor_Proxy

class Source_Union::Visitor_Proxy
:	public virtual ZVisitor_Expr_Op0_T<ZRA::Expr_Rel>
	{
	typedef ZVisitor_Expr_Op0_T<ZRA::Expr_Rel> inherited;
public:
	virtual void Visit_Proxy(const ZRef<Proxy>& iExpr)
		{ this->Visit_Expr_Op0(iExpr); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Proxy definition

Source_Union::Proxy::Proxy(Source_Union* iSource)
:	fSource(iSource)
	{}

void Source_Union::Proxy::Finalize()
	{ fSource->pFinalizeProxy(this); }

void Source_Union::Proxy::Accept(ZVisitor& iVisitor)
	{
	if (Visitor_Proxy* theVisitor = dynamic_cast<Visitor_Proxy*>(&iVisitor))
		this->Accept_Proxy(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Source_Union::Proxy::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZRA::Expr_Rel>& iVisitor)
	{
	if (Visitor_Proxy* theVisitor = dynamic_cast<Visitor_Proxy*>(&iVisitor))
		this->Accept_Proxy(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZRef<ZRA::Expr_Rel> Source_Union::Proxy::Self()
	{ return this; }

ZRef<ZRA::Expr_Rel> Source_Union::Proxy::Clone()
	{ return this; }

void Source_Union::Proxy::Accept_Proxy(Visitor_Proxy& iVisitor)
	{ iVisitor.Visit_Proxy(this); }

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Walker_Proxy

class Source_Union::Walker_Proxy : public ZQE::Walker
	{
public:
	Walker_Proxy(ZRef<Source_Union> iSource, ZRef<Proxy> iProxy)
	:	fSource(iSource)
	,	fProxy(iProxy)
	,	fIter_PIP(iProxy->fPIP_InProxy)
		{}

	virtual ~Walker_Proxy()
		{}

	virtual void Rewind()
		{ fSource->pRewind(this); }

	virtual ZRef<Walker> Prime(
		const map<string8,size_t>& iOffsets,
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset)
		{
		fSource->pPrime(this, iOffsets, oOffsets, ioBaseOffset);
		return this;
		}

	virtual bool ReadInc(
		ZVal_Any* ioResults,
		set<ZRef<ZCounted> >* oAnnotations)
		{ return fSource->pReadInc(this, ioResults, oAnnotations); }

	ZRef<Source_Union> fSource;
	ZRef<Proxy> fProxy;
	size_t fBaseOffset;

	DListIterator<PIP, DLink_PIP_InProxy> fIter_PIP;
	ZRef<ZQE::Result> fCurrentResult;
	size_t fCurrentIndex;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::ClientSearch

class Source_Union::DLink_ClientSearch_InPSearch
:	public DListLink<ClientSearch, DLink_ClientSearch_InPSearch, kDebug>
	{};

class Source_Union::ClientSearch
:	public DLink_ClientSearch_InPSearch
	{
public:
	ClientSearch(int64 iRefcon, PSearch* iPSearch)
	:	fRefcon(iRefcon)
	,	fPSearch(iPSearch)
		{}

	int64 fRefcon;
	PSearch* fPSearch;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::PSearch

class Source_Union::DLink_PSearch_NeedsWork
:	public DListLink<PSearch, DLink_PSearch_NeedsWork, kDebug>
	{};

class Source_Union::PSearch
:	public DLink_PSearch_NeedsWork
	{
public:
	PSearch(ZRef<ZRA::Expr_Rel> iRel)
	:	fRel(iRel)
		{}

	ZRef<ZRA::Expr_Rel> fRel;
	ZRef<ZRA::Expr_Rel> fRel_Analyzed;
	set<ZRef<Proxy> > fProxiesDependedUpon;
	ZRef<ZQE::Result> fResult;
	DListHead<DLink_ClientSearch_InPSearch> fClientSearches;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Visitor_DoMakeWalker

class Source_Union::Visitor_DoMakeWalker
:	public virtual ZQE::Visitor_DoMakeWalker
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Proxy
	{
public:
	Visitor_DoMakeWalker(ZRef<Source_Union> iSource)
	:	fSource(iSource)
		{}

	virtual void Visit_Expr_Rel_Concrete(const ZRef<ZRA::Expr_Rel_Concrete>& iExpr)
		{
		// We should never see a Concrete as we're being used to make walkers
		// for an analyzed rel.
		ZUnimplemented();
		}

	virtual void Visit_Proxy(const ZRef<Proxy>& iExpr)
		{ this->pSetResult(fSource->pMakeWalker(iExpr)); }

	ZRef<Source_Union> fSource;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Analyze

class Source_Union::Analyze
:	public virtual ZVisitor_Expr_Op_Do_Transform_T<ZRA::Expr_Rel>
,	public virtual ZRA::Visitor_Expr_Rel_Product
,	public virtual ZRA::Visitor_Expr_Rel_Embed
,	public virtual ZRA::Visitor_Expr_Rel_Project
,	public virtual ZRA::Visitor_Expr_Rel_Rename
,	public virtual ZRA::Visitor_Expr_Rel_Calc
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
,	public virtual ZRA::Visitor_Expr_Rel_Const
	{
	typedef ZVisitor_Expr_Op_Do_Transform_T<ZRA::Expr_Rel> inherited;
public:
	Analyze(Source_Union* iSource_Union, PSearch* iPSearch);

// From ZVisitor_Expr_Op2_T via ZVisitor_Expr_Op_DoTransform_T
	virtual void Visit_Expr_Op2(const ZRef<ZExpr_Op2_T<ZRA::Expr_Rel> >& iExpr);

// From ZRA::Visitor_Expr_Rel_XXX
	virtual void Visit_Expr_Rel_Product(const ZRef<ZRA::Expr_Rel_Product>& iExpr);

	virtual void Visit_Expr_Rel_Embed(const ZRef<ZRA::Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZRef<ZRA::Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZRef<ZRA::Expr_Rel_Rename>& iExpr);

	virtual void Visit_Expr_Rel_Calc(const ZRef<ZRA::Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZRef<ZRA::Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Concrete(const ZRef<ZRA::Expr_Rel_Concrete>& iExpr);

// Our protocol
	void HandleOp2(const ZRef<ZExpr_Op2_T<ZRA::Expr_Rel> >& iExpr);
	ZRef<ZRA::Expr_Rel> TopLevelDo(ZRef<ZRA::Expr_Rel> iRel);

	Source_Union* fSource_Union;
	PSearch* fPSearch;
	set<PSource*> fPSources;

	ZRA::Rename fRename;
	ZRA::RelHead fResultRelHead;
	};

Source_Union::Analyze::Analyze(Source_Union* iSource_Union, PSearch* iPSearch)
:	fSource_Union(iSource_Union)
,	fPSearch(iPSearch)
	{}

void Source_Union::Analyze::Visit_Expr_Op2(const ZRef<ZExpr_Op2_T<ZRA::Expr_Rel> >& iExpr)
	{
	ZUnimplemented();
	}

void Source_Union::Analyze::Visit_Expr_Rel_Product(const ZRef<ZRA::Expr_Rel_Product>& iExpr)
	{
	// Visit left branch
	const ZRef<ZRA::Expr_Rel> oldOp0 = iExpr->GetOp0();
	const ZRef<ZRA::Expr_Rel> newOp0 = this->Do(oldOp0);

	// Remember which PSources it touches.
	set<PSource*> leftPSources;
	leftPSources.swap(fPSources);

	// And the relhead
	ZRA::RelHead leftRelHead;
	leftRelHead.swap(fResultRelHead);
	
	// Visit right branch
	const ZRef<ZRA::Expr_Rel> oldOp1 = iExpr->GetOp1();
	const ZRef<ZRA::Expr_Rel> newOp1 = this->Do(oldOp1);
	
	// Remember its PSources.
	set<PSource*> rightPSources;
	rightPSources.swap(fPSources);
	ZRA::RelHead rightRelHead;
	rightRelHead.swap(fResultRelHead);

	fPSources = ZUtil_STL::sOr(leftPSources, rightPSources);
	fResultRelHead = ZUtil_STL::sOr(leftRelHead, rightRelHead);

	if (leftPSources.size() <= 1)
		{
		// Our left branch is simple, it references zero or one source.
		if (fPSources.size() <= 1)
			{
			// And with the addition of the right branch we still reference <= 1 source.
			if (oldOp0 == newOp0 && oldOp1 == newOp1)
				this->pSetResult(iExpr->Self());
			else
				this->pSetResult(iExpr->Clone(newOp0, newOp1));
			}
		else
			{
			// This is the interesting scenario. With the addition of our right branch
			// we *now* reference multiple sources. We register a proxy for the left branch.
			ZRef<Proxy> proxy0 = fSource_Union->pGetProxy(fPSearch, leftPSources, leftRelHead, newOp0);

			if (rightPSources.size() <= 1)
				{
				// Right branch is simple, and thus won't have registered a proxy yet.
				ZRef<Proxy> proxy1 = fSource_Union->pGetProxy(fPSearch, rightPSources, rightRelHead, newOp1);
				this->pSetResult(iExpr->Clone(proxy0, proxy1));
				}
			else
				{
				this->pSetResult(iExpr->Clone(proxy0, newOp1));
				}
			}
		}
	else
		{
		// Our left branch itself references multiples sources.
		if (rightPSources.size() <= 1)
			{
			// Right branch is simple, and thus won't have registered a proxy yet.
			ZRef<Proxy> proxy1 = fSource_Union->pGetProxy(fPSearch, rightPSources, rightRelHead, newOp1);
			this->pSetResult(iExpr->Clone(newOp0, proxy1));
			}
		else
			{
			if (oldOp0 == newOp0 && oldOp1 == newOp1)
				this->pSetResult(iExpr->Self());
			else
				this->pSetResult(iExpr->Clone(newOp0, newOp1));
			}
		}
	}

void Source_Union::Analyze::Visit_Expr_Rel_Embed(const ZRef<ZRA::Expr_Rel_Embed>& iExpr)
	{
	ZRA::Visitor_Expr_Rel_Embed::Visit_Expr_Rel_Embed(iExpr);

	fResultRelHead = iExpr->GetRelName();
	}

void Source_Union::Analyze::Visit_Expr_Rel_Project(const ZRef<ZRA::Expr_Rel_Project>& iExpr)
	{
	ZRA::Visitor_Expr_Rel_Project::Visit_Expr_Rel_Project(iExpr);

	fResultRelHead &= iExpr->GetProjectRelHead();
	}

void Source_Union::Analyze::Visit_Expr_Rel_Rename(const ZRef<ZRA::Expr_Rel_Rename>& iExpr)
	{
	ZRA::Visitor_Expr_Rel_Rename::Visit_Expr_Rel_Rename(iExpr);

	if (ZUtil_STL::sEraseIfContains(fResultRelHead, iExpr->GetOld()))
		fResultRelHead |= iExpr->GetNew();
	}

void Source_Union::Analyze::Visit_Expr_Rel_Calc(const ZRef<ZRA::Expr_Rel_Calc>& iExpr)
	{
	ZRA::Visitor_Expr_Rel_Calc::Visit_Expr_Rel_Calc(iExpr);

	fResultRelHead |= iExpr->GetRelName();
	}

void Source_Union::Analyze::Visit_Expr_Rel_Concrete(const ZRef<ZRA::Expr_Rel_Concrete>& iExpr)
	{
	ZAssertStop(kDebug, fResultRelHead.empty());

	fResultRelHead = iExpr->GetConcreteRelHead();

	// Identify which PSources can service this concrete.
	fPSources = fSource_Union->pIdentifyPSources(fResultRelHead);
	
	if (fPSources.size() <= 1)
		this->pSetResult(iExpr);
	else
		this->pSetResult(fSource_Union->pGetProxy(fPSearch, fPSources, fResultRelHead, iExpr));
	}

void Source_Union::Analyze::Visit_Expr_Rel_Const(const ZRef<ZRA::Expr_Rel_Const>& iExpr)
	{
	ZRA::Visitor_Expr_Rel_Const::Visit_Expr_Rel_Const(iExpr);
	fResultRelHead |= iExpr->GetRelName();
	}

ZRef<ZRA::Expr_Rel> Source_Union::Analyze::TopLevelDo(ZRef<ZRA::Expr_Rel> iRel)
	{
	ZRef<ZRA::Expr_Rel> result = this->Do(iRel);
	if (fPSources.size() <= 1)
		return fSource_Union->pGetProxy(fPSearch, fPSources, fResultRelHead, result);
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::PSource

class Source_Union::DLink_PSource_NeedsWork
:	public DListLink<PSource, DLink_PSource_NeedsWork, kDebug>
	{};

class Source_Union::DLink_PSource_CollectFrom
:	public DListLink<PSource, DLink_PSource_CollectFrom, kDebug>
	{};

class Source_Union::PSource
:	public DLink_PSource_NeedsWork
,	public DLink_PSource_CollectFrom
	{
public:
	PSource(ZRef<Source> iSource, const string8& iPrefix);

	bool Intersects(const RelHead& iRelHead);
	ZRef<ZRA::Expr_Rel> UsableRel(ZRef<ZRA::Expr_Rel> iRel);

	ZRef<Source> fSource;
	const string8 fPrefix;

	int64 fNextRefcon;

	Map_Refcon_PIP fMap_Refcon_PIP;
	DListHead<DLink_PIP_NeedsWork> fPIP_NeedsWork;
	};

Source_Union::PSource::PSource(ZRef<Source> iSource, const string8& iPrefix)
:	fSource(iSource)
,	fPrefix(iPrefix)
,	fNextRefcon(1)
	{}

bool Source_Union::PSource::Intersects(const RelHead& iRelHead)
	{
	if (fPrefix.empty())
		{
		return fSource->Intersects(iRelHead);
		}
	else
		{
		if (ZRA::sHasPrefix(fPrefix, iRelHead))
			return fSource->Intersects(ZRA::sPrefixErase(fPrefix, iRelHead));
		}
	return false;
	}

ZRef<ZRA::Expr_Rel> Source_Union::PSource::UsableRel(ZRef<ZRA::Expr_Rel> iRel)
	{
	if (fPrefix.empty())
		return iRel;

	return InsertPrefix(fPrefix).Do(iRel);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union

Source_Union::Source_Union()
:	fEvent(Event::sZero())
,	fCallable_ResultsAvailable(MakeCallable(MakeWeakRef(this), &Source_Union::pResultsAvailable))
	{}

Source_Union::~Source_Union()
	{}

bool Source_Union::Intersects(const RelHead& iRelHead)
	{
	ZAcqMtxR acq(fMtxR);

	for (Map_Source_PSource::iterator
		iterSource = fMap_Source_PSource.begin(),
		endSource = fMap_Source_PSource.end();
		iterSource != endSource; ++iterSource)
		{
		if (iterSource->first->Intersects(iRelHead))
			return true;
		}

	return false;
	}

void Source_Union::ModifyRegistrations(
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZAcqMtxR acq(fMtxR);

	// -----

	// Remove any searches that need it
	while (iRemovedCount--)
		{
		const int64 theRefcon = *iRemoved++;

		Map_Refcon_ClientSearch::iterator iterClientSearch =
			fMap_Refcon_ClientSearch.find(theRefcon);
		
		ClientSearch* theClientSearch = &iterClientSearch->second;
		
		PSearch* thePSearch = theClientSearch->fPSearch;
		thePSearch->fClientSearches.Erase(theClientSearch);

		if (thePSearch->fClientSearches.Empty())
			{
			for (set<ZRef<Proxy> >::iterator
				iterProxies = thePSearch->fProxiesDependedUpon.begin(),
				endProxies = thePSearch->fProxiesDependedUpon.end();
				iterProxies != endProxies; ++iterProxies)
				{
				ZRef<Proxy> theProxy = *iterProxies;
				ZUtil_STL::sEraseMustContain(kDebug, theProxy->fDependentPSearches, thePSearch);
				if (theProxy->fDependentPSearches.empty())
					{
					for (DListEraser<PIP, DLink_PIP_InProxy>
						eraserPIP = theProxy->fPIP_InProxy; eraserPIP; eraserPIP.Advance())
						{
						PIP* thePIP = eraserPIP.Current();
						PSource* thePSource = thePIP->fPSource;
						thePSource->fPIP_NeedsWork.InsertIfNotContains(thePIP);
						fPSource_NeedsWork.InsertIfNotContains(thePSource);
						thePIP->fProxy = nullptr;
						}
					}
				}
			fPSearch_NeedsWork.EraseIfContains(thePSearch);
			ZUtil_STL::sEraseMustContain(kDebug, fMap_Rel_PSearch, thePSearch->fRel);
			}
		
		fMap_Refcon_ClientSearch.erase(iterClientSearch);
		}

	// -----

	// Add any searches
	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		ZRef<ZRA::Expr_Rel> theRel = iAdded->GetRel();

		pair<Map_Rel_PSearch::iterator,bool> inPSearch =
			fMap_Rel_PSearch.insert(make_pair(theRel, PSearch(theRel)));

		PSearch* thePSearch = &inPSearch.first->second;
		
		if (inPSearch.second)
			{
			if (ZLOGPF(s, eDebug))
				{
				s << "Raw:\n";
				ZRA::Util_Strim_Rel::sToStrim(theRel, s);
				}

			ZRA::Transform_Thing theTT;
			theRel = theTT.TopLevelDo(theRel);
			if (ZLOGF(s, eDebug))
				{
				s << "\nConcrete2Temp: " << theTT.fRename_Concrete2Temp;
				s << "\nTemp2Concrete: " << theTT.fRename_Temp2Concrete;
				s << "\nResultingRelHead: " << theTT.fResultingRelHead;
				}

#if 1
			theRel = ZRA::Transform_ConsolidateRenames().Do(theRel);

			if (ZLOGPF(s, eDebug))
				{
				s << "Thingified:\n";
				ZRA::Util_Strim_Rel::sToStrim(theRel, s);
				}
#endif

			thePSearch->fRel_Analyzed = Analyze(this, thePSearch).TopLevelDo(theRel);
			fPSearch_NeedsWork.Insert(thePSearch);//??

			if (ZLOGPF(s, eDebug))
				{
				s << "Analyzed:\n";
				ZRA::Util_Strim_Rel::sToStrim(thePSearch->fRel_Analyzed, s);
				}
			}

		const int64 theRefcon = iAdded->GetRefcon();

		Map_Refcon_ClientSearch::iterator iterClientSearch =
			fMap_Refcon_ClientSearch.insert(
			make_pair(theRefcon, ClientSearch(theRefcon, thePSearch))).first;

		thePSearch->fClientSearches.Insert(&iterClientSearch->second);
		}

	this->pInvokeCallable_ResultsAvailable();
	}

void Source_Union::CollectResults(vector<SearchResult>& oChanged)
	{
	ZAcqMtxR acq(fMtxR);

	// -----

	for (DListEraser<PSource, DLink_PSource_NeedsWork>
		eraserPSource = fPSource_NeedsWork; eraserPSource; eraserPSource.Advance())
		{
		PSource* thePSource = eraserPSource.Current();
		vector<AddedSearch> theAddedSearches;
		vector<int64> theRemoves;
		for (DListEraser<PIP, DLink_PIP_NeedsWork>
			eraserPIP = thePSource->fPIP_NeedsWork; eraserPIP; eraserPIP.Advance())
			{
			PIP* thePIP = eraserPIP.Current();
			if (thePIP->fProxy)
				{
				if (thePIP->fNeedsAdd)
					{
					thePIP->fNeedsAdd = false;
					theAddedSearches.push_back(
						AddedSearch(thePIP->fRefcon, thePSource->UsableRel(thePIP->fProxy->fRel)));
					}
				}
			else
				{
				if (!thePIP->fNeedsAdd)
					{
					theRemoves.push_back(thePIP->fRefcon);
					thePSource->fMap_Refcon_PIP.erase(thePIP->fRefcon);
					}
				}
			}
		
		thePSource->fSource->ModifyRegistrations(
			ZUtil_STL::sFirstOrNil(theAddedSearches), theAddedSearches.size(),
			ZUtil_STL::sFirstOrNil(theRemoves), theRemoves.size());
		}

	// -----

	for (DListEraser<PSource, DLink_PSource_CollectFrom>
		eraserPSource = fPSource_CollectFrom; eraserPSource; eraserPSource.Advance())
		{ this->pCollectFrom(eraserPSource.Current()); }

	// -----

	for (DListEraser<PSearch, DLink_PSearch_NeedsWork>
		eraserPSearch = fPSearch_NeedsWork; eraserPSearch; eraserPSearch.Advance())
		{
		PSearch* thePSearch = eraserPSearch.Current();
		bool allOK = true;
		ZRef<Event> theEvent;
		for (set<ZRef<Proxy> >::iterator iterProxy = thePSearch->fProxiesDependedUpon.begin();
			allOK && iterProxy != thePSearch->fProxiesDependedUpon.end(); ++iterProxy)
			{
			ZRef<Proxy> theProxy = *iterProxy;
			for (DListIterator<PIP, DLink_PIP_InProxy> iterPIP = theProxy->fPIP_InProxy;
				allOK && iterPIP; iterPIP.Advance())
				{
				PIP* thePIP = iterPIP.Current();
				if (!thePIP->fResult)
					{
					allOK = false;
					}
				else
					{
					if (theEvent)
						theEvent = theEvent->Joined(thePIP->fEvent);
					else
						theEvent = thePIP->fEvent;
					}
				}
			}

		if (allOK)
			{
			ZRef<ZQE::Walker> theWalker =
				Visitor_DoMakeWalker(this).Do(thePSearch->fRel_Analyzed);

			ZRef<ZQE::Result> theResult = sSearch(theWalker);
			
			for (DListIterator<ClientSearch, DLink_ClientSearch_InPSearch>
				iterCS = thePSearch->fClientSearches; iterCS; iterCS.Advance())
				{
				oChanged.push_back(SearchResult(iterCS.Current()->fRefcon, theResult, theEvent));
				}
			}
		}
	}

void Source_Union::InsertSource(ZRef<Source> iSource, const string8& iPrefix)
	{
	ZAcqMtxR acq(fMtxR);
	ZAssertStop(kDebug, fMap_Refcon_ClientSearch.empty());

	iSource->SetCallable_ResultsAvailable(fCallable_ResultsAvailable);

	ZUtil_STL::sInsertMustNotContain(kDebug, fMap_Source_PSource,
		iSource, PSource(iSource, iPrefix));
	}

void Source_Union::EraseSource(ZRef<Source> iSource)
	{
	ZAcqMtxR acq(fMtxR);
	Map_Source_PSource::iterator iterSource = fMap_Source_PSource.find(iSource);
	PSource& thePSource = iterSource->second;
	vector<int64> toRemove;
	for (Map_Refcon_PIP::iterator
		iterPIP = thePSource.fMap_Refcon_PIP.begin(),
		endPIP = thePSource.fMap_Refcon_PIP.end();
		iterPIP != endPIP; ++iterPIP)
		{
		toRemove.push_back(iterPIP->first);
		iterPIP->second.fProxy->fPIP_InProxy.Erase(&iterPIP->second);
		}
	thePSource.fMap_Refcon_PIP.clear();
	
	iSource->ModifyRegistrations(nullptr, 0,
		ZUtil_STL::sFirstOrNil(toRemove), toRemove.size());
			
	iSource->SetCallable_ResultsAvailable(null);

	fMap_Source_PSource.erase(iterSource);
	}

set<Source_Union::PSource*> Source_Union::pIdentifyPSources(const RelHead& iRelHead)
	{
	set<PSource*> result;
	for (Map_Source_PSource::iterator
		iterSource = fMap_Source_PSource.begin(),
		endSource = fMap_Source_PSource.end();
		iterSource != endSource; ++iterSource)
		{
		PSource* thePSource = &iterSource->second;
		if (thePSource->Intersects(iRelHead))
			result.insert(thePSource);
		}
	return result;
	}

ZRef<Source_Union::Proxy> Source_Union::pGetProxy(PSearch* iPSearch,
	const set<PSource*>& iPSources, const RelHead& iRelHead, ZRef<ZRA::Expr_Rel> iRel)
	{
	ZRef<Proxy> theProxyRef;
	if (ZQ<Proxy*> theQ = ZUtil_STL::sGetIfContains(fProxyMap, iRel))
		{
		theProxyRef = theQ.Get();
		ZAssert(iRelHead == theProxyRef->fResultRelHead);
		}
	else
		{
		Proxy* theProxy = new Proxy(this);
		theProxyRef = theProxy;
		theProxy->fRel = iRel;
		theProxy->fResultRelHead = iRelHead;
		ZUtil_STL::sInsertMustNotContain(kDebug, fProxyMap, iRel, theProxy);
		for (set<PSource*>::const_iterator
			iterPSources = iPSources.begin(), endPSources = iPSources.end();
			iterPSources != endPSources; ++iterPSources)
			{
			PSource* thePSource = *iterPSources;

			const int64 theRefcon = thePSource->fNextRefcon++;

			PIP* thePIP = &thePSource->fMap_Refcon_PIP.insert(
				make_pair(theRefcon, PIP())).first->second;

			thePIP->fNeedsAdd = true;
			thePIP->fRefcon = theRefcon;
			thePIP->fProxy = theProxy;
			thePIP->fPSource = thePSource;
			theProxy->fPIP_InProxy.Insert(thePIP);

			thePSource->fPIP_NeedsWork.Insert(thePIP);
			fPSource_NeedsWork.InsertIfNotContains(thePSource);
			}
		}

	theProxyRef->fDependentPSearches.insert(iPSearch);
	iPSearch->fProxiesDependedUpon.insert(theProxyRef);

	return theProxyRef;
	}

void Source_Union::pFinalizeProxy(Proxy* iProxy)
	{
	if (!iProxy->FinishFinalize())
		return;

	ZAssertStop(kDebug, iProxy->fPIP_InProxy.Empty());

	ZUtil_STL::sEraseMustContain(kDebug, fProxyMap, iProxy->fRel);

	delete iProxy;
	}

ZRef<Source_Union::Walker_Proxy> Source_Union::pMakeWalker(ZRef<Proxy> iProxy)
	{ return new Walker_Proxy(this, iProxy); }

void Source_Union::pRewind(ZRef<Walker_Proxy> iWalker)
	{
	iWalker->fIter_PIP = iWalker->fProxy->fPIP_InProxy;
	iWalker->fCurrentResult.Clear();
	}

void Source_Union::pPrime(ZRef<Walker_Proxy> iWalker,
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	iWalker->fBaseOffset = ioBaseOffset;
	const RelHead& theRelHead = iWalker->fProxy->fResultRelHead;
	for (RelHead::const_iterator i = theRelHead.begin(); i != theRelHead.end(); ++i)
		oOffsets[*i] = ioBaseOffset++;
	}

bool Source_Union::pReadInc(ZRef<Walker_Proxy> iWalker,
	ZVal_Any* ioResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	for (;;)
		{
		if (!iWalker->fCurrentResult)
			{
			if (!iWalker->fIter_PIP)
				return false;
			iWalker->fCurrentIndex = 0;
			iWalker->fCurrentResult = iWalker->fIter_PIP.Current()->fResult;
			}

		ZAssertStop(kDebug, iWalker->fCurrentResult);

		if (iWalker->fCurrentIndex >= iWalker->fCurrentResult->Count())
			{
			iWalker->fCurrentResult.Clear();
			iWalker->fIter_PIP.Advance();
			continue;
			}

		if (oAnnotations)
			iWalker->fCurrentResult->GetAnnotationsAt(iWalker->fCurrentIndex, *oAnnotations);

		const ZRA::RelHead& theRH = iWalker->fCurrentResult->GetRelHead();
		ZAssert(theRH == iWalker->fProxy->fResultRelHead);
		size_t theOffset = iWalker->fBaseOffset;
		const ZVal_Any* theVals = iWalker->fCurrentResult->GetValsAt(iWalker->fCurrentIndex);
		for (ZRA::RelHead::const_iterator i = theRH.begin(); i != theRH.end(); ++i)
			ioResults[theOffset++] = *theVals++;
		++iWalker->fCurrentIndex;
		return true;
		}
	}

void Source_Union::pCollectFrom(PSource* iPSource)
	{
	vector<SearchResult> theSearchResults;
	iPSource->fSource->CollectResults(theSearchResults);
	
	for (vector<SearchResult>::iterator iterSearchResults = theSearchResults.begin();
		iterSearchResults != theSearchResults.end(); ++iterSearchResults)
		{
		const int64 theRefcon = iterSearchResults->GetRefcon();
		Map_Refcon_PIP::iterator iter = iPSource->fMap_Refcon_PIP.find(theRefcon);
		if (iPSource->fMap_Refcon_PIP.end() != iter)
			{
			PIP* thePIP = &iter->second;
			thePIP->fResult = iterSearchResults->GetResult();
			thePIP->fEvent = iterSearchResults->GetEvent();
			if (!thePIP->fEvent)
				thePIP->fEvent = Event::sZero();//##
			for (set<PSearch*>::iterator
				i = thePIP->fProxy->fDependentPSearches.begin(),
				end = thePIP->fProxy->fDependentPSearches.end();
				i != end; ++i)
				{ fPSearch_NeedsWork.InsertIfNotContains(*i); }
			}
		}
	}

void Source_Union::pResultsAvailable(ZRef<Source> iSource)
	{
	Map_Source_PSource::iterator iterSource = fMap_Source_PSource.find(iSource);
	fPSource_CollectFrom.InsertIfNotContains(&iterSource->second);
	this->pInvokeCallable_ResultsAvailable();
	}

} // namespace ZDataspace
} // namespace ZooLib
