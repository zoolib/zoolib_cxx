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

#include "zoolib/dataspace/ZDataspace_Source_Union.h"

#include "zoolib/zqe/ZQE_Iterator_Any.h"
#include "zoolib/zqe/ZQE_Iterator_Std.h"
#include "zoolib/zqe/ZQE_Result_Any.h"

#include "zoolib/zra/ZRA_Util_RelOperators.h"

// Debugging
#include "zoolib/ZStdIO.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"
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
	vector<RelRename> fRelRenames;
	PredCompound fPredCompound;
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
	PQuery(int64 iRefcon);

	void Regenerate();

	int64 fRefcon;
	ZRef<ZRA::Expr_Rel> fRel;
	SearchThing fSearchThing;

	DListHead<DLink_PSourceProduct_InPQuery> fPSourceProducts;
	DListHead<DLink_PSourceSearches_InPQuery> fPSourceSearches;

	vector<ZRef<ZRA::Expr_Rel> > fRels;

	vector<ZRef<ZQE::Result> > fResults;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Iterator_PSourceProduct

class Source_Union::Iterator_PSourceProduct : public ZQE::Iterator
	{
public:
	Iterator_PSourceProduct(PSourceProduct* iPSP);
	Iterator_PSourceProduct(PSourceProduct* iPSP, size_t iIndex);

	virtual ~Iterator_PSourceProduct();

	virtual ZRef<ZQE::Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

private:
	PSourceProduct* fPSP;
	size_t fIndex;
	};

Source_Union::Iterator_PSourceProduct::Iterator_PSourceProduct(PSourceProduct* iPSP)
:	fPSP(iPSP)
,	fIndex(0)
	{}

Source_Union::Iterator_PSourceProduct::Iterator_PSourceProduct(
	PSourceProduct* iPSP, size_t iIndex)
:	fPSP(iPSP)
,	fIndex(iIndex)
	{}

Source_Union::Iterator_PSourceProduct::~Iterator_PSourceProduct()
	{}

ZRef<ZQE::Iterator> Source_Union::Iterator_PSourceProduct::Clone()
	{ return new Iterator_PSourceProduct(fPSP, fIndex); }

ZRef<ZQE::Result> Source_Union::Iterator_PSourceProduct::ReadInc()
	{
	if (fIndex < fPSP->fResults.size())
		return fPSP->fResults[fIndex++];
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::Iterator_PSourceSearches

class Source_Union::Iterator_PSourceSearches : public ZQE::Iterator
	{
public:
	Iterator_PSourceSearches(PSourceSearches* iPSS, size_t iWhichVector);
	Iterator_PSourceSearches(PSourceSearches* iPSS, size_t iWhichVector, size_t iIndex);

	virtual ~Iterator_PSourceSearches();

	virtual ZRef<ZQE::Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

private:
	PSourceSearches* fPSS;
	size_t fWhichVector;
	size_t fIndex;
	};

Source_Union::Iterator_PSourceSearches::Iterator_PSourceSearches(
	PSourceSearches* iPSS, size_t iWhichVector)
:	fPSS(iPSS)
,	fWhichVector(iWhichVector)
,	fIndex(0)
	{}

Source_Union::Iterator_PSourceSearches::Iterator_PSourceSearches(
	PSourceSearches* iPSS, size_t iWhichVector, size_t iIndex)
:	fPSS(iPSS)
,	fWhichVector(iWhichVector)
,	fIndex(iIndex)
	{}

Source_Union::Iterator_PSourceSearches::~Iterator_PSourceSearches()
	{}

ZRef<ZQE::Iterator> Source_Union::Iterator_PSourceSearches::Clone()
	{ return new Iterator_PSourceSearches(fPSS, fWhichVector, fIndex); }

ZRef<ZQE::Result> Source_Union::Iterator_PSourceSearches::ReadInc()
	{
	if (fIndex < fPSS->fResultsVector[fWhichVector].size())
		return fPSS->fResultsVector[fWhichVector][fIndex++];
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::PQuery definition

Source_Union::PQuery::PQuery(int64 iRefcon)
:	fRefcon(iRefcon)
	{}

void Source_Union::PQuery::Regenerate()
	{
	ZRA::Util_Strim_Rel::sToStrim(fRel, ZStdIO::strim_err);
	ZStdIO::strim_err << "\n";

	fResults.clear();

	// For each entry in fRels we have a source rel, effectively. Union the
	// results from each of the fPSourceSearches for each index. Join that
	// with each of the other fPSourceSearches, and with each of the fPSourceproducts,
	// and filter by fSearchThing's fValPredCompound.

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
					ZYad_ZooLibStrim::sToStrim(sMakeYadR(theVal), ZStdIO::strim_err);
					ZStdIO::strim_err << "\n";

					if (fSearchThing.fPredCompound.Matches(theContext, theVal))
						fResults.push_back(theResult);
					}
				}
			else
				break;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union::PSource

class Source_Union::PSource
	{
public:
	PSource(Source* iSource);

	Source* fSource;

	int64 fNextRefcon;

	map<int64, PSourceProduct*> fMap_RefconToPSourceProduct;
	map<int64, PSourceSearches*> fMap_RefconToPSourceSearches;

	DListHead<DLink_PSourceProduct_ToAdd> fPSourceProducts_ToAdd;
	DListHead<DLink_PSourceSearches_ToAdd> fPSourceSearches_ToAdd;

	vector<int64> fPSourceQueries_ToRemove;
	};

Source_Union::PSource::PSource(Source* iSource)
:	fSource(iSource)
,	fNextRefcon(1)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union helpers

static ZValPred spRenamed(const RelRename& iRelRename, const ZValPred& iPred)
	{
	// Replace to-names in iPred with the from-names
	return iPred.Renamed(iRelRename.GetRename());
	}

static PredCompound::Sect spExtractPertinent(
	PredCompound::Sect& ioSect, const RelRename& iRelRename)
	{
	// iRelHead -- the set of names provided by the source
	// So we preserve only those ZValPreds whose names all appear in iRelHead
	
	const RelHead theTos = iRelRename.GetRelHead_To();
	PredCompound::Sect result;
	for (PredCompound::Sect::iterator iterSect = ioSect.begin();
		iterSect != ioSect.end(); /*no inc*/)
		{
		if (theTos.Contains((*iterSect).GetNames()))
			{
			result.push_back(spRenamed(iRelRename.Inverted(), *iterSect));
			iterSect = ioSect.erase(iterSect);
			}
		else
			{
			++iterSect;
			}
		}
	return result;
	}

static PredCompound spExtractPertinent(
	PredCompound& ioPredCompound, const RelRename& iRelRename)
	{
	PredCompound result;
	for (PredCompound::SectUnion::iterator iterUnion = ioPredCompound.fSectUnion.begin();
		iterUnion != ioPredCompound.fSectUnion.end(); ++iterUnion)
		{
		result |= spExtractPertinent(*iterUnion, iRelRename);
		}
	return result;
	}

static RelRename spMerge(const vector<RelRename>& iRelRenames)
	{
	RelRename result;
	for (vector<RelRename>::const_iterator i = iRelRenames.begin(); i != iRelRenames.end(); ++i)
		result |= *i;
	return result;
	}

static bool spIsEmptyOrContains(const set<RelHead>& iRelHeads, const RelHead& iRH)
	{ return iRelHeads.empty() || ZUtil_STL::sContains(iRelHeads, iRH); }

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
	// If any of our sources have an relhead set (signifying universality) then we must do the same.
	set<RelHead> result;
	for (map<Source*, PSource*>::iterator i = fMap_SourceToPSource.begin();
		i != fMap_SourceToPSource.end(); ++i)
		{
		const set<RelHead>& theRelHeads = (*i).first->GetRelHeads();
		if (theRelHeads.empty())
			return set<RelHead>();
		result = ZUtil_STL_set::sOr(result, theRelHeads);
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

	// Remember which queries were registered, don't bother to re-register any in iRemoved.
	set<int64> removedSorted(iRemoved, iRemoved + iRemovedCount);
	vector<int64> theRemoved;
	vector<AddedSearch> theAdded(iAdded, iAdded + iAddedCount);
	for (map<int64, PQuery*>::iterator i = fMap_RefconToPQuery.begin();
		i != fMap_RefconToPQuery.end(); ++i)
		{
		PQuery* thePQuery = (*i).second;
		if (!ZUtil_STL::sContains(removedSorted, thePQuery->fRefcon))
			theAdded.push_back(AddedSearch(thePQuery->fRefcon, thePQuery->fRel));
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

void Source_Union::InsertSource(Source* iSource)
	{
	ZAssert(! ZUtil_STL::sContains(fMap_SourceToPSource, iSource));
	fPSources_ToAdd.insert(new PSource(iSource));
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
		PQuery* thePQuery = new PQuery(iAdded->fRefcon);
		thePQuery->fRel = iAdded->fRel;
		++iAdded;

		thePQuery->fSearchThing = sAsSearchThing(thePQuery->fRel);
		ZUtil_STL::sInsertMustNotContain(kDebug,
			fMap_RefconToPQuery, thePQuery->fRefcon, thePQuery);

		// Go through the SearchThing's RelRenames, for each see if there's a
		// single source that handles it, leaving any remainder to other sources.
		vector<RelRename> theRelRenames = thePQuery->fSearchThing.fRelRenames;
		map<PSource*, vector<RelRename> > products;
		for (vector<RelRename>::iterator iterRelRenames = theRelRenames.begin();
			iterRelRenames != theRelRenames.end(); /*no inc*/)
			{
			PSource* soleSourceHandlingRelRename = nullptr;
			for (map<Source*, PSource*>::iterator iterSource = fMap_SourceToPSource.begin();
				iterSource != fMap_SourceToPSource.end(); ++iterSource)
				{
				if (spIsEmptyOrContains(
					(*iterSource).first->GetRelHeads(), (*iterRelRenames).GetRelHead_From()))
					{
					if (! soleSourceHandlingRelRename)
						{
						soleSourceHandlingRelRename = (*iterSource).second;
						}
					else
						{
						soleSourceHandlingRelRename = nullptr;
						break;
						}
					}
				}

 			if (! soleSourceHandlingRelRename)
				{
				++iterRelRenames;
				}
			else
				{
				products[soleSourceHandlingRelRename].push_back(*iterRelRenames);
				iterRelRenames = theRelRenames.erase(iterRelRenames);
				}
			}

		// Anything remaining in theRelRenames is to be produced locally.
		for (vector<RelRename>::iterator i = theRelRenames.begin(); i != theRelRenames.end(); ++i)
			{
			// Get a concrete using the rel's real names.
			ZRef<ZRA::Expr_Rel> theRel = sAsRel((*i).GetRelHead_From());

			// Get the subset of iPred that applies to iRelRename, but using the rel's real names.
			const PredCompound thePredCompound =
				spExtractPertinent(thePQuery->fSearchThing.fPredCompound, *i);

			theRel = theRel & sAsExpr_Logic(thePredCompound);

			thePQuery->fRels.push_back(theRel);
			}

		for (map<Source*, PSource*>::iterator iterSource = fMap_SourceToPSource.begin();
			iterSource != fMap_SourceToPSource.end(); ++iterSource)
			{
			PSource* thePSource = (*iterSource).second;

			map<PSource*, vector<RelRename> >::iterator iterProducts = products.find(thePSource);
			if (iterProducts != products.end())
				{
				PSourceProduct* thePSP =
					new PSourceProduct(thePSource, thePQuery, thePSource->fNextRefcon++);
				thePSource->fMap_RefconToPSourceProduct[thePSP->fRefcon] = thePSP;
				thePSP->fRelRenames = (*iterProducts).second;
				thePSP->fPredCompound = spExtractPertinent(
					thePQuery->fSearchThing.fPredCompound, spMerge(thePSP->fRelRenames));
				thePSource->fPSourceProducts_ToAdd.Insert(thePSP);
				thePQuery->fPSourceProducts.Insert(thePSP);
				}
			else
				{
				PSourceSearches* thePSS =
					new PSourceSearches(thePSource, thePQuery, thePSource->fNextRefcon);
				thePSource->fNextRefcon += theRelRenames.size();
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
				PSourceProduct* thePSourceProduct = iter.Current();

				ZRef<ZRA::Expr_Rel> theRel =
					sAsRelFrom(thePSourceProduct->fRelRenames)
					& sAsExpr_Logic(thePSourceProduct->fPredCompound);

				localAdded.push_back(AddedSearch(thePSourceProduct->fRefcon, theRel));
				}

			for (DListIteratorEraseAll<PSourceSearches, DLink_PSourceSearches_ToAdd>
				iter = thePSource->fPSourceSearches_ToAdd; iter; iter.Advance())
				{
				PSourceSearches* thePSourceSearches = iter.Current();
				PQuery* thePQuery = thePSourceSearches->fPQuery;
				int64 theRefcon = thePSourceSearches->fFirstRefcon;
				thePSourceSearches->fResultsVector.resize(thePQuery->fRels.size());
				for (vector<ZRef<ZRA::Expr_Rel> >::iterator i = thePQuery->fRels.begin();
					i != thePQuery->fRels.end(); ++i)
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
					const int64 offset = thePSourceSearches->fResultsVector.size() + theRefcon - foundRefcon - 1;
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

		for (size_t x = thePSourceSearches->fPQuery->fRels.size(); x; --x)
			thePSource->fPSourceQueries_ToRemove.push_back(theRefcon++);

		delete thePSourceSearches;
		}

	delete iPQuery;
	}

} // namespace ZDataspace
} // namespace ZooLib
