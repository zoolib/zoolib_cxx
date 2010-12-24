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

#include "zoolib/zqe/ZQE_Walker_Const.h"

namespace ZooLib {
namespace ZQE {

using std::map;
using std::set;

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Const

Walker_Const::Walker_Const(ZRef<Walker> iWalker,
	const string8& iRelName, const ZVal_Any& iVal)
:	Walker_Unary(iWalker)
,	fRelName(iRelName)
,	fVal(iVal)
	{}

Walker_Const::~Walker_Const()
	{}

void Walker_Const::Prime(const std::map<string8,size_t>& iBindingOffsets, 
	std::map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fOutputOffset = ioBaseOffset++;
	oOffsets[fRelName] = fOutputOffset;

	fWalker->Prime(iBindingOffsets, oOffsets, ioBaseOffset);
	}

bool Walker_Const::ReadInc(const ZVal_Any* iBindings,
	ZVal_Any* oResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	if (!fWalker->ReadInc(iBindings, oResults, oAnnotations))
		return false;

	oResults[fOutputOffset] = fVal;
	return true;
	}

} // namespace ZQE
} // namespace ZooLib
