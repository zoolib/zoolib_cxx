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

#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/zqe/ZQE_Result.h"
#include "zoolib/zqe/ZQE_Walker_Embed.h"

namespace ZooLib {
namespace ZQE {

using std::map;
using std::set;
using std::vector;

using namespace ZUtil_STL;

// =================================================================================================
// MARK: - Walker_Embed

Walker_Embed::Walker_Embed(const ZRef<Walker>& iWalker_Parent,
		const string8& iRelName, const ZRef<Walker>& iWalker_Embedee)
:	fWalker_Parent(iWalker_Parent)
,	fRelName(iRelName)
,	fWalker_Embedee(iWalker_Embedee)
	{}

Walker_Embed::~Walker_Embed()
	{}

void Walker_Embed::Rewind()
	{ fWalker_Parent->Rewind(); }

ZRef<Walker> Walker_Embed::Prime
	(const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fWalker_Parent = fWalker_Parent->Prime(iOffsets, oOffsets, ioBaseOffset);
	if (not fWalker_Parent)
		return null;

	map<string8,size_t> embedeeOffsets;
	fWalker_Embedee = fWalker_Embedee->Prime(oOffsets, embedeeOffsets, ioBaseOffset);

	for (map<string8,size_t>::iterator i = embedeeOffsets.begin(); i != embedeeOffsets.end(); ++i)
		{
		fEmbedeeRelHead |= i->first;
		fEmbedeeOffsets.push_back(i->second);
		}

	fOutputOffset = ioBaseOffset++;
	oOffsets[fRelName] = fOutputOffset;

	return this;
	}

bool Walker_Embed::ReadInc
	(ZVal_Any* ioResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	if (not fWalker_Parent->ReadInc(ioResults, oAnnotations))
		return false;

	if (fWalker_Embedee)
		{
		fWalker_Embedee->Rewind();

		vector<ZVal_Any> thePackedRows;
		for (;;)
			{
			if (not fWalker_Embedee->ReadInc(ioResults, nullptr))
				break;

			for (vector<size_t>::iterator i = fEmbedeeOffsets.begin(); i != fEmbedeeOffsets.end(); ++i)
				thePackedRows.push_back(ioResults[*i]);
			}

		ZRef<ZQE::Result> theResult = new ZQE::Result(fEmbedeeRelHead, &thePackedRows, nullptr);
		ioResults[fOutputOffset] = theResult;
		}

	return true;
	}

} // namespace ZQE
} // namespace ZooLib
