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

#include "zoolib/QueryEngine/Walker_Dee.h"
#include "zoolib/QueryEngine/Walker_Product.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark Walker_Product

Walker_Product::Walker_Product(const ZRef<Walker>& iWalker_Left, const ZRef<Walker>& iWalker_Right)
:	fWalker_Left(iWalker_Left)
,	fWalker_Right(iWalker_Right)
,	fNeedLoadLeft(true)
	{}

Walker_Product::~Walker_Product()
	{}

void Walker_Product::Rewind()
	{
	fWalker_Left->Rewind();
	fNeedLoadLeft = true;
	}

ZRef<Walker> Walker_Product::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fWalker_Left = fWalker_Left->Prime(iOffsets, fLeftOffsets, ioBaseOffset);

	fResults_Left.resize(ioBaseOffset);
	oOffsets.insert(fLeftOffsets.begin(), fLeftOffsets.end());

	map<string8,size_t> combined = iOffsets;
	combined.insert(fLeftOffsets.begin(), fLeftOffsets.end());

	fWalker_Right = fWalker_Right->Prime(combined, oOffsets, ioBaseOffset);

	if (not fWalker_Left || not fWalker_Right)
		return null;

	if (fWalker_Left.DynamicCast<Walker_Dee>())
		return fWalker_Right;
	else if (fWalker_Right.DynamicCast<Walker_Dee>())
		return fWalker_Left;

	return this;
	}

bool Walker_Product::QReadInc(Val_Any* ioResults)
	{
	for (;;)
		{
		if (fNeedLoadLeft)
			{
			fWalker_Right->Rewind();
			fNeedLoadLeft = false;

			if (not fWalker_Left->QReadInc(ioResults))
				return false;

			std::copy_n(ioResults, fResults_Left.size(), fResults_Left.begin());
			}
		else
			{
			std::copy(fResults_Left.begin(), fResults_Left.end(), ioResults);
			}

		if (fWalker_Right->QReadInc(ioResults))
			return true;

		fNeedLoadLeft = true;
		}
	}

} // namespace QueryEngine
} // namespace ZooLib
