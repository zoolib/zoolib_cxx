// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/ResultFromWalker.h"

#include "zoolib/Default.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;
using RelationalAlgebra::RelHead;

// =================================================================================================
#pragma mark - sQuery

ZP<Result> sResultFromWalker(ZP<Walker> iWalker)
	{
	map<string8,size_t> offsets;
	size_t baseOffset = 0;
	iWalker = iWalker->Prime(sDefault(), offsets, baseOffset);

	vector<Val_DB> thePackedRows;
	vector<Val_DB> theRow(baseOffset);
	for (;;)
		{
		if (not iWalker->QReadInc(&theRow[0]))
			break;

		foreacha (entry, offsets)
			thePackedRows.push_back(theRow[entry.second]);
		}

	RelHead theRelHead;
	foreacha (entry, offsets)
		theRelHead.insert(entry.first);

	return new Result(&theRelHead, &thePackedRows);
	}

} // namespace QueryEngine
} // namespace ZooLib
