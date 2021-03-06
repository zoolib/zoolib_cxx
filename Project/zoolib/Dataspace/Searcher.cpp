// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Dataspace/Searcher.h"

#include "zoolib/Log.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/RelationalAlgebra/GetRelHead.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - SearchSpec

SearchSpec::SearchSpec()
	{}

SearchSpec::SearchSpec(const SearchSpec& iOther)
:	fConcreteHead(iOther.fConcreteHead)
,	fRestriction(iOther.fRestriction)
	{}

SearchSpec::~SearchSpec()
	{}

SearchSpec SearchSpec::operator=(const SearchSpec& iOther)
	{
	fConcreteHead = iOther.fConcreteHead;
	fRestriction = iOther.fRestriction;
	return *this;
	}

SearchSpec::SearchSpec(const ConcreteHead& iConcreteHead,
	const ZP<Expr_Bool>& iRestriction)
:	fConcreteHead(iConcreteHead)
,	fRestriction(iRestriction)
	{}

bool SearchSpec::operator==(const SearchSpec& iOther) const
	{
	return fConcreteHead == iOther.fConcreteHead
		&& 0 == fRestriction->Compare(iOther.fRestriction);
	}

bool SearchSpec::operator<(const SearchSpec& iOther) const
	{
	if (fConcreteHead < iOther.fConcreteHead)
		return true;

	if (iOther.fConcreteHead < fConcreteHead)
		return false;

	return fRestriction->Compare(iOther.fRestriction) < 0;
	}

const ConcreteHead& SearchSpec::GetConcreteHead() const
	{ return fConcreteHead; }

const ZP<Expr_Bool>& SearchSpec::GetRestriction() const
	{ return fRestriction; }

// =================================================================================================
#pragma mark - AddedSearch

AddedSearch::AddedSearch()
	{}

AddedSearch::AddedSearch(const AddedSearch& iOther)
:	fRefcon(iOther.fRefcon)
,	fSearchSpec(iOther.fSearchSpec)
	{}

AddedSearch::~AddedSearch()
	{}

AddedSearch& AddedSearch::operator=(const AddedSearch& iOther)
	{
	fRefcon = iOther.fRefcon;
	fSearchSpec = iOther.fSearchSpec;
	return *this;
	}

AddedSearch::AddedSearch(int64 iRefcon, const SearchSpec& iSearchSpec)
:	fRefcon(iRefcon)
,	fSearchSpec(iSearchSpec)
	{}

int64 AddedSearch::GetRefcon() const
	{ return fRefcon; }

const SearchSpec& AddedSearch::GetSearchSpec() const
	{ return fSearchSpec; }

// =================================================================================================
#pragma mark - SearchResult

SearchResult::SearchResult()
:	fRefcon(0)
	{}

SearchResult::SearchResult(const SearchResult& iOther)
:	fRefcon(iOther.fRefcon)
,	fResult(iOther.fResult)
	{}

SearchResult::~SearchResult()
	{}

SearchResult& SearchResult::operator=(const SearchResult& iOther)
	{
	fRefcon = iOther.fRefcon;
	fResult = iOther.fResult;
	return *this;
	}

SearchResult::SearchResult(int64 iRefcon, const ZP<QueryEngine::Result>& iResult)
:	fRefcon(iRefcon)
,	fResult(iResult)
	{}

int64 SearchResult::GetRefcon() const
	{ return fRefcon; }

ZP<QueryEngine::Result> SearchResult::GetResult() const
	{ return fResult; }

// =================================================================================================
#pragma mark - Searcher

Searcher::Searcher()
	{}

Searcher::~Searcher()
	{}

void Searcher::SetCallable_SearcherResultsAvailable(
	ZP<Callable_SearcherResultsAvailable> iCallable)
	{
	ZAcqMtx acq(fMtx);
	fCalled_SearcherResultsAvailable.Reset();
	fCallable_SearcherResultsAvailable = iCallable;
	}

void Searcher::pCollectResultsCalled()
	{
	ZAcqMtx acq(fMtx);
	fCalled_SearcherResultsAvailable.Reset();
	}

void Searcher::pTriggerSearcherResultsAvailable()
	{
	ZAcqMtx acq(fMtx);
	if (not fCalled_SearcherResultsAvailable())
		{
		if (ZP<Callable_SearcherResultsAvailable> theCallable =
			fCallable_SearcherResultsAvailable)
			{
			ZRelMtx rel(fMtx);
			theCallable->Call(this);
			}
		}
	}

} // namespace Dataspace
} // namespace ZooLib
