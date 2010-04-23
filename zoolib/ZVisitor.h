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

#ifndef __ZVisitor__
#define __ZVisitor__ 1
#include "zconfig.h"

#include "zoolib/ZRef_Counted.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZSetRestore_T

// Temporary home for this.

template <class T>
class ZSetRestore_T
	{
public:
	ZSetRestore_T(T& ioRef, T iVal)
	:	fRef(ioRef)
	,	fPrior(ioRef)
		{ fRef = iVal; }

	~ZSetRestore_T()
		{ fRef = fPrior; }

private:
	T& fRef;
	const T fPrior;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitee

class ZVisitor;

class ZVisitee : public virtual ZCounted
	{
public:
// Our protocol
	virtual void Accept(ZVisitor& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor

class ZVisitor
	{
public:
	virtual void Visit(ZRef<ZVisitee> iRep);
	};

NAMESPACE_ZOOLIB_END

#endif // __ZVisitor__
