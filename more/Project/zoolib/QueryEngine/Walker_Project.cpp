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

#include "zoolib/Util_STL_set.h"

#include "zoolib/QueryEngine/Walker_Project.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
// MARK: - Walker_Project

Walker_Project::Walker_Project(const ZRef<Walker>& iWalker, const RelationalAlgebra::RelHead& iRelHead)
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

ZRef<Walker> Walker_Project::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	map<string8,size_t> childOffsets;
	fWalker = fWalker->Prime(iOffsets, childOffsets, ioBaseOffset);

	foreachi (ii, fRelHead)
		{
		const size_t childOffset = childOffsets[*ii];
		fChildMapping.push_back(childOffset);
		oOffsets[*ii] = childOffset;
		}

	if (not fWalker)
		return null;

	return this;
	}

bool Walker_Project::QReadInc(Val_Any* ioResults)
	{
	const size_t count = fRelHead.size();

	for (;;)
		{
		if (not fWalker->QReadInc(ioResults))
			return false;

		vector<Val_Any> subset;
		subset.reserve(count);
		for (size_t xx = 0; xx < count; ++xx)
			subset.push_back(ioResults[fChildMapping[xx]]);

		if (Util_STL::sQInsert(fPriors, subset))
			return true;
		}
	}

} // namespace QueryEngine
} // namespace ZooLib
