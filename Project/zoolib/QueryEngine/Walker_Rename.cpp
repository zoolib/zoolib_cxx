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

#include "zoolib/QueryEngine/Walker_Rename.h"

#include "zoolib/Util_STL_map.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;

using namespace Util_STL;

// =================================================================================================
#pragma mark - Walker_Rename

Walker_Rename::Walker_Rename(const ZRef<Walker>& iWalker, const string8& iNew, const string8& iOld)
:	Walker_Unary(iWalker)
,	fNew(iNew)
,	fOld(iOld)
	{}

Walker_Rename::~Walker_Rename()
	{}

ZRef<Walker> Walker_Rename::Prime(
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

bool Walker_Rename::QReadInc(Val_Any* ioResults)
	{
	this->Called_QReadInc();
	return fWalker->QReadInc(ioResults);
	}

} // namespace QueryEngine
} // namespace ZooLib
