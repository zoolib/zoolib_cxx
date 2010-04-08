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

#ifndef __ZExprRep__
#define __ZExprRep__ 1
#include "zconfig.h"

#include "zoolib/ZRef_Counted.h"

NAMESPACE_ZOOLIB_BEGIN

class ZVisitor_ExprRep;

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep

class ZExprRep : public ZRefCountedWithFinalize
	{
protected:
	ZExprRep();

public:
	virtual ~ZExprRep();

// From ZRefCountedWithFinalize
	virtual bool Accept(ZVisitor& iVisitor);

// Our protocol
	virtual bool Accept(ZVisitor_ExprRep& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep

class ZVisitor_ExprRep : public ZVisitor
	{
public:
// Our protocol
	virtual bool Visit_ExprRep(ZRef<ZExprRep> iRep);
	};

NAMESPACE_ZOOLIB_END

#endif // __ZExprRep__
