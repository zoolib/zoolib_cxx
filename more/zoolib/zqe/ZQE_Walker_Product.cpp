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

#include "zoolib/zqe/ZQE_Walker_Product.h"

namespace ZooLib {
namespace ZQE {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Product

Walker_Product::Walker_Product(ZRef<Walker> iWalker_Left, ZRef<Walker> iWalker_Right)
:	fWalker_Left(iWalker_Left)
,	fWalker_Right(iWalker_Right)
,	fNeedLoadLeft(true)
	{}

Walker_Product::~Walker_Product()
	{}

void Walker_Product::Rewind()
	{
	fWalker_Left->Rewind();
	fWalker_Right->Rewind();
	fAnnotations_Left.clear();
	fNeedLoadLeft = true;
	}

void Walker_Product::Prime(const std::map<string8,size_t>& iBindingOffsets, 
	std::map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fBaseOffset = ioBaseOffset;

	map<string8,size_t> leftOffsets;
	size_t leftWidth = 0;
	fWalker_Left->Prime(map<string8,size_t>(), leftOffsets, leftWidth);
	for (map<string8,size_t>::iterator i = leftOffsets.begin(); i != leftOffsets.end(); ++i)
		oOffsets[i->first] = fBaseOffset + i->second;
	ioBaseOffset += leftWidth;
	fResults_Left.resize(leftWidth);

	fWalker_Right->Prime(map<string8,size_t>(), oOffsets, ioBaseOffset);
	}

bool Walker_Product::ReadInc(const ZVal_Any* iBindings,
	ZVal_Any* oResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	set<ZRef<ZCounted> > localAnnotations;
	set<ZRef<ZCounted> >* localAnnotationsPtr = nullptr;
	if (oAnnotations)
		localAnnotationsPtr = &localAnnotations;

	for (;;)
		{
		if (fNeedLoadLeft)
			{
			fWalker_Right->Rewind();
			fAnnotations_Left.clear();
			fNeedLoadLeft = false;

			if (!fWalker_Left->ReadInc(nullptr, &fResults_Left[0], &fAnnotations_Left))
				return false;
			}

		if (fWalker_Right->ReadInc(nullptr, oResults, localAnnotationsPtr))
			{
			if (oAnnotations)
				{
				oAnnotations->insert(fAnnotations_Left.begin(), fAnnotations_Left.end());
				oAnnotations->insert(localAnnotations.begin(), localAnnotations.end());
				}
			std::copy(fResults_Left.begin(), fResults_Left.end(), &oResults[fBaseOffset]);
			return true;
			}

		fNeedLoadLeft = true;
		localAnnotations.clear();
		}
	}

} // namespace ZQE
} // namespace ZooLib
