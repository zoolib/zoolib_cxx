/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_map.h"

#include "zoolib/QueryEngine/Result.h"
#include "zoolib/QueryEngine/Walker_Embed.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;

using namespace ZUtil_STL;

// =================================================================================================
// MARK: - Walker_Embed

Walker_Embed::Walker_Embed(const ZRef<Walker>& iWalker_Parent,
		const string8& iColName, const ZRef<Walker>& iWalker_Embedee)
:	fWalker_Parent(iWalker_Parent)
,	fColName(iColName)
,	fWalker_Embedee(iWalker_Embedee)
	{}

Walker_Embed::~Walker_Embed()
	{}

void Walker_Embed::Rewind()
	{ fWalker_Parent->Rewind(); }

ZRef<Walker> Walker_Embed::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fWalker_Parent = fWalker_Parent->Prime(iOffsets, oOffsets, ioBaseOffset);
	if (not fWalker_Parent)
		return null;

	map<string8,size_t> embedeeOffsets;
	fWalker_Embedee = fWalker_Embedee->Prime(oOffsets, embedeeOffsets, ioBaseOffset);

	foreachi (ii, embedeeOffsets)
		{
		fEmbedeeRelHead |= ii->first;
		fEmbedeeOffsets.push_back(ii->second);
		}

	fOutputOffset = ioBaseOffset++;
	oOffsets[fColName] = fOutputOffset;

	return this;
	}

bool Walker_Embed::QReadInc(
	ZVal_Any* ioResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	if (not fWalker_Parent->QReadInc(ioResults, oAnnotations))
		return false;

	if (fWalker_Embedee)
		{
		fWalker_Embedee->Rewind();

		vector<ZVal_Any> thePackedRows;
		for (;;)
			{
			if (not fWalker_Embedee->QReadInc(ioResults, nullptr))
				break;

			foreachi (ii, fEmbedeeOffsets)
				thePackedRows.push_back(ioResults[*ii]);
			}

		ZRef<QueryEngine::Result> theResult = new QueryEngine::Result(fEmbedeeRelHead, &thePackedRows, nullptr);
		ioResults[fOutputOffset] = theResult;
		}

	return true;
	}

} // namespace QueryEngine
} // namespace ZooLib
