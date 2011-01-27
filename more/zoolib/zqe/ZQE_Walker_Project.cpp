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

#include "zoolib/zqe/ZQE_Walker_Project.h"

namespace ZooLib {
namespace ZQE {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Project

Walker_Project::Walker_Project(const ZRef<Walker>& iWalker, const ZRA::RelHead& iRelHead)
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

	for (ZRA::RelHead::iterator i = fRelHead.begin(); i != fRelHead.end(); ++i)
		{
		const size_t childOffset = childOffsets[*i];
		fChildMapping.push_back(childOffset);
		oOffsets[*i] = childOffset;
		}
	return this;
	}

bool Walker_Project::ReadInc(
	ZVal_Any* ioResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	set<ZRef<ZCounted> > localAnnotations;
	set<ZRef<ZCounted> >* localAnnotationsPtr = nullptr;
	if (oAnnotations)
		localAnnotationsPtr = &localAnnotations;

	for (;;)
		{
		if (!fWalker->ReadInc(ioResults, localAnnotationsPtr))
			return false;

		const size_t count = fRelHead.size();
		vector<ZVal_Any> subset;
		subset.reserve(count);
		for (size_t x = 0; x < count; ++x)
			subset.push_back(ioResults[fChildMapping[x]]);

		if (ZUtil_STL::sInsertIfNotContains(fPriors, subset))
			{
			if (oAnnotations)
				oAnnotations->insert(localAnnotations.begin(), localAnnotations.end());
			return true;
			}
		localAnnotations.clear();
		}
	}

} // namespace ZQE
} // namespace ZooLib
