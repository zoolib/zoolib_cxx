// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Dataspace/Relater.h"

namespace ZooLib {
namespace Dataspace {

using std::set;

// =================================================================================================
#pragma mark - AddedQuery

AddedQuery::AddedQuery()
	{}

AddedQuery::AddedQuery(const AddedQuery& iOther)
:	fRefcon(iOther.fRefcon)
,	fRel(iOther.fRel)
	{}

AddedQuery::~AddedQuery()
	{}

AddedQuery& AddedQuery::operator=(const AddedQuery& iOther)
	{
	fRefcon = iOther.fRefcon;
	fRel = iOther.fRel;
	return *this;
	}

AddedQuery::AddedQuery(int64 iRefcon, const ZP<RelationalAlgebra::Expr_Rel>& iRel)
:	fRefcon(iRefcon)
,	fRel(iRel)
	{}

int64 AddedQuery::GetRefcon() const
	{ return fRefcon; }

ZP<RelationalAlgebra::Expr_Rel> AddedQuery::GetRel() const
	{ return fRel; }

// =================================================================================================
#pragma mark - QueryResult

QueryResult::QueryResult()
:	fRefcon(0)
	{}

QueryResult::QueryResult(const QueryResult& iOther)
:	fRefcon(iOther.fRefcon)
,	fResult(iOther.fResult)
,	fResultDeltas(iOther.fResultDeltas)
	{}

QueryResult::~QueryResult()
	{}

QueryResult& QueryResult::operator=(const QueryResult& iOther)
	{
	fRefcon = iOther.fRefcon;
	fResult = iOther.fResult;
	fResultDeltas = iOther.fResultDeltas;
	return *this;
	}

QueryResult::QueryResult(int64 iRefcon, const ZP<Result>& iResult)
:	fRefcon(iRefcon)
,	fResult(iResult)
	{}

QueryResult::QueryResult(int64 iRefcon,
	const ZP<Result>& iResult,
	const ZP<ResultDeltas>& iResultDeltas)
:	fRefcon(iRefcon)
,	fResult(iResult)
,	fResultDeltas(iResultDeltas)
	{}


int64 QueryResult::GetRefcon() const
	{ return fRefcon; }

ZP<Result> QueryResult::GetResult() const
	{ return fResult; }

ZP<ResultDeltas> QueryResult::GetResultDeltas() const
	{ return fResultDeltas; }

// =================================================================================================
#pragma mark - Relater

Relater::Relater()
	{}

Relater::~Relater()
	{}

void Relater::SetCallable_RelaterResultsAvailable(ZP<Callable_RelaterResultsAvailable> iCallable)
	{
	ZAcqMtx acq(fMtx);
	fCalled_RelaterResultsAvailable.Reset();
	fCallable_RelaterResultsAvailable = iCallable;
	}

void Relater::pCalled_RelaterCollectResults()
	{
	ZAcqMtx acq(fMtx);
	fCalled_RelaterResultsAvailable.Reset();
	}

void Relater::pTrigger_RelaterResultsAvailable()
	{
	ZAcqMtx acq(fMtx);
	if (not fCalled_RelaterResultsAvailable())
		{
		if (ZP<Callable_RelaterResultsAvailable> theCallable = fCallable_RelaterResultsAvailable)
			{
			ZRelMtx rel(fMtx);
			theCallable->Call(this);
			}
		}
	}

} // namespace Dataspace
} // namespace ZooLib
