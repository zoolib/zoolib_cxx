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

#include "zoolib/QueryEngine/Walker_Result.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;

// =================================================================================================
#pragma mark - Walker_Result

Walker_Result::Walker_Result(ZRef<Result> iResult)
:	fResult(iResult)
,	fIndex(0)
	{}

Walker_Result::~Walker_Result()
	{}

void Walker_Result::Rewind()
	{
	this->Called_Rewind();
	fIndex = 0;
	}

ZRef<Walker> Walker_Result::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fBaseOffset = ioBaseOffset;
	foreachi (ii, fResult->GetRelHead())
		oOffsets[*ii] = ioBaseOffset++;
	return this;
	}

bool Walker_Result::QReadInc(Val_Any* oResults)
	{
	this->Called_QReadInc();

	if (fIndex >= fResult->Count())
		return false;

	const Val_Any* theVals = fResult->GetValsAt(fIndex);
	std::copy_n(theVals, fResult->GetRelHead().size(), oResults + fBaseOffset);

	++fIndex;

	return true;
	}

} // namespace QueryEngine
} // namespace ZooLib
