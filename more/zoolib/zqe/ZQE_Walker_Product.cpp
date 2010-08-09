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

#include "zoolib/zqe/ZQE_Walker_Product.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Product

Walker_Product::Walker_Product(ZRef<Walker> iWalker_Left, ZRef<Walker> iWalker_Right,
	ZRef<Walker> iWalker_Right_Model, ZRef<Row> iRow_Left)
:	fWalker_Left(iWalker_Left)
,	fWalker_Right(iWalker_Right)
,	fWalker_Right_Model(iWalker_Right_Model)
,	fRow_Left(iRow_Left)
	{}

Walker_Product::Walker_Product(ZRef<Walker> iWalker_Left, ZRef<Walker> iWalker_Right_Model)
:	fWalker_Left(iWalker_Left)
,	fWalker_Right_Model(iWalker_Right_Model)
	{}

Walker_Product::~Walker_Product()
	{}

size_t Walker_Product::Count()
	{ return fWalker_Left->Count() + fWalker_Right_Model->Count(); }

string8 Walker_Product::NameOf(size_t iIndex)
	{
	const size_t leftCount = fWalker_Left->Count();
	if (iIndex >= leftCount)
		return fWalker_Right_Model->NameOf(iIndex - leftCount);
	else
		return fWalker_Left->NameOf(iIndex);
	}

ZRef<Walker> Walker_Product::Clone()
	{
	if (fWalker_Right)
		{
		return new Walker_Product(fWalker_Left->Clone(), fWalker_Right->Clone(),
			fWalker_Right_Model->Clone(), fRow_Left);
		}
	else
		{
		return new Walker_Product(fWalker_Left->Clone(), fWalker_Right_Model->Clone());
		}
	}

ZRef<Row> Walker_Product::ReadInc()
	{
	for (;;)
		{
		if (!fWalker_Right)
			{
			fWalker_Right = fWalker_Right_Model->Clone();
			fRow_Left = fWalker_Left->ReadInc();
			if (!fRow_Left)
				return null;			
			}

		if (ZRef<Row> theRow_Right = fWalker_Right->ReadInc())
			return new Row_Pair(fRow_Left, theRow_Right);

		fWalker_Right.Clear();
		}
	}

} // namespace ZQE
} // namespace ZooLib
