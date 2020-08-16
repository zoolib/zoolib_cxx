// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Walker_Result.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;

// =================================================================================================
#pragma mark - Walker_Result

Walker_Result::Walker_Result(ZP<Result> iResult)
:	fResult(iResult)
,	fIndex(0)
	{}

Walker_Result::~Walker_Result()
	{}

void Walker_Result::Rewind()
	{
	this->Called_Rewind();
	fIndex = 0;
	}

ZP<Walker> Walker_Result::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fBaseOffset = ioBaseOffset;
	foreacha (entry, fResult->GetRelHead())
		oOffsets[entry] = ioBaseOffset++;
	return this;
	}

bool Walker_Result::QReadInc(Val_DB* oResults)
	{
	this->Called_QReadInc();

	if (fIndex >= fResult->Count())
		return false;

	const Val_DB* theVals = fResult->GetValsAt(fIndex);
	std::copy_n(theVals, fResult->GetRelHead().size(), oResults + fBaseOffset);

	++fIndex;

	return true;
	}

} // namespace QueryEngine
} // namespace ZooLib
