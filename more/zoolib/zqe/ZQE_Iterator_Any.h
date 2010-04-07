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

#ifndef __ZQE_Iterator_Any__
#define __ZQE_Iterator_Any__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Logic.h"
#include "zoolib/ZValCondition.h"
#include "zoolib/zqe/ZQE_Iterator.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Restrict

class Iterator_Any_Restrict : public ZQE::Iterator
	{
public:
	Iterator_Any_Restrict(const ZValCondition& iValCondition, ZRef<ZQE::Iterator> iIterator);
	
	virtual ZRef<Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

private:
	ZValCondition fValCondition;
	ZRef<Iterator> fIterator;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Select

class Iterator_Any_Select : public ZQE::Iterator
	{
public:
	Iterator_Any_Select(ZRef<ZExprRep_Logic> iExprRep_Logic, ZRef<ZQE::Iterator> iIterator);
	
	virtual ZRef<Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

private:
	ZRef<ZExprRep_Logic> fExprRep_Logic;
	ZRef<Iterator> fIterator;
	};

} // namespace ZQE
NAMESPACE_ZOOLIB_END

#endif // __ZQE_Iterator_Any__
