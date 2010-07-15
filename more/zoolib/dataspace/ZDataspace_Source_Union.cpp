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

#include "zoolib/ZExpr_Logic.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZUtil_STL_set.h"

#include "zoolib/dataspace/ZDataspace_Source_Union.h"

#include "zoolib/zqe/ZQE_Iterator_Any.h"
#include "zoolib/zqe/ZQE_Iterator_Std.h"
#include "zoolib/zqe/ZQE_Result_Any.h"

#include "zoolib/zra/ZRA_Util_RelOperators.h"

// Debugging
//###include "zoolib/ZStdIO.h"
#include "zoolib/ZLog.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

namespace ZooLib {
namespace ZDataspace {

typedef ZValPredCompound PredCompound;

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::PSourceProduct

class Source_Union::DLink_PSourceProduct_ToAdd
:	public DListLink<PSourceProduct, DLink_PSourceProduct_ToAdd, kDebug>
	{};

class Source_Union::DLink_PSourceProduct_InPQuery
:	public DListLink<PSourceProduct, DLink_PSourceProduct_InPQuery, kDebug>
	{};

class Source_Union::PSourceProduct
:	public DLink_PSourceProduct_ToAdd
,	public DLink_PSourceProduct_InPQuery
	{
public:
	PSourceProduct(PSource* iPSource, PQuery* iPQuery, int64 iRefcon);

	PSource* fPSource;
	PQuery* fPQuery;
	int64 fRefcon;

	SearchThing fSearchThing;
	vector<ZRef<ZQE::Result> > fResults;
	};

Source_Union::PSourceProduct::PSourceProduct(PSource* iPSource, PQuery* iPQuery, int64 iRefcon)
:	fPSource(iPSource)
,	fPQuery(iPQuery)
,	fRefcon(iRefcon)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::PSourceSearches

class Source_Union::DLink_PSourceSearches_ToAdd
:	public DListLink<PSourceSearches, DLink_PSourceSearches_ToAdd, kDebug>
	{};

class Source_Union::DLink_PSourceSearches_InPQuery
:	public DListLink<PSourceSearches, DLink_PSourceSearches_InPQuery, kDebug>
	{};

class Source_Union::PSourceSearches
:	public DLink_PSourceSearches_ToAdd
,	public DLink_PSourceSearches_InPQuery
	{
public:
	PSourceSearches(PSource* iPSource, PQuery* iPQuery, int64 iFirstRefcon);

	PSource* fPSource;
	PQuery* fPQuery;
	int64 fFirstRefcon;

	vector<vector<ZRef<ZQE::Result> > > fResultsVector;
	};

Source_Union::PSourceSearches::PSourceSearches(PSource* iPSource, PQuery* iPQuery, int64 iFirstRefcon)
:	fPSource(iPSource)
,	fPQuery(iPQuery)
,	fFirstRefcon(iFirstRefcon)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::PQuery declaration

class Source_Union::DLink_PQuery_Changed
:	public DListLink<PQuery, DLink_PQuery_Changed, kDebug>
	{};

class Source_Union::PQuery
:	public DLink_PQuery_Changed
	{
public:
	PQuery(int64 iRefcon, const SearchThing& iSearchThing);

	void Regenerate();

	const int64 fRefcon;
	const SearchThing fSearchThing;

	DListHead<DLink_PSourceProduct_InPQuery> fPSourceProducts;
	DListHead<DLink_PSourceSearches_InPQuery> fPSourceSearches;

	vector<SearchThing> fSearchThings;

	vector<ZRef<ZQE::Result> > fResults;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::PQuery definition

Source_Union::PQuery::PQuery(int64 iRefcon, const SearchThing& iSearchThing)
:	fRefcon(iRefcon)
,	fSearchThing(iSearchThing)
	{}

void Source_Union::PQuery::Regenerate()
	{
	if (ZLOGPF(s, eDebug))
		s << fSearchThing;

	fResults.clear();

	// For each entry in fRels we have a source rel, effectively. Union the
	// results from each of the fPSourceSearches for each position. Join that
	// with each of the other fPSourceSearches, and with each of the fPSourceProducts,
	// and filter by fSearchThing's fValPredCompound.

#if 0 //##
	ZRef<ZQE::Iterator> theProduct;

	for (size_t x = 0; x < fRels.size(); ++x)
		{
		ZRef<ZQE::Iterator> theUnion;
		for (DListIterator<PSourceSearches, DLink_PSourceSearches_InPQuery>
			iter = fPSourceSearches; iter; iter.Advance())
			{
			PSourceSearches* thePSS = iter.Current();
			ZRef<ZQE::Iterator> anIterator = new Iterator_PSourceSearches(thePSS, x);
			if (theUnion)
				theUnion = new ZQE::Iterator_Union(theUnion, anIterator);
			else
				theUnion = anIterator;
			}

		if (theUnion)
			{
			if (theProduct)
				theProduct = new ZQE::Iterator_Product(theProduct, theUnion);
			else
				theProduct = theUnion;
			}
		}

	for (DListIterator<PSourceProduct, DLink_PSourceProduct_InPQuery>
		iter = fPSourceProducts; iter; iter.Advance())
		{
		PSourceProduct* thePSP = iter.Current();
		ZRef<ZQE::Iterator> anIterator = new Iterator_PSourceProduct(thePSP);

		if (theProduct)
			theProduct = new ZQE::Iterator_Product(theProduct, anIterator);
		else
			theProduct = anIterator;
		}

	if (theProduct)
		{
		ZValContext theContext;
		for (;;)
			{
			if (ZRef<ZQE::Result> theResult = theProduct->ReadInc())
				{
				if (ZRef<ZQE::Result_Any> theResult_Any = theResult.DynamicCast<ZQE::Result_Any>())
					{
					ZVal_Any theVal = theResult_Any->GetVal();
					if (ZLOGPF(s, eDebug))
						ZYad_ZooLibStrim::sToStrim(sMakeYadR(theVal), s);

					if (fSearchThing.fPredCompound.Matches(theContext, theVal))
						fResults.push_back(theResult);
					}
				}
			else
				break;
			}
		}
#endif
	}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::PSource

class Source_Union::PSource
	{
public:
	PSource(Source* iSource, const string8& iPrefix);

	Source* fSource;
	const string8 fPrefix;

	int64 fNextRefcon;

	map<int64, PSourceProduct*> fMap_RefconToPSourceProduct;
	map<int64, PSourceSearches*> fMap_RefconToPSourceSearches;

	DListHead<DLink_PSourceProduct_ToAdd> fPSourceProducts_ToAdd;
	DListHead<DLink_PSourceSearches_ToAdd> fPSourceSearches_ToAdd;

	vector<int64> fPSourceQueries_ToRemove;
	};

Source_Union::PSource::PSource(Source* iSource, const string8& iPrefix)
:	fSource(iSource)
,	fPrefix(iPrefix)
,	fNextRefcon(1)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union helpers

static ZValPred spRenamed(const NameMap& iNameMap, const ZValPred& iPred)
	{
	// Replace to-names in iPred with the from-names
	return iPred.Renamed(iNameMap.GetRename());
	}

static PredCompound::Sect spPertinentOnly(
	const PredCompound::Sect& iSect, const NameMap& iNameMap)
	{
	// iRelHead -- the set of names provided by the source
	// So we preserve only those ZValPreds whose names all appear in iRelHead
	
	const RelHead theTos = iNameMap.GetRelHead_To();
	PredCompound::Sect result;
	for (PredCompound::Sect::const_iterator iterSect = iSect.begin();
		iterSect != iSect.end(); ++iterSect)
		{
		if (theTos.Contains((*iterSect).GetNames()))
			result.push_back(*iterSect);
//			result.push_back(spRenamed(iNameMap.Inverted(), *iterSect));
		}
	return result;
	}

static PredCompound spPertinentOnly(
	const PredCompound& iPredCompound, const NameMap& iNameMap)
	{
	PredCompound result;
	for (PredCompound::SectUnion::const_iterator iterUnion = iPredCompound.fSectUnion.begin();
		iterUnion != iPredCompound.fSectUnion.end(); ++iterUnion)
		{
		result |= spPertinentOnly(*iterUnion, iNameMap);
		}
	return result;
	}

static NameMap spMerge(const vector<NameMap>& iNameMaps)
	{
	NameMap result;
	for (vector<NameMap>::const_iterator i = iNameMaps.begin(); i != iNameMaps.end(); ++i)
		result |= *i;
	return result;
	}

static bool spIsEmptyOrContains(const set<RelHead>& iRelHeads, const RelHead& iRH)
	{
	if (iRelHeads.empty())
		return true;

	for (set<RelHead>::const_iterator i = iRelHeads.begin(); i != iRelHeads.end(); ++i)
		{
		if (ZUtil_STL_set::sIncludes(*i, iRH))
			return true;
		}
	return false;
//	return iRelHeads.empty() || ZUtil_STL::sContains(iRelHeads, iRH);
	}

static set<RelHead> spPrefixAdd(const string8& iPrefix, const set<RelHead>& iRelHeads)
	{
	if (iPrefix.empty())
		return iRelHeads;

	set<RelHead> result;
	for (set<RelHead>::const_iterator i = iRelHeads.begin(); i != iRelHeads.end(); ++i)
		{
		ZAssertLog(0, !(*i).empty());
		result.insert(ZRA::sPrefixAdd(iPrefix, *i));
		}

	return result;
	}

static set<RelHead> spPrefixRemove(const string8& iPrefix, const set<RelHead>& iRelHeads)
	{
	if (iPrefix.empty())
		return iRelHeads;

	set<RelHead> result;
	for (set<RelHead>::const_iterator i = iRelHeads.begin(); i != iRelHeads.end(); ++i)
		{
		result.insert(ZRA::sPrefixRemove(iPrefix, *i));
		}

	return result;
	}

static NameMap spPrefixRemove(const string8& iPrefix, const NameMap& iNameMap)
	{
	set<NameMap::Elem_t> result;
	const set<NameMap::Elem_t>& theElems = iNameMap.GetElems();
	for (set<NameMap::Elem_t>::const_iterator i = theElems.begin(); i != theElems.end(); ++i)
		result.insert(NameMap::Elem_t((*i).first, ZRA::sPrefixRemove(iPrefix, (*i).second)));
	return result;
	}

static vector<NameMap> spPrefixRemove(const string8& iPrefix, const vector<NameMap>& iNameMaps)
	{
	if (iPrefix.empty())
		return iNameMaps;

	vector<NameMap> result;
	for (vector<NameMap>::const_iterator i = iNameMaps.begin(); i != iNameMaps.end(); ++i)
		result.push_back(spPrefixRemove(iPrefix, *i));

	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union

Source_Union::Source_Union()
:	fNextRefcon(1)
	{}

Source_Union::~Source_Union()
	{}

set<RelHead> Source_Union::GetRelHeads()
	{
	set<RelHead> result;
	for (map<Source*, PSource*>::iterator i = fMap_SourceToPSource.begin();
		i != fMap_SourceToPSource.end(); ++i)
		{
		const set<RelHead>& theRelHeads = (*i).first->GetRelHeads();
		if (theRelHeads.empty())
			{
			// If any of our sources have an empty relhead set (signifying universality)
			// then we must do the same.
			return set<RelHead>();
			}
		result = ZUtil_STL_set::sOr(result, spPrefixAdd((*i).second->fPrefix, theRelHeads));
		}
	return result;
	}

void Source_Union::Update(
	bool iLocalOnly,
	AddedSearch* iAdded, size_t iAddedCount,
	int64* iRemoved, size_t iRemovedCount,
	vector<SearchResult>& oChanged,
	Clock& oClock)
	{
	if (fPSources_ToRemove.empty() && fPSources_ToAdd.empty())
		{
		this->pUpdate(iAdded, iAddedCount, iRemoved, iRemovedCount, oChanged, oClock);
		return;
		}

	// Remove sources.
	for (set<PSource*>::iterator iterPSource = fPSources_ToRemove.begin();
		iterPSource != fPSources_ToRemove.end(); ++iterPSource)
		{
		PSource* thePSource = *iterPSource;
		ZUtil_STL::sEraseMustContain(kDebug, fMap_SourceToPSource, thePSource->fSource);

		for (map<int64, PSourceProduct*>::iterator
			i = thePSource->fMap_RefconToPSourceProduct.begin();
			i != thePSource->fMap_RefconToPSourceProduct.end(); ++i)
			{
			PSourceProduct* thePSP = (*i).second;
			thePSP->fPQuery->fPSourceProducts.Erase(thePSP);
			delete thePSP;
			}

		for (map<int64, PSourceSearches*>::iterator
			i = thePSource->fMap_RefconToPSourceSearches.begin();
			i != thePSource->fMap_RefconToPSourceSearches.end(); ++i)
			{
			PSourceSearches* thePSS = (*i).second;
			thePSS->fPQuery->fPSourceSearches.Erase(thePSS);
			delete thePSS;
			}

		delete thePSource;
		}
	fPSources_ToRemove.clear();

	// Remember which queries were registered, don't bother to re-register any that are in iRemoved.
	set<int64> removedSorted(iRemoved, iRemoved + iRemovedCount);
	vector<int64> theRemoved;
	vector<AddedSearch> theAdded(iAdded, iAdded + iAddedCount);
	for (map<int64, PQuery*>::iterator i = fMap_RefconToPQuery.begin();
		i != fMap_RefconToPQuery.end(); ++i)
		{
		PQuery* thePQuery = (*i).second;
		if (!ZUtil_STL::sContains(removedSorted, thePQuery->fRefcon))
			theAdded.push_back(AddedSearch(thePQuery->fRefcon, thePQuery->fSearchThing));
		theRemoved.push_back(thePQuery->fRefcon);
		}

	// Remove them
	this->pUpdate(nullptr, 0,
		ZUtil_STL::sFirstOrNil(theRemoved), theRemoved.size(),
		oChanged, oClock);

	// We won't get any changes, but ensure that the vector is clear.
	oChanged.clear();

	// Add in new sources
	for (set<PSource*>::iterator iterPSource = fPSources_ToAdd.begin();
		iterPSource != fPSources_ToAdd.end(); ++iterPSource)
		{
		PSource* thePSource = *iterPSource;
		ZUtil_STL::sInsertMustNotContain(kDebug,
			fMap_SourceToPSource, thePSource->fSource, thePSource);
		}
	fPSources_ToAdd.clear();

	// And register/re-register queries.
	this->pUpdate(ZUtil_STL::sFirstOrNil(theAdded), theAdded.size(),
		nullptr, 0,
		oChanged, oClock);
	}

void Source_Union::InsertSource(Source* iSource, const string8& iPrefix)
	{
	ZAssert(! ZUtil_STL::sContains(fMap_SourceToPSource, iSource));
	fPSources_ToAdd.insert(new PSource(iSource, iPrefix));
	}

void Source_Union::EraseSource(Source* iSource)
	{
	ZAssert(ZUtil_STL::sContains(fMap_SourceToPSource, iSource));
	ZUtil_STL::sInsertMustNotContain(kDebug, fPSources_ToAdd, fMap_SourceToPSource[iSource]);
	}

void Source_Union::pUpdate(
	AddedSearch* iAdded, size_t iAddedCount,
	int64* iRemoved, size_t iRemovedCount,
	vector<SearchResult>& oChanged,
	Clock& oClock)
	{
	oChanged.clear();

	ZAssert(fPQuery_Changed.Empty());

	while (iRemovedCount--)
		{
		PQuery* thePQuery = ZUtil_STL::sEraseAndReturn(kDebug, fMap_RefconToPQuery, *iRemoved++);
		this->pDetachPQuery(thePQuery);
		}

	while (iAddedCount--)
		{
		PQuery* thePQuery = new PQuery(iAdded->fRefcon, iAdded->fSearchThing);
		++iAdded;

		ZUtil_STL::sInsertMustNotContain(kDebug,
			fMap_RefconToPQuery, thePQuery->fRefcon, thePQuery);

		// Go through the SearchThing's NameMaps. For each, see if there's a
		// single source that handles it. Any that isn't is left in theNameMaps.
		vector<NameMap> theNameMaps = thePQuery->fSearchThing.fNameMaps;
		map<PSource*, vector<NameMap> > products;
		for (vector<NameMap>::iterator iterNameMaps = theNameMaps.begin();
			iterNameMaps != theNameMaps.end(); /*no inc*/)
			{
			PSource* soleSourceHandlingNameMap = nullptr;
			for (map<Source*, PSource*>::iterator iterSource = fMap_SourceToPSource.begin();
				iterSource != fMap_SourceToPSource.end(); ++iterSource)
				{
				PSource* thePSource = (*iterSource).second;
				const string thePrefix = thePSource->fPrefix;
				set<RelHead> theSet = (*iterSource).first->GetRelHeads();

				theSet = spPrefixAdd(thePrefix, theSet);
				
				if (ZLOGPF(s, eDebug))
					s << theSet;
				
				const RelHead theRHFrom = (*iterNameMaps).GetRelHead_From();

				if (ZLOGPF(s, eDebug))
					s << theRHFrom;

				if (spIsEmptyOrContains(theSet, theRHFrom))
					{
					if (! soleSourceHandlingNameMap)
						{
						soleSourceHandlingNameMap = (*iterSource).second;
						}
					else
						{
						soleSourceHandlingNameMap = nullptr;
						break;
						}
					}
				}

 			if (! soleSourceHandlingNameMap)
				{
				++iterNameMaps;
				}
			else
				{
				products[soleSourceHandlingNameMap].push_back(*iterNameMaps);
				iterNameMaps = theNameMaps.erase(iterNameMaps);
				}
			}

		// Anything remaining in theNameMaps is to be product-ed locally.
		for (vector<NameMap>::iterator i = theNameMaps.begin(); i != theNameMaps.end(); ++i)
			{
			SearchThing theSearchThing;
			theSearchThing.fNameMaps.push_back(*i);
			theSearchThing.fPredCompound =
				spPertinentOnly(thePQuery->fSearchThing.fPredCompound, *i);
			thePQuery->fSearchThings.push_back(theSearchThing);
			}

		// Create the PSourceProduct and PSourceSearches instances for each PSource.
		for (map<Source*, PSource*>::iterator iterSource = fMap_SourceToPSource.begin();
			iterSource != fMap_SourceToPSource.end(); ++iterSource)
			{
			PSource* thePSource = (*iterSource).second;

			map<PSource*, vector<NameMap> >::iterator iterProducts = products.find(thePSource);
			if (iterProducts != products.end())
				{
				PSourceProduct* thePSP =
					new PSourceProduct(thePSource, thePQuery, thePSource->fNextRefcon++);
				thePSource->fMap_RefconToPSourceProduct[thePSP->fRefcon] = thePSP;
				thePSP->fSearchThing.fNameMaps = (*iterProducts).second;
				thePSP->fSearchThing.fPredCompound = spPertinentOnly(
					thePQuery->fSearchThing.fPredCompound, spMerge(thePSP->fSearchThing.fNameMaps));
				thePSource->fPSourceProducts_ToAdd.Insert(thePSP);
				thePQuery->fPSourceProducts.Insert(thePSP);
				}
			else
				{
				PSourceSearches* thePSS =
					new PSourceSearches(thePSource, thePQuery, thePSource->fNextRefcon);
				// We can use theRelHeads.size() or thePQuery->fSearchThings.size();
				ZAssert(thePQuery->fSearchThings.size() == theNameMaps.size());
				thePSource->fNextRefcon += thePQuery->fSearchThings.size();
				thePSource->fMap_RefconToPSourceSearches[thePSource->fNextRefcon - 1] = thePSS;
				thePSource->fPSourceSearches_ToAdd.Insert(thePSS);
				thePQuery->fPSourceSearches.Insert(thePSS);
				}
			}
		}

	// >------
	// We can/should parallelize this next bit, one task per source.
	for (map<Source*, PSource*>::iterator i = fMap_SourceToPSource.begin();
		i != fMap_SourceToPSource.end(); ++i)
		{
		PSource* thePSource = (*i).second;
		if (thePSource->fPSourceProducts_ToAdd.Size()
			|| thePSource->fPSourceSearches_ToAdd.Size()
			|| thePSource->fPSourceQueries_ToRemove.size())
			{
			vector<AddedSearch> localAdded;

			for (DListIteratorEraseAll<PSourceProduct, DLink_PSourceProduct_ToAdd>
				iter = thePSource->fPSourceProducts_ToAdd; iter; iter.Advance())
				{
				// We need to modify the searchthing's namemap so it's 'from' values
				// have the prefix stripped.
				PSourceProduct* thePSourceProduct = iter.Current();
				SearchThing theSearchThing;
				theSearchThing.fPredCompound = thePSourceProduct->fSearchThing.fPredCompound;
				theSearchThing.fNameMaps =
					spPrefixRemove(thePSource->fPrefix, thePSourceProduct->fSearchThing.fNameMaps);
				localAdded.push_back(
					AddedSearch(thePSourceProduct->fRefcon, theSearchThing));
				}

			for (DListIteratorEraseAll<PSourceSearches, DLink_PSourceSearches_ToAdd>
				iter = thePSource->fPSourceSearches_ToAdd; iter; iter.Advance())
				{
				PSourceSearches* thePSourceSearches = iter.Current();
				PQuery* thePQuery = thePSourceSearches->fPQuery;
				int64 theRefcon = thePSourceSearches->fFirstRefcon;
				thePSourceSearches->fResultsVector.resize(thePQuery->fSearchThings.size());
				for (vector<SearchThing>::iterator i = thePQuery->fSearchThings.begin();
					i != thePQuery->fSearchThings.end(); ++i)
					{
					localAdded.push_back(AddedSearch(theRefcon++, *i));
					}
				}

			vector<int64> localRemoved;
			localRemoved.swap(thePSource->fPSourceQueries_ToRemove);

			vector<SearchResult> localChanged;
			Clock localClock;
			thePSource->fSource->Update(false,
				ZUtil_STL::sFirstOrNil(localAdded), localAdded.size(),
				ZUtil_STL::sFirstOrNil(localRemoved), localRemoved.size(),
				localChanged,
				localClock);

			// Watch this -- we'll need to handle the clock join carefully when MTed.
			fClock.Join(localClock);

			for (vector<SearchResult>::iterator i = localChanged.begin();
				i != localChanged.end(); ++i)
				{
				const int64 theRefcon = (*i).fRefcon;
				map<int64, PSourceProduct*>::iterator iterProduct =
					thePSource->fMap_RefconToPSourceProduct.find(theRefcon);

				if (iterProduct != thePSource->fMap_RefconToPSourceProduct.end()
					&& (*iterProduct).first == theRefcon)
					{
					PSourceProduct* thePSourceProduct = (*iterProduct).second;
					thePSourceProduct->fResults.swap((*i).fResults);
					fPQuery_Changed.InsertIfNotContains(thePSourceProduct->fPQuery);
					}
				else
					{
					map<int64, PSourceSearches*>::iterator iterSearches =
						thePSource->fMap_RefconToPSourceSearches.lower_bound(theRefcon);
					ZAssert(iterSearches != thePSource->fMap_RefconToPSourceSearches.end());
					PSourceSearches* thePSourceSearches = (*iterSearches).second;
					const int64 foundRefcon = (*iterSearches).first;
					const int64 offset =
						thePSourceSearches->fResultsVector.size() + theRefcon - foundRefcon - 1;
					thePSourceSearches->fResultsVector[offset].swap((*i).fResults);
					fPQuery_Changed.InsertIfNotContains(thePSourceSearches->fPQuery);
					}
				}
			}
		}
	// <------


	// >------
	// Now go through all PQuerys in PQueryChanged and re-run their searches.
	// Again, this could be parallel, but per query and across sources.
	for (DListIterator<PQuery, DLink_PQuery_Changed>
		iter = fPQuery_Changed; iter; iter.Advance())
		{
		iter.Current()->Regenerate();
		}
	// <------

	// Back to single-threaded, to extract results and put them in oChanged.
	oChanged.reserve(fPQuery_Changed.Size());
	for (DListIteratorEraseAll<PQuery, DLink_PQuery_Changed>
		iter = fPQuery_Changed; iter; iter.Advance())
		{
		PQuery* thePQuery = iter.Current();
		SearchResult theSearchResult;
		theSearchResult.fRefcon = thePQuery->fRefcon;
		theSearchResult.fResults = thePQuery->fResults;
		oChanged.push_back(theSearchResult);
		}

	oClock = fClock;
	}

void Source_Union::pDetachPQuery(PQuery* iPQuery)
	{
	fPQuery_Changed.EraseIfContains(iPQuery);

	for (DListIteratorEraseAll<PSourceProduct, DLink_PSourceProduct_InPQuery>
		iter = iPQuery->fPSourceProducts; iter; iter.Advance())
		{
		PSourceProduct* thePSourceProduct = iter.Current();
		PSource* thePSource = thePSourceProduct->fPSource;

		thePSource->fMap_RefconToPSourceProduct.erase(thePSourceProduct->fRefcon);

		thePSource->fPSourceQueries_ToRemove.push_back(thePSourceProduct->fRefcon);

		delete thePSourceProduct;
		}

	for (DListIteratorEraseAll<PSourceSearches, DLink_PSourceSearches_InPQuery>
		iter = iPQuery->fPSourceSearches; iter; iter.Advance())
		{
		PSourceSearches* thePSourceSearches = iter.Current();
		PSource* thePSource = thePSourceSearches->fPSource;

		int64 theRefcon = thePSourceSearches->fFirstRefcon;

		thePSource->fMap_RefconToPSourceSearches.erase(theRefcon);

		for (size_t x = thePSourceSearches->fPQuery->fSearchThings.size(); x; --x)
			thePSource->fPSourceQueries_ToRemove.push_back(theRefcon++);

		delete thePSourceSearches;
		}

	delete iPQuery;
	}

} // namespace ZDataspace
} // namespace ZooLib
