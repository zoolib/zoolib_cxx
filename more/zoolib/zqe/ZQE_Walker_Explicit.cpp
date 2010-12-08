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

#include "zoolib/zqe/ZQE_Walker_Explicit.h"

namespace ZooLib {
namespace ZQE {

using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Explicit

Walker_Explicit::Walker_Explicit(const vector<string8>& iNames, const ZRef<Row>& iRow)
:	fNames(iNames)
,	fRow(iRow)
	{}

Walker_Explicit::Walker_Explicit(const ZMap_Any& iMap)
	{
	vector<ZVal_Any> theVals;
	for (ZMap_Any::Index_t i = iMap.Begin(); i != iMap.End(); ++i)
		{
		fNames.push_back(iMap.NameOf(i));
		theVals.push_back(iMap.Get(i));
		}
	fRow = new Row_Vector(&theVals);
	}

Walker_Explicit::~Walker_Explicit()
	{}

size_t Walker_Explicit::NameCount()
	{ return fNames.size(); }

string8 Walker_Explicit::NameAt(size_t iIndex)
	{
	if (iIndex < fNames.size())
		return fNames.at(iIndex);
	return string8();
	}

ZRef<Walker> Walker_Explicit::Clone()
	{ return new Walker_Explicit(fNames, fRow); }

ZRef<Row> Walker_Explicit::ReadInc()
	{
	ZRef<Row> result;
	result.swap(fRow);
	return result;
	}

} // namespace ZQE
} // namespace ZooLib
