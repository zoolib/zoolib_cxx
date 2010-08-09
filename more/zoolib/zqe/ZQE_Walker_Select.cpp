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

#include "zoolib/zqe/ZQE_Walker_Select.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Select

Walker_Select::Walker_Select(ZRef<Walker> iWalker, ZRef<ZExpr_Logic> iExpr_Logic)
:	fWalker(iWalker)
,	fExpr_Logic(iExpr_Logic)
	{}

Walker_Select::~Walker_Select()
	{}

size_t Walker_Select::Count()
	{ return fWalker->Count(); }

string8 Walker_Select::NameOf(size_t iIndex)
	{ return fWalker->NameOf(iIndex); }

ZRef<Walker> Walker_Select::Clone()
	{ return new Walker_Select(fWalker->Clone(), fExpr_Logic); }

ZRef<Row> Walker_Select::ReadInc()
	{
	for (;;)
		{
		if (ZRef<Row> theRow = fWalker->ReadInc())
			{
			if (true)
				{
				ZUnimplemented();
				// Check that theRow matches fExpr_Logic;
				return theRow;
				}
			continue;
			}
		return null;
		}
	}

} // namespace ZQE
} // namespace ZooLib
