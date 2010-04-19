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
#include "zoolib/zql/ZQL_RelHead.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Project

class Iterator_Any_Project
:	public Iterator
	{
public:
	Iterator_Any_Project(ZRef<Iterator> iIterator, const ZQL::RelHead& iRelHead);
	
// From Iterator
	virtual ZRef<Iterator> Clone();
	virtual ZRef<Result> ReadInc();

private:
	ZRef<Iterator> fIterator;
	ZQL::RelHead fRelHead;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Rename

class Iterator_Any_Rename
:	public Iterator
	{
public:
	Iterator_Any_Rename(ZRef<Iterator> iIterator, const std::string& iNew, const std::string& iOld);
	
// From Iterator
	virtual ZRef<Iterator> Clone();
	virtual ZRef<Result> ReadInc();

private:
	ZRef<Iterator> fIterator;
	const std::string fNew;
	const std::string fOld;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Restrict

class Iterator_Any_Restrict
:	public Iterator
	{
public:
	Iterator_Any_Restrict(ZRef<Iterator> iIterator, const ZValCondition& iValCondition);
	
// From Iterator
	virtual ZRef<Iterator> Clone();
	virtual ZRef<Result> ReadInc();

private:
	ZRef<Iterator> fIterator;
	const ZValCondition fValCondition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Select

class Iterator_Any_Select : public Iterator
	{
public:
	Iterator_Any_Select(ZRef<Iterator> iIterator, ZRef<ZExpr_Logic> iExpr_Logic);
	
// From Iterator
	virtual ZRef<Iterator> Clone();
	virtual ZRef<Result> ReadInc();

private:
	ZRef<Iterator> fIterator;
	ZRef<ZExpr_Logic> fExpr_Logic;
	};

} // namespace ZQE
NAMESPACE_ZOOLIB_END

#endif // __ZQE_Iterator_Any__
