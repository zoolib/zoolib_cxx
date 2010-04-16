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

#ifndef __ZQE_Iterator__
#define __ZQE_Iterator__ 1
#include "zconfig.h"

#include "zoolib/zqe/ZQE_Result.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Iterator

class Iterator : public ZCounted
	{
protected:
	Iterator();

public:
	virtual ~Iterator();
	
// Our protocol
	virtual ZRef<Iterator> Clone() = 0;
	virtual ZRef<Result> ReadInc() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Intersect

class Iterator_Intersect : public Iterator
	{
public:
	Iterator_Intersect(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS);
	
// From Iterator
	virtual ZRef<Iterator> Clone();
	virtual ZRef<Result> ReadInc();

private:
	Iterator_Intersect(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS,
		ZRef<Iterator> iIterator_RHS_Model);

	ZRef<Iterator> fIterator_LHS;
	ZRef<Iterator> fIterator_RHS;
	ZRef<Iterator> fIterator_RHS_Model;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Join

class Iterator_Join : public Iterator
	{
public:
	Iterator_Join(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS);
	
// From Iterator
	virtual ZRef<Iterator> Clone();
	virtual ZRef<Result> ReadInc();

private:
	Iterator_Join(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS,
		ZRef<Iterator> iIterator_RHS_Model);

	ZRef<Iterator> fIterator_LHS;
	ZRef<Iterator> fIterator_RHS;
	ZRef<Iterator> fIterator_RHS_Model;
	ZRef<Result> fResult_LHS;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Union

class Iterator_Union : public Iterator
	{
public:
	Iterator_Union(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS);
	
// From Iterator
	virtual ZRef<Iterator> Clone();
	virtual ZRef<Result> ReadInc();

private:
	ZRef<Iterator> fIterator_LHS;
	ZRef<Iterator> fIterator_RHS;
	};

} // namespace ZQE
NAMESPACE_ZOOLIB_END

#endif // __ZQE_Iterator__
