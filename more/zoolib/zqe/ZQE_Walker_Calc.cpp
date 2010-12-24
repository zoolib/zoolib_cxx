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

#include "zoolib/zqe/ZQE_Walker_Calc.h"

namespace ZooLib {
namespace ZQE {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Calc

Walker_Calc::Walker_Calc(ZRef<Walker> iWalker,
	const string8& iRelName, ZRef<Callable> iCallable)
:	Walker_Unary(iWalker)
,	fRelName(iRelName)
,	fCallable(iCallable)
	{
	//## Need better API on the callable, so it can look up data in bindings (and output?)
	}

Walker_Calc::~Walker_Calc()
	{}

void Walker_Calc::Prime(const map<string8,size_t>& iBindingOffsets, 
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fOutputOffset = ioBaseOffset++;
	oOffsets[fRelName] = fOutputOffset;

	fBindingOffsets = iBindingOffsets;
	fWalker->Prime(iBindingOffsets, fChildOffsets, ioBaseOffset);
	oOffsets.insert(fChildOffsets.begin(), fChildOffsets.end());
	}

bool Walker_Calc::ReadInc(const ZVal_Any* iBindings,
	ZVal_Any* oResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	if (!fWalker->ReadInc(iBindings, oResults, oAnnotations))
		return false;

	ZMap_Any theMap;
	for (map<string8,size_t>::iterator i = fBindingOffsets.begin(); i != fBindingOffsets.end(); ++i)
		theMap.Set(i->first, iBindings[i->second]);
	for (map<string8,size_t>::iterator i = fChildOffsets.begin(); i != fChildOffsets.end(); ++i)
		theMap.Set(i->first, oResults[i->second]);

	oResults[fOutputOffset] = fCallable->Call(theMap);
	return true;
	}

} // namespace ZQE
} // namespace ZooLib
