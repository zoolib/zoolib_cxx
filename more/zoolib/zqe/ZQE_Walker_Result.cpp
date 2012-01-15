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

#include "zoolib/zqe/ZQE_Walker_Result.h"

namespace ZooLib {
namespace ZQE {

using std::map;
using std::set;

// =================================================================================================
// MARK: - Walker_Result

Walker_Result::Walker_Result(ZRef<Result> iResult)
:	fResult(iResult)
,	fIndex(0)
	{}

Walker_Result::~Walker_Result()
	{}

void Walker_Result::Rewind()
	{ fIndex = 0; }

ZRef<Walker> Walker_Result::Prime
	(const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fBaseOffset = ioBaseOffset;
	const ZRA::RelHead& theRH = fResult->GetRelHead();
	for (ZRA::RelHead::const_iterator i = theRH.begin(); i != theRH.end(); ++i)
		oOffsets[*i] = ioBaseOffset++;
	return this;
	}

bool Walker_Result::ReadInc
	(ZVal_Any* oResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	if (fIndex >= fResult->Count())
		return false;

	if (oAnnotations)
		fResult->GetAnnotationsAt(fIndex, *oAnnotations);

	const ZRA::RelHead& theRH = fResult->GetRelHead();
	size_t theOffset = fBaseOffset;
	const ZVal_Any* theVals = fResult->GetValsAt(fIndex);
	for (ZRA::RelHead::const_iterator i = theRH.begin(); i != theRH.end(); ++i)
		oResults[theOffset++] = *theVals++;

	++fIndex;

	return true;
	}

} // namespace ZQE
} // namespace ZooLib
