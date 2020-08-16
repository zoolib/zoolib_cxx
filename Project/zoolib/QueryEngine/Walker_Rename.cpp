// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Walker_Rename.h"

#include "zoolib/Util_STL_map.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;

using namespace Util_STL;

// =================================================================================================
#pragma mark - Walker_Rename

Walker_Rename::Walker_Rename(const ZP<Walker>& iWalker, const string8& iNew, const string8& iOld)
:	Walker_Unary(iWalker)
,	fNew(iNew)
,	fOld(iOld)
	{}

Walker_Rename::~Walker_Rename()
	{}

ZP<Walker> Walker_Rename::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	// Confusion will result if the name we're going to be our new name is already in the list
	ZAssert(not sContains(iOffsets, fNew));

	map<string8,size_t> childOffsets;
	fWalker = fWalker->Prime(iOffsets, childOffsets, ioBaseOffset);

	sInsertMust(childOffsets, fNew, sGetEraseMust(childOffsets, fOld));

	oOffsets.insert(childOffsets.begin(), childOffsets.end());

	return this;
	}

bool Walker_Rename::QReadInc(Val_DB* ioResults)
	{
	this->Called_QReadInc();
	return fWalker->QReadInc(ioResults);
	}

} // namespace QueryEngine
} // namespace ZooLib
