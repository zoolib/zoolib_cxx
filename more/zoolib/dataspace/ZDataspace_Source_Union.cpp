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
#include "zoolib/ZExpr_Bool_ValPred_Any.h"
#include "zoolib/Zlog.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZVisitor_Expr_Op_DoTransform_T.h"

#include "zoolib/dataspace/ZDataspace_Source_Union.h"
#include "zoolib/dataspace/ZDataspace_Util_Strim.h"

#include "zoolib/zqe/ZQE_Search.h"
#include "zoolib/zqe/ZQE_Visitor_DoMakeWalker.h"

#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict_Any.h"
#include "zoolib/zra/ZRA_Expr_Rel_Select.h"
#include "zoolib/zra/ZRA_Util_RelCompare.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

namespace ZooLib {
namespace ZDataspace {

using std::map;
using std::pair;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers (anonymous)

namespace { // anonymous

set<string8> spRename(const map<string8,string8>& iRename, const set<string8>& iSet)
	{
	if (iRename.empty())
		return iSet;

	set<string8> result;
	for (set<string8>::const_iterator i = iSet.begin(); i != iSet.end(); ++i)
		{
		map<string8,string8>::const_iterator iter = iRename.find(*i);
		if (iRename.end() == iter)
			result.insert(*i);
		else
			result.insert(iter->second);
		}
	return result;
	}

RelHead spPrefix(const string8& iPrefix, const RelHead& iRelHead)
	{
	if (iPrefix.empty())
		return iRelHead;
	RelHead result;
	for (RelHead::const_iterator i = iRelHead.begin(); i != iRelHead.end(); ++i)
		result |= iPrefix + *i;
	return result;
	}

set<RelHead> spPrefix(const string8& iPrefix, const set<RelHead>& iRelHeads)
	{
	set<RelHead> result;
	for (set<RelHead>::const_iterator i = iRelHeads.begin(); i != iRelHeads.end(); ++i)
		result.insert(spPrefix(iPrefix, *i));
	return result;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Proxy declaration

class Source_Union::Proxy
:	public virtual ZRA::Expr_Rel
,	public virtual ZExpr_Op0_T<ZRA::Expr_Rel>
	{
	typedef ZExpr_Op0_T<Expr_Rel> inherited;
public:
// From ZExpr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZRef<ZRA::Expr_Rel> Self();
	virtual ZRef<ZRA::Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Proxy(Visitor_Proxy& iVisitor);

	RelHead fRelHead;
	ZRef<ZRA::Expr_Rel> fRel;
	set<PSearch*> fDependentPSearches;

	// Something that ties this proxy to each Source.
	set<ProxyInPSource*> fProxyInPSources;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Visitor_Proxy

class Source_Union::Visitor_Proxy
:	public virtual ZVisitor_Expr_Op0_T<ZRA::Expr_Rel>
	{
	typedef ZVisitor_Expr_Op0_T<ZRA::Expr_Rel> inherited;
public:
	virtual void Visit_Proxy(ZRef<Proxy> iExpr)
		{ this->Visit_Expr_Op0(iExpr); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Proxy definition

void Source_Union::Proxy::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZRA::Expr_Rel>& iVisitor)
	{
	if (Visitor_Proxy* theVisitor =
		dynamic_cast<Visitor_Proxy*>(&iVisitor))
		{
		this->Accept_Proxy(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op0(iVisitor);
		}
	}

ZRef<ZRA::Expr_Rel> Source_Union::Proxy::Self()
	{ return this; }

ZRef<ZRA::Expr_Rel> Source_Union::Proxy::Clone()
	{ return this; }

void Source_Union::Proxy::Accept_Proxy(Visitor_Proxy& iVisitor)
	{ iVisitor.Visit_Proxy(this); }

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Comparator_Proxy

bool Source_Union::Comparator_Proxy::operator()(
	const ZRef<Proxy>& iLeft, const ZRef<Proxy>& iRight) const
	{
	if (iLeft->fRelHead < iRight->fRelHead)
		return true;
	if (iRight->fRelHead < iLeft->fRelHead)
		return false;

	return ZRA::Util_RelCompare::sCompare(iLeft->fRel, iRight->fRel) < 0;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Walker_Proxy

class Source_Union::Walker_Proxy : public ZQE::Walker
	{
public:
	Walker_Proxy(ZRef<Source_Union> iSource, ZRef<Proxy> iProxy);
	virtual ~Walker_Proxy();

	virtual void Rewind();

	virtual void Prime(const map<string8,size_t>& iBindingOffsets, 
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool ReadInc(const ZVal_Any* iBindings,
		ZVal_Any* oResults,
		set<ZRef<ZCounted> >* oAnnotations);

	ZRef<Source_Union> fSource;
	ZRef<Proxy> fProxy;
	size_t fBaseOffset;

	set<ProxyInPSource*>::const_iterator fIter_ProxyInPSource;
	ZRef<ZQE::Result> fCurrentResult;
	size_t fCurrentIndex;
	};

Source_Union::Walker_Proxy::Walker_Proxy(ZRef<Source_Union> iSource, ZRef<Proxy> iProxy)
:	fSource(iSource)
,	fProxy(iProxy)
	{}

Source_Union::Walker_Proxy::~Walker_Proxy()
	{}

void Source_Union::Walker_Proxy::Rewind()
	{ fSource->pRewind(this); }

void Source_Union::Walker_Proxy::Prime(const map<string8,size_t>& iBindingOffsets, 
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{ fSource->pPrime(this, iBindingOffsets, oOffsets, ioBaseOffset); }

bool Source_Union::Walker_Proxy::ReadInc(const ZVal_Any* iBindings,
	ZVal_Any* oResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{ return fSource->pReadInc(this, iBindings, oResults, oAnnotations); }

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::PSearch

class Source_Union::PSearch
	{
public:
	PSearch(int64 iRefcon, ZRef<ZRA::Expr_Rel> iRel);

	int64 fRefcon;
	ZRef<ZRA::Expr_Rel> fRel;
	ZRef<ZRA::Expr_Rel> fRel_Analyzed;
	set<ZRef<Proxy> > fProxiesDependedUpon;
	ZRef<ZQE::Result> fResult;
	};

Source_Union::PSearch::PSearch(int64 iRefcon, ZRef<ZRA::Expr_Rel> iRel)
:	fRefcon(iRefcon)
,	fRel(iRel)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::ProxyInPSource

class Source_Union::ProxyInPSource
	{
public:
	Proxy* fProxy;
	PSource* fPSource;
	int64 fRefcon;
	ZRef<ZQE::Result> fResult;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeWalker

class Source_Union::Visitor_DoMakeWalker
:	public virtual ZQE::Visitor_DoMakeWalker
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Proxy
	{
public:
	Visitor_DoMakeWalker(ZRef<Source_Union> iSource)
	:	fSource(iSource)
		{}

	virtual void Visit_Expr_Rel_Concrete(ZRef<ZRA::Expr_Rel_Concrete> iExpr)
		{
		// We should never see a Concrete as we're being used to make walkers
		// for an analyzed rel.
		ZUnimplemented();
		}

	virtual void Visit_Proxy(ZRef<Proxy> iExpr)
		{ this->pSetResult(fSource->pMakeWalker(iExpr)); }

	ZRef<Source_Union> fSource;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Analyze

class Source_Union::Analyze
:	public virtual ZVisitor_Expr_Op_DoTransform_T<ZRA::Expr_Rel>
,	public virtual ZRA::Visitor_Expr_Rel_Project
,	public virtual ZRA::Visitor_Expr_Rel_Rename
,	public virtual ZRA::Visitor_Expr_Rel_Restrict_Any
,	public virtual ZRA::Visitor_Expr_Rel_Select
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
	{
	typedef ZVisitor_Expr_Op_DoTransform_T<ZRA::Expr_Rel> inherited;
public:
	Analyze(Source_Union* iSource_Union, PSearch* iPSearch);

// From ZVisitor_Expr_Op2_T via ZVisitor_Expr_Op_DoTransform_T
	virtual void Visit_Expr_Op2(ZRef<ZExpr_Op2_T<ZRA::Expr_Rel> > iExpr);

// From ZRA::Visitor_Expr_Rel_XXX
	virtual void Visit_Expr_Rel_Project(ZRef<ZRA::Expr_Rel_Project> iExpr);
	virtual void Visit_Expr_Rel_Rename(ZRef<ZRA::Expr_Rel_Rename> iExpr);
	virtual void Visit_Expr_Rel_Restrict(ZRef<ZRA::Expr_Rel_Restrict_Any> iExpr);
	virtual void Visit_Expr_Rel_Select(ZRef<ZRA::Expr_Rel_Select> iExpr);
	virtual void Visit_Expr_Rel_Concrete(ZRef<ZRA::Expr_Rel_Concrete> iExpr);

// Our protocol
	ZRef<ZRA::Expr_Rel> TopLevelDo(ZRef<ZRA::Expr_Rel> iRel);

	Source_Union* fSource_Union;
	PSearch* fPSearch;
	map<string8,string8> fRename;
	ZRA::RelHead fRelHead;
	};

Source_Union::Analyze::Analyze(Source_Union* iSource_Union, PSearch* iPSearch)
:	fSource_Union(iSource_Union)
,	fPSearch(iPSearch)
	{}

void Source_Union::Analyze::Visit_Expr_Op2(ZRef<ZExpr_Op2_T<ZRA::Expr_Rel> > iExpr)
	{
	// Visit left branch
	const ZRef<ZRA::Expr_Rel> oldOp0 = iExpr->GetOp0();
	const ZRef<ZRA::Expr_Rel> newOp0 = this->Do(oldOp0);
	const RelHead leftRelHead = fRelHead;

	fRelHead.clear();

	// Visit right branch
	const ZRef<ZRA::Expr_Rel> oldOp1 = iExpr->GetOp1();
	const ZRef<ZRA::Expr_Rel> newOp1 = this->Do(oldOp1);
	const RelHead rightRelHead = fRelHead;

	fRelHead |= leftRelHead;
	
	const bool leftIsSimple = fSource_Union->pIsSimple(leftRelHead);
	const bool rightIsSimple = fSource_Union->pIsSimple(rightRelHead);

	if (ZLOGF(s, eDebug+2))
		{
		ZRA::Util_Strim_RelHead::sWrite_RelHead(leftRelHead,s);
		ZRA::Util_Strim_RelHead::sWrite_RelHead(rightRelHead,s);
		ZRA::Util_Strim_RelHead::sWrite_RelHead(fRelHead,s);
		}

	if (leftIsSimple)
		{
		// Our left branch is simple, it references zero or one source.
		if (fSource_Union->pIsSimple(fRelHead))
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
			// we *now* reference multiple sources. We register the left
			// branch as a standalone search against the singular source in priorSource
			ZRef<Proxy> proxy0 = fSource_Union->pGetProxy(fPSearch, leftRelHead, newOp0);

			if (rightIsSimple)
				{
				// Right branch is simple, and thus won't have registered a proxy yet.
				ZRef<Proxy> proxy1 = fSource_Union->pGetProxy(fPSearch, rightRelHead, newOp1);
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
		if (rightIsSimple)
			{
			// Right branch is simple, and thus won't have registered a proxy yet.
			ZRef<Proxy> proxy1 = fSource_Union->pGetProxy(fPSearch, rightRelHead, newOp1);
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

void Source_Union::Analyze::Visit_Expr_Rel_Project(ZRef<ZRA::Expr_Rel_Project> iExpr)
	{
	ZRA::Visitor_Expr_Rel_Project::Visit_Expr_Rel_Project(iExpr);

	fRelHead &= spRename(fRename, iExpr->GetProjectRelHead());
	}

void Source_Union::Analyze::Visit_Expr_Rel_Rename(ZRef<ZRA::Expr_Rel_Rename> iExpr)
	{
	ZRA::Visitor_Expr_Rel_Rename::Visit_Expr_Rel_Rename(iExpr);
	
	string8 theOld = iExpr->GetOld();
	const string8 theNew = iExpr->GetNew();
	if (ZQ<string8> theQ = ZUtil_STL::sEraseAndReturnIfContains(fRename, theOld))
		theOld = theQ.Get();
	ZUtil_STL::sInsertMustNotContain(kDebug, fRename, theNew, theOld);
	}

void Source_Union::Analyze::Visit_Expr_Rel_Restrict(ZRef<ZRA::Expr_Rel_Restrict_Any> iExpr)
	{
	ZRA::Visitor_Expr_Rel_Restrict_Any::Visit_Expr_Rel_Restrict(iExpr);

	fRelHead |= spRename(fRename, iExpr->GetValPred().GetNames());
	}

void Source_Union::Analyze::Visit_Expr_Rel_Select(ZRef<ZRA::Expr_Rel_Select> iExpr)
	{
	ZRA::Visitor_Expr_Rel_Select::Visit_Expr_Rel_Select(iExpr);

	fRelHead |= spRename(fRename, sGetNames(iExpr->GetExpr_Bool()));
	}

void Source_Union::Analyze::Visit_Expr_Rel_Concrete(ZRef<ZRA::Expr_Rel_Concrete> iExpr)
	{
	const ZRA::RelHead theRelHead = iExpr->GetConcreteRelHead();
	ZAssert(fRelHead.empty());

	fRelHead |= iExpr->GetConcreteRelHead();

	if (fSource_Union->pIsSimple(theRelHead))
		this->pSetResult(iExpr);
	else
		this->pSetResult(fSource_Union->pGetProxy(fPSearch, theRelHead, iExpr));
	}

ZRef<ZRA::Expr_Rel> Source_Union::Analyze::TopLevelDo(ZRef<ZRA::Expr_Rel> iRel)
	{
	ZRef<ZRA::Expr_Rel> result = this->Do(iRel);
	if (fSource_Union->pIsSimple(fRelHead))
		return fSource_Union->pGetProxy(fPSearch, fRelHead, result);
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * InsertPrefix (anonymous)

namespace { // anonymous

class InsertPrefix
:	public virtual ZVisitor_Expr_Op_DoTransform_T<ZRA::Expr_Rel>
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
	{
	typedef ZVisitor_Expr_Op_DoTransform_T<ZRA::Expr_Rel> inherited;
public:
	InsertPrefix(const string8& iPrefix);

// From ZRA::Visitor_Expr_Rel_Concrete
	virtual void Visit_Expr_Rel_Concrete(ZRef<ZRA::Expr_Rel_Concrete> iExpr);

	string8 fPrefix;
	};

InsertPrefix::InsertPrefix(const string8& iPrefix)
:	fPrefix(iPrefix)
	{}

void InsertPrefix::Visit_Expr_Rel_Concrete(ZRef<ZRA::Expr_Rel_Concrete> iExpr)
	{
	const RelHead& theRelHead = iExpr->GetConcreteRelHead();
	
	const size_t prefixLength = fPrefix.length();
	RelHead newRelHead;
	for (RelHead::const_iterator i = theRelHead.begin(); i != theRelHead.end(); ++i)
		{
		const string8& oldName = *i;
		if (oldName.substr(0, prefixLength) == fPrefix)
			{
			newRelHead |= oldName.substr(prefixLength);
			}
		else
			{
			ZUnimplemented();
			newRelHead |= oldName;
			}
		}

	ZRef<ZRA::Expr_Rel> theRel = ZRA::sConcrete(newRelHead);
	for (RelHead::const_iterator i = newRelHead.begin(); i != newRelHead.end(); ++i)
		theRel = sRename(theRel, fPrefix + *i, *i);
	
	this->pSetResult(theRel);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::PSource

class Source_Union::PSource
	{
public:
	PSource(ZRef<Source> iSource, const string8& iPrefix);

	bool Intersects(const RelHead& iRelHead);
	ZRef<ZRA::Expr_Rel> UsableRel(ZRef<ZRA::Expr_Rel> iRel);

	ZRef<Source> fSource;
	const string8 fPrefix;

	int64 fNextRefcon;

	map<int64, ProxyInPSource*> fMap_RefconToProxyInPSource;
	set<ProxyInPSource*> fAdds;
	vector<int64>  fRemoves;
	};

Source_Union::PSource::PSource(ZRef<Source> iSource, const string8& iPrefix)
:	fSource(iSource)
,	fPrefix(iPrefix)
,	fNextRefcon(1)
	{}

bool Source_Union::PSource::Intersects(const RelHead& iRelHead)
	{
	const RelHead& sourceRelHead = fSource->GetRelHead();
	if (fPrefix.empty())
		{
		if (sourceRelHead.empty() || !(iRelHead & sourceRelHead).empty())
			return true;
		}
	else
		{
		const size_t prefixLength = fPrefix.length();
		for (RelHead::const_iterator i = iRelHead.begin(); i != iRelHead.end(); ++i)
			{
			if (i->substr(0, prefixLength) == fPrefix)
				{
				if (sourceRelHead.empty() || ZUtil_STL::sContains(sourceRelHead, i->substr(prefixLength)))
					return true;
				}
			}
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
	{
	}

set<RelHead> Source_Union::GetRelHeads()
	{
	ZAcqMtxR acq(fMtxR);

	set<RelHead> result;
	for (map<ZRef<Source>, PSource*>::iterator i = fMap_SourceToPSource.begin();
		i != fMap_SourceToPSource.end(); ++i)
		{
		set<RelHead> sourceRelHeads = i->first->GetRelHeads();
		if (sourceRelHeads.empty())
			return sourceRelHeads;
		result.insert(sourceRelHeads.begin(), sourceRelHeads.end());
		}
	return result;
	}

void Source_Union::ModifyRegistrations(
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZAcqMtxR acq(fMtxR);

	// Remove any sources that need removing.
	for (set<PSource*>::iterator iterPSource = fPSources_ToRemove.begin();
		iterPSource != fPSources_ToRemove.end(); ++iterPSource)
		{
		// For each ProxyInPSource, unregister it, remove it from owning Proxy
		// and delete it.
		PSource* thePSource = *iterPSource;
		vector<int64> toRemove;
		for (map<int64, ProxyInPSource*>::iterator
			iterPIP = thePSource->fMap_RefconToProxyInPSource.begin();
			iterPIP != thePSource->fMap_RefconToProxyInPSource.end(); ++iterPIP)
			{
			ProxyInPSource* thePIP = iterPIP->second;
			toRemove.push_back(iterPIP->first);
			thePIP->fProxy->fProxyInPSources.erase(thePIP);
			fProxiesThatNeedWork.insert(thePIP->fProxy);
			delete thePIP;
			}
		thePSource->fMap_RefconToProxyInPSource.clear();
		
		ZRef<Source> theSource = thePSource->fSource;
		theSource->ModifyRegistrations(nullptr, 0,
			ZUtil_STL::sFirstOrNil(toRemove), toRemove.size());
				
		theSource->SetCallable_ResultsAvailable(null);
		ZUtil_STL::sEraseMustContain(kDebug, fMap_SourceToPSource, theSource);
		delete thePSource;
		}
	fPSources_ToRemove.clear();

	// -----

	// Remove any searches that need it
	while (iRemovedCount--)
		{
		const int64 theRefcon = *iRemoved++;
		// Unstitch from sources.
		PSearch* thePSearch =
			ZUtil_STL::sEraseAndReturn(kDebug, fMap_RefconToPSearch, theRefcon);
		
		for (set<ZRef<Proxy> >::iterator iterProxies = thePSearch->fProxiesDependedUpon.begin();
			iterProxies != thePSearch->fProxiesDependedUpon.end(); ++iterProxies)
			{
			ZRef<Proxy> theProxy = *iterProxies;
			ZUtil_STL::sEraseMustContain(kDebug, theProxy->fDependentPSearches, thePSearch);
			fProxiesThatNeedWork.insert(theProxy);
			}
		delete thePSearch;
		}

	// -----

	// Add any sources that need it.
	for (map<ZRef<Source>,string8>::iterator iterSources = fSources_ToAdd.begin();
		iterSources != fSources_ToAdd.end(); ++iterSources)
		{
		ZRef<Source> theSource = iterSources->first;
		theSource->SetCallable_ResultsAvailable(fCallable_ResultsAvailable);
		PSource* thePSource = new PSource(theSource, iterSources->second);
		ZUtil_STL::sInsertMustNotContain(kDebug, fMap_SourceToPSource, theSource, thePSource);
		// Punt for now -- we will need to determine which Proxies must be reorganized
		// as a result of this source being added. Probably best to do it en masse at some point.
		// Alternatively, do a re-analysis of all PSearches, and clean up any Proxies that
		// dangle.
		}
	fSources_ToAdd.clear();

	// -----

	// Add any searches
	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		const int64 theRefcon = iAdded->GetRefcon();
		PSearch* thePSearch = new PSearch(theRefcon, iAdded->GetRel());
		ZUtil_STL::sInsertMustNotContain(kDebug, fMap_RefconToPSearch, theRefcon, thePSearch);

		thePSearch->fRel_Analyzed = Analyze(this, thePSearch).TopLevelDo(thePSearch->fRel);

		if (ZLOGF(s, eDebug))
			{
			s << "Raw, then analyzed:\n";
			ZRA::Util_Strim_Rel::sToStrim(thePSearch->fRel, s);
			s << "\n\n";
			ZRA::Util_Strim_Rel::sToStrim(thePSearch->fRel_Analyzed, s);
			}
		}
	}

void Source_Union::CollectResults(vector<SearchResult>& oChanged)
	{
	ZAcqMtxR acq(fMtxR);

	// Do work on any proxies that need it.
	for (set<ZRef<Proxy> >::iterator iterProxies = fProxiesThatNeedWork.begin();
		iterProxies != fProxiesThatNeedWork.end(); ++iterProxies)
		{
		ZRef<Proxy> theProxy = *iterProxies;
		if (theProxy->fDependentPSearches.empty())
			{
			// We can unregister the PIPs from their PSources.
			for (set<ProxyInPSource*>::iterator iterPIP = theProxy->fProxyInPSources.begin();
				iterPIP != theProxy->fProxyInPSources.end(); ++iterPIP)
				{
				ProxyInPSource* thePIP = *iterPIP;
				PSource* thePSource = thePIP->fPSource;
				ZUtil_STL::sEraseMustContain(
					kDebug, thePSource->fMap_RefconToProxyInPSource, thePIP->fRefcon);
				thePSource->fRemoves.push_back(thePIP->fRefcon);
				delete thePIP;
				}
			theProxy->fProxyInPSources.clear();
			}
		else
			{
			if (theProxy->fProxyInPSources.empty())
				{
				// need to register PIPs for each PSource that's relevant.
				for (map<ZRef<Source>, PSource*>::iterator
					iterSources = fMap_SourceToPSource.begin();
					iterSources != fMap_SourceToPSource.end(); ++iterSources)
					{
					PSource* thePSource = iterSources->second;
					if (thePSource->Intersects(theProxy->fRelHead))
						{
						ProxyInPSource* aPIP = new ProxyInPSource;
						aPIP->fPSource = thePSource;
						aPIP->fProxy = theProxy.Get();
						ZUtil_STL::sInsertMustNotContain(kDebug, theProxy->fProxyInPSources, aPIP);
						aPIP->fRefcon = thePSource->fNextRefcon++;
						ZUtil_STL::sInsertMustNotContain(
							kDebug, thePSource->fMap_RefconToProxyInPSource, aPIP->fRefcon, aPIP);
						ZUtil_STL::sInsertMustNotContain(kDebug, thePSource->fAdds, aPIP);
						fPSourcesThatNeedWork.insert(thePSource);
						}
					}
				}
			}
		}
	fProxiesThatNeedWork.clear();

	// -----

	for (set<PSource*>::iterator iterPSources = fPSourcesThatNeedWork.begin();
		iterPSources != fPSourcesThatNeedWork.end(); ++iterPSources)
		{
		PSource* thePSource = *iterPSources;
		vector<AddedSearch> theAddedSearches;
		for (set<ProxyInPSource*>::iterator iterPIP = thePSource->fAdds.begin();
			iterPIP != thePSource->fAdds.end(); ++iterPIP)
			{
			ProxyInPSource* thePIP = *iterPIP;

			theAddedSearches.push_back(
				AddedSearch(thePIP->fRefcon, thePSource->UsableRel(thePIP->fProxy->fRel)));
			}
		thePSource->fAdds.clear();
		
		thePSource->fSource->ModifyRegistrations(
			ZUtil_STL::sFirstOrNil(theAddedSearches), theAddedSearches.size(),
			ZUtil_STL::sFirstOrNil(thePSource->fRemoves), thePSource->fRemoves.size());
		thePSource->fRemoves.clear();
		}
	fPSourcesThatNeedWork.clear();


	// -----

	for (set<PSource*>::iterator iterPSources = fPSources_ToCollectFrom.begin();
		iterPSources != fPSources_ToCollectFrom.end(); ++iterPSources)
		{
		this->pCollectFrom(*iterPSources);
		}
	fPSources_ToCollectFrom.clear();

	// -----

	for (set<PSearch*>::iterator iterPSearch = fPSearchesThatNeedWork.begin();
		iterPSearch != fPSearchesThatNeedWork.end(); ++iterPSearch)
		{
		PSearch* thePSearch = *iterPSearch;
		bool allOK = true;
		for (set<ZRef<Proxy> >::iterator iterProxy = thePSearch->fProxiesDependedUpon.begin();
			allOK && iterProxy != thePSearch->fProxiesDependedUpon.end(); ++iterProxy)
			{
			ZRef<Proxy> theProxy = *iterProxy;
			for (set<ProxyInPSource*>::iterator iterPIP = theProxy->fProxyInPSources.begin();
				allOK && iterPIP != theProxy->fProxyInPSources.end(); ++iterPIP)
				{
				if (!(*iterPIP)->fResult)
					allOK = false;
				}
			}

		if (!allOK)
			{
			++iterPSearch;
			continue;
			}

		ZRef<ZQE::Walker> theWalker =
			Visitor_DoMakeWalker(this).Do(thePSearch->fRel_Analyzed);
		
		// We'll need to build an event that's based off the events in depended-upon
		// ProxyInPSource instances
		SearchResult theSearchResult(thePSearch->fRefcon, sSearch(theWalker), null);
		oChanged.push_back(theSearchResult);
		}
	fPSearchesThatNeedWork.clear();
	}

void Source_Union::InsertSource(ZRef<Source> iSource, const string8& iPrefix)
	{
	ZAcqMtxR acq(fMtxR);

	ZAssert(iSource);
	ZAssert(! ZUtil_STL::sContains(fMap_SourceToPSource, iSource));
	ZUtil_STL::sInsertMustNotContain(kDebug, fSources_ToAdd, iSource, iPrefix);
	}

void Source_Union::EraseSource(ZRef<Source> iSource)
	{
	ZAcqMtxR acq(fMtxR);

	PSource* thePSource = ZUtil_STL::sGetMustContain(kDebug, fMap_SourceToPSource, iSource);
	ZUtil_STL::sInsertMustNotContain(kDebug, fPSources_ToRemove, thePSource);
	}


ZRef<ZQE::Walker> Source_Union::pMakeWalker(ZRef<Proxy> iProxy)
	{ return new Walker_Proxy(this, iProxy); }

void Source_Union::pRewind(ZRef<Walker_Proxy> iWalker)
	{
	iWalker->fIter_ProxyInPSource = iWalker->fProxy->fProxyInPSources.begin();
	iWalker->fCurrentResult.Clear();
	}

void Source_Union::pPrime(ZRef<Walker_Proxy> iWalker,
	const map<string8,size_t>& iBindingOffsets, 
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	iWalker->fIter_ProxyInPSource = iWalker->fProxy->fProxyInPSources.begin();
	iWalker->fBaseOffset = ioBaseOffset;
	const RelHead& theRelHead = iWalker->fProxy->fRelHead;
	for (RelHead::const_iterator i = theRelHead.begin(); i != theRelHead.end(); ++i)
		oOffsets[*i] = ioBaseOffset++;
	}

bool Source_Union::pReadInc(ZRef<Walker_Proxy> iWalker,
	const ZVal_Any* iBindings,
	ZVal_Any* oResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	for (;;)
		{
		if (!iWalker->fCurrentResult)
			{
			if (iWalker->fProxy->fProxyInPSources.end() == iWalker->fIter_ProxyInPSource)
				return false;
			iWalker->fCurrentIndex = 0;
			iWalker->fCurrentResult = (*iWalker->fIter_ProxyInPSource)->fResult;
			}

		ZAssert(iWalker->fCurrentResult);

		if (iWalker->fCurrentIndex >= iWalker->fCurrentResult->Count())
			{
			iWalker->fCurrentResult.Clear();
			++iWalker->fIter_ProxyInPSource;
			continue;
			}

		if (oAnnotations)
			iWalker->fCurrentResult->GetAnnotationsAt(iWalker->fCurrentIndex, *oAnnotations);

		const ZRA::RelHead& theRH = iWalker->fCurrentResult->GetRelHead();
		size_t theOffset = iWalker->fBaseOffset;
		const ZVal_Any* theVals = iWalker->fCurrentResult->GetValsAt(iWalker->fCurrentIndex);
		for (ZRA::RelHead::const_iterator i = theRH.begin(); i != theRH.end(); ++i)
			oResults[theOffset++] = *theVals++;
		++iWalker->fCurrentIndex;
		return true;
		}
	}

ZRef<Source_Union::Proxy> Source_Union::pGetProxy(
	PSearch* iPSearch, const RelHead& iRelHead, ZRef<ZRA::Expr_Rel> iRel)
	{
	ZRef<Proxy> theProxy = new Proxy;
	theProxy->fRelHead = iRelHead;
	theProxy->fRel = iRel;
	ProxySet::iterator i = fProxies.find(theProxy);
	if (fProxies.end() == i)
		{
		fProxies.insert(theProxy);
		fProxiesThatNeedWork.insert(theProxy);
		}
	else
		{
		theProxy = *i;
		}

	// It's OK for a search to depend on a proxy multiple times.
	theProxy->fDependentPSearches.insert(iPSearch);
	iPSearch->fProxiesDependedUpon.insert(theProxy);

	return theProxy;
	}

bool Source_Union::pIsSimple(const RelHead& iRelHead)
	{
	// iRelHead is simple if it references columns from zero or one sources
	bool intersectsASource = false;
	for (map<ZRef<Source>, PSource*>::iterator
		i = fMap_SourceToPSource.begin(), end = fMap_SourceToPSource.end(); i != end; ++i)
		{
		PSource* thePSource = i->second;
		if (thePSource->Intersects(iRelHead))
			{
			if (intersectsASource)
				return false;
			intersectsASource = true;
			}
		}
	return true;
	}

void Source_Union::pCollectFrom(PSource* iPSource)
	{
	vector<SearchResult> theSearchResults;
	iPSource->fSource->CollectResults(theSearchResults);
	
	for (vector<SearchResult>::iterator iterSearchResults = theSearchResults.begin();
		iterSearchResults != theSearchResults.end(); ++iterSearchResults)
		{
		const int64 theRefcon = iterSearchResults->GetRefcon();
		ZRef<ZQE::Result> theResult = iterSearchResults->GetResult();

		if (ZQ<ProxyInPSource*> theQ =
			ZUtil_STL::sGetIfContains(iPSource->fMap_RefconToProxyInPSource, theRefcon))
			{
			ProxyInPSource* thePIP = theQ.Get();
			thePIP->fResult = theResult;
			fProxiesThatNeedWork.insert(thePIP->fProxy);
			fPSearchesThatNeedWork.insert
				(thePIP->fProxy->fDependentPSearches.begin(),
				thePIP->fProxy->fDependentPSearches.end()); // Hmmm?
			}
		}
	}

void Source_Union::pResultsAvailable(ZRef<Source> iSource)
	{
	PSource* thePSource = ZUtil_STL::sGetMustContain(kDebug, fMap_SourceToPSource, iSource);
	fPSources_ToCollectFrom.insert(thePSource);
	this->pInvokeCallable_ResultsAvailable();
	}

} // namespace ZDataspace
} // namespace ZooLib
