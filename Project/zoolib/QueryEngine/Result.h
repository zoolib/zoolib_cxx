// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Result_h__
#define __ZooLib_QueryEngine_Result_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"
#include "zoolib/Counted.h"
#include "zoolib/Multi.h"
#include "zoolib/Val_DB.h"

#include "zoolib/RelationalAlgebra/RelHead.h"

#include <map>
#include <set>
#include <vector>

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Result

class Result : public Counted
	{
	Result(const Result& iOther);

public:
	Result(RelationalAlgebra::RelHead* ioRelHead,
		std::vector<Val_DB>* ioPackedRows);

	Result(const RelationalAlgebra::RelHead& iRelHead,
		std::vector<Val_DB>* ioPackedRows);

	Result(const ZP<Result>& iOther, size_t iRow);

	virtual ~Result();

	const RelationalAlgebra::RelHead& GetRelHead();

	size_t Count();
	const Val_DB* GetValsAt(size_t iIndex);

	int Compare(const Result& iOther) const;

	ZP<Result> Fresh();

public:
	RelationalAlgebra::RelHead fRelHead;
	std::vector<Val_DB> fPackedRows;
	};

// =================================================================================================
#pragma mark - ResultDeltas

class ResultDeltas : public Counted
	{
public:
	ResultDeltas();

	virtual ~ResultDeltas();

public:
	std::vector<size_t> fMapping;
	std::vector<Val_DB> fPackedRows;
	};

// =================================================================================================
#pragma mark - ResultDiffer

class ResultDiffer
	{
public:
	ResultDiffer(const RelationalAlgebra::RelHead& iIdentity,
		const RelationalAlgebra::RelHead& iSignificant,
		bool iEmitDummyChanges = false);

	void Apply(const ZP<Result>& iResult,
		ZP<Result>* oPriorResult,
		const ZP<ResultDeltas>& iResultDeltas,
		ZP<Result>* oCurResult,
		std::vector<size_t>* oRemoved,
		std::vector<std::pair<size_t,size_t>>* oAdded,
		std::vector<Multi3<size_t,size_t,size_t>>* oChanged);

private:
	const RelationalAlgebra::RelHead fIdentity;
	const RelationalAlgebra::RelHead fSignificant;
	const bool fEmitDummyChanges;

	ZP<Result> fResult_Prior;
	std::vector<size_t> fSort_Prior;

	std::vector<size_t> fPermute;
	};

// =================================================================================================
#pragma mark - sBuildBindings

void sBuildBindings(ZP<Result> iResult, std::map<string8,size_t>& oResult);

} // namespace QueryEngine

// =================================================================================================
#pragma mark - sCompare_T

template <>
int sCompare_T<QueryEngine::Result>(
	const QueryEngine::Result& iL, const QueryEngine::Result& iR);

template <>
int sCompare_T<ZP<QueryEngine::Result>>(
	const ZP<QueryEngine::Result>& iL, const ZP<QueryEngine::Result>& iR);

} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Result_h__
