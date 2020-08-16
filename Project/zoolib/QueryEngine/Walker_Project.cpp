// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Walker_Project.h"

#include "zoolib/Util_STL_set.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark - Walker_Project

Walker_Project::Walker_Project(const ZP<Walker>& iWalker, const RelationalAlgebra::RelHead& iRelHead)
:	Walker_Unary(iWalker)
,	fRelHead(iRelHead)
	{}

Walker_Project::~Walker_Project()
	{}

void Walker_Project::Rewind()
	{
	Walker_Unary::Rewind();
	fPriors.clear();
	}

ZP<Walker> Walker_Project::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	map<string8,size_t> childOffsets;
	fWalker = fWalker->Prime(iOffsets, childOffsets, ioBaseOffset);

	foreacha (entry, fRelHead)
		{
		const size_t childOffset = childOffsets[entry];
		fChildMapping.push_back(childOffset);
		oOffsets[entry] = childOffset;
		}

	if (not fWalker)
		return null;

	return this;
	}

bool Walker_Project::QReadInc(Val_DB* ioResults)
	{
	this->Called_QReadInc();

	const size_t count = fRelHead.size();

	for (;;)
		{
		if (not fWalker->QReadInc(ioResults))
			return false;

		vector<Val_DB> subset;
		subset.reserve(count);
		for (size_t xx = 0; xx < count; ++xx)
			subset.push_back(ioResults[fChildMapping[xx]]);

		if (Util_STL::sQInsert(fPriors, subset))
			return true;
		}
	}

} // namespace QueryEngine
} // namespace ZooLib
