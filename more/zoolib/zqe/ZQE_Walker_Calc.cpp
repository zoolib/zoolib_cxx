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

#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/zqe/ZQE_Walker_Calc.h"

namespace ZooLib {
namespace ZQE {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Calc

Walker_Calc::Walker_Calc(const string8& iRelName,
	const ZRA::Rename& iBindings,
	const ZRef<Callable>& iCallable)
:	fRelName(iRelName)
,	fBindings(iBindings)
,	fCallable(iCallable)
	{
	//## Need better API on the callable, so it can look up data in bindings (and output?)
	}

Walker_Calc::~Walker_Calc()
	{}

ZRef<Walker> Walker_Calc::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	for (map<string8,size_t>::const_iterator i = iOffsets.begin(); i != iOffsets.end(); ++i)
		{
		if (ZUtil_STL::sContains(fBindings, i->first))
			fBindingOffsets.insert(*i);
		}

	fOutputOffset = ioBaseOffset++;
	oOffsets[fRelName] = fOutputOffset;
	return this;
	}

bool Walker_Calc::ReadInc(
	ZVal_Any* ioResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	if (fExhausted)
		return false;
	fExhausted = true;

	// Need to make use of fBindings to build this.
	ZMap_Any theMap;
	for (map<string8,size_t>::iterator i = fBindingOffsets.begin(); i != fBindingOffsets.end(); ++i)
		theMap.Set(i->first, ioResults[i->second]);

	ioResults[fOutputOffset] = fCallable->Call(theMap);
	return true;
	}

} // namespace ZQE
} // namespace ZooLib
