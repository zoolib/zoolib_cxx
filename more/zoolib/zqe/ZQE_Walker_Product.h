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

#ifndef __ZQE_Walker_Product__
#define __ZQE_Walker_Product__ 1
#include "zconfig.h"

#include "zoolib/zqe/ZQE_Walker.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Product

class Walker_Product : public Walker
	{
	Walker_Product(ZRef<Walker> iWalker_Left, ZRef<Walker> iWalker_Right,
		ZRef<Walker> iRight_Model, ZRef<Row> iRow_Left);

public:
	Walker_Product(ZRef<Walker> iWalker_Left, ZRef<Walker> iWalker_Right_Model);
	virtual ~Walker_Product();

// From ZQE::Walker
	virtual size_t NameCount();
	virtual string8 NameAt(size_t iIndex);

	virtual ZRef<Walker> Clone();
	virtual ZRef<Row> ReadInc(ZMap_Any iBindings);

private:
	ZRef<Walker> fWalker_Left;
	ZRef<Walker> fWalker_Right;
	ZRef<Walker> fWalker_Right_Model;
	
	ZRef<Row> fRow_Left;
	};

} // namespace ZQE
} // namespace ZooLib

#endif // __ZQE_Walker_Product__
