// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Result.h"

#include "zoolib/Compare_vector.h"

using std::map;
using std::pair;
using std::vector;

namespace ZooLib {

// =================================================================================================
#pragma mark - QueryEngine::Result

namespace QueryEngine {

using RelationalAlgebra::RelHead;

Result::Result(const Result& iOther)
:	fRelHead(iOther.fRelHead)
,	fPackedRows(iOther.fPackedRows)
	{}

Result::Result(const RelationalAlgebra::RelHead& iRelHead)
:	fRelHead(iRelHead)
	{}

Result::Result(RelHead* ioRelHead,
	vector<Val_DB>* ioPackedRows)
	{
	ioRelHead->swap(fRelHead);
	ioPackedRows->swap(fPackedRows);
	}

Result::Result(const RelHead& iRelHead,
	vector<Val_DB>* ioPackedRows)
:	fRelHead(iRelHead)
	{
	ioPackedRows->swap(fPackedRows);
	}

Result::Result(const ZP<Result>& iOther, size_t iRow)
:	fRelHead(iOther->GetRelHead())
	{
	if (iRow < iOther->Count())
		{
		const Val_DB* theVals = iOther->GetValsAt(iRow);
		fPackedRows.insert(fPackedRows.end(), theVals, theVals + fRelHead.size());
		}
	}

Result::~Result()
	{}

const RelHead& Result::GetRelHead()
	{ return fRelHead; }

size_t Result::Count()
	{
	if (const size_t theSize = fRelHead.size())
		return fPackedRows.size() / theSize;
	return 0;
	}

const Val_DB* Result::GetValsAt(size_t iIndex)
	{
	const size_t theOffset = fRelHead.size() * iIndex;
	ZAssert(theOffset < fPackedRows.size() + fRelHead.size());
	return &fPackedRows[theOffset];
	}

int Result::Compare(const ZP<Result>& iOther) const
	{
	if (int compare = sCompare_T(fRelHead, iOther->fRelHead))
		return compare;
	return sCompare_T(fPackedRows, iOther->fPackedRows);
	}

ZP<Result> Result::Fresh()
	{ return new Result(*this); }

// =================================================================================================
#pragma mark - ResultDeltas

ResultDeltas::ResultDeltas()
	{}

ResultDeltas::~ResultDeltas()
	{}

// =================================================================================================
#pragma mark - sApplyDeltas

ZP<Result> sApplyDeltas(ZP<Result> iResult, ZP<ResultDeltas> iResultDeltas)
	{
	if (not iResult || not iResultDeltas || not iResultDeltas->fMapping.size())
		return iResult;

	ZP<Result> result = iResult->Fresh();

	const size_t theColCount = result->GetRelHead().size();
	for (size_t xx = 0; xx < iResultDeltas->fMapping.size(); ++xx)
		{
		const size_t target = iResultDeltas->fMapping[xx];

		std::copy_n(&iResultDeltas->fPackedRows[xx * theColCount],
			theColCount,
			&result->fPackedRows[target * theColCount]);
		}

	return result;
	}

} // namespace QueryEngine
} // namespace ZooLib
