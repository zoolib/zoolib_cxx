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

#include "zoolib/zqe/ZQE_Walker_Rename.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Rename

Walker_Rename::Walker_Rename(ZRef<Walker> iWalker, const string8& iNew, const string8& iOld)
:	fWalker(iWalker)
,	fNew(iNew)
,	fOld(iOld)
	{}

Walker_Rename::~Walker_Rename()
	{}

size_t Walker_Rename::NameCount()
	{ return fWalker->NameCount(); }

string8 Walker_Rename::NameAt(size_t iIndex)
	{ return fWalker->NameAt(iIndex); }

ZRef<Walker> Walker_Rename::Clone()
	{ return new Walker_Rename(fWalker->Clone(), fNew, fOld); }

ZRef<Row> Walker_Rename::ReadInc(ZMap_Any iBindings)
	{
	for (ZRef<Row> theRow; theRow = fWalker->ReadInc(iBindings); /*no inc*/)
		{
//		ZMap_Any theBindings = iBindings;
//		for (size_t x = 0, count = fWalker->NameCount(); x < count; ++x)
//			theBindings.Set(fWalker->NameAt(x), theRow->Get(x));
//		if (sMatches(fExpr_Bool, theBindings))
			return theRow;
		}
	return null;
	}

} // namespace ZQE
} // namespace ZooLib
