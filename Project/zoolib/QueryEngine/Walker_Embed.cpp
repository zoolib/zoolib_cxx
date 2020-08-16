// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_STL_map.h"

#include "zoolib/QueryEngine/Result.h"
#include "zoolib/QueryEngine/Walker_Embed.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;

using namespace Util_STL;

using RelationalAlgebra::RelHead;

// =================================================================================================
#pragma mark - Walker_Embed

Walker_Embed::Walker_Embed(const ZP<Walker>& iWalker_Parent, const RelHead& iBoundNames,
	const string8& iColName, const ZP<Walker>& iWalker_Embedee)
:	fWalker_Parent(iWalker_Parent)
,	fBoundNames(iBoundNames)
,	fColName(iColName)
,	fWalker_Embedee(iWalker_Embedee)
	{}

Walker_Embed::~Walker_Embed()
	{}

void Walker_Embed::Rewind()
	{
	this->Called_Rewind();
	fWalker_Parent->Rewind();
	}

ZP<Walker> Walker_Embed::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fWalker_Parent = fWalker_Parent->Prime(iOffsets, oOffsets, ioBaseOffset);
	if (not fWalker_Parent)
		return null;

	map<string8,size_t> embedeeOffsets;
	fWalker_Embedee = fWalker_Embedee->Prime(oOffsets, embedeeOffsets, ioBaseOffset);

	foreacha (entry, embedeeOffsets)
		{
		fEmbedeeRelHead |= entry.first;
		fEmbedeeOffsets.push_back(entry.second);
		}

	fOutputOffset = ioBaseOffset++;
	oOffsets[fColName] = fOutputOffset;

	return this;
	}

bool Walker_Embed::QReadInc(Val_DB* ioResults)
	{
	this->Called_QReadInc();

	if (not fWalker_Parent->QReadInc(ioResults))
		return false;

	if (fWalker_Embedee)
		{
		fWalker_Embedee->Rewind();

		vector<Val_DB> thePackedRows;
		for (;;)
			{
			if (not fWalker_Embedee->QReadInc(ioResults))
				break;

			foreacha (entry, fEmbedeeOffsets)
				thePackedRows.push_back(ioResults[entry]);
			}

		ZP<Result> theResult = new Result(fEmbedeeRelHead, &thePackedRows);
		ioResults[fOutputOffset] = theResult;
		}

	return true;
	}

} // namespace QueryEngine
} // namespace ZooLib
