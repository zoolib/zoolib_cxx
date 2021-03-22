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
	Result(const RelationalAlgebra::RelHead& iRelHead);

	Result(RelationalAlgebra::RelHead* ioRelHead,
		std::vector<Val_DB>* ioPackedRows);

	Result(const RelationalAlgebra::RelHead& iRelHead,
		std::vector<Val_DB>* ioPackedRows);

	Result(const ZP<Result>& iOther, size_t iRow);

	virtual ~Result();

	const RelationalAlgebra::RelHead& GetRelHead();

	size_t Count();
	const Val_DB* GetValsAt(size_t iIndex);

	int Compare(const ZP<Result>& iOther) const;

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
#pragma mark - sApplyDeltas

ZP<Result> sApplyDeltas(ZP<Result> iResult, ZP<ResultDeltas> iResultDeltas);

} // namespace QueryEngine

template <>
inline int sCompare_T(const ZP<QueryEngine::Result>& iL, const ZP<QueryEngine::Result>& iR)
	{ return iL->Compare(iR); }

} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Result_h__
