/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/QueryEngine/Walker_Union.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark - Walker_Union

Walker_Union::Walker_Union(const ZRef<Walker>& iWalker_Left, const ZRef<Walker>& iWalker_Right)
:	fWalker_Left(iWalker_Left)
,	fExhaustedLeft(false)
,	fWalker_Right(iWalker_Right)
	{}

Walker_Union::~Walker_Union()
	{}

void Walker_Union::Rewind()
	{
	this->Called_Rewind();
	fExhaustedLeft = false;
	fWalker_Left->Rewind();
	fWalker_Right->Rewind();
	fPriors.clear();
	}

ZRef<Walker> Walker_Union::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	map<string8,size_t> leftOffsets;
	fWalker_Left = fWalker_Left->Prime(iOffsets, leftOffsets, ioBaseOffset);

	if (not fWalker_Left)
		return fWalker_Right->Prime(iOffsets, oOffsets, ioBaseOffset);

	map<string8,size_t> rightOffsets;
	fWalker_Right = fWalker_Right->Prime(iOffsets, rightOffsets, ioBaseOffset);

	oOffsets.insert(leftOffsets.begin(), leftOffsets.end());
	if (not fWalker_Right)
		return fWalker_Left;

	size_t count = leftOffsets.size();
	ZAssert(count == rightOffsets.size());

	map<string8,size_t>::const_iterator iterLeft = leftOffsets.begin();
	map<string8,size_t>::const_iterator iterRight = rightOffsets.begin();

	fMapping_Left.resize(count);
	vector<size_t>::iterator iterMappingLeft = fMapping_Left.begin();

	fMapping_Right.resize(count);
	vector<size_t>::iterator iterMappingRight = fMapping_Right.begin();

	while (count--)
		{
		ZAssert(iterLeft->first == iterRight->first);

		*iterMappingLeft++ = (iterLeft++)->second;
		*iterMappingRight++ = (iterRight++)->second;
		}

	return this;
	}

bool Walker_Union::QReadInc(Val_Any* ioResults)
	{
	this->Called_QReadInc();

	const size_t count = fMapping_Left.size();

	for (;;)
		{
		vector<Val_Any> subset;
		subset.reserve(count);
		if (not fExhaustedLeft)
			{
			if (fWalker_Left->QReadInc(ioResults))
				{
				for (size_t xx = 0; xx < count; ++xx)
					subset.push_back(ioResults[fMapping_Left[xx]]);

				Util_STL::sInsertMust(fPriors, subset);
				return true;
				}
			fExhaustedLeft = true;
			}

		if (not fWalker_Right->QReadInc(ioResults))
			return false;

		for (size_t xx = 0; xx < count; ++xx)
			subset.push_back(ioResults[fMapping_Right[xx]]);

		if (not Util_STL::sContains(fPriors, subset))
			{
			for (size_t xx = 0; xx < count; ++xx)
				ioResults[fMapping_Left[xx]] = subset[xx];
			return true;
			}
		}
	}

} // namespace QueryEngine
} // namespace ZooLib
