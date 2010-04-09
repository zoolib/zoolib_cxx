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

#ifndef __ZExpr__
#define __ZExpr__ 1
#include "zconfig.h"

#include "zoolib/ZRef_Counted.h"

NAMESPACE_ZOOLIB_BEGIN

class ZVisitor_Expr;

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr

class ZExpr : public ZRefCountedWithFinalize
	{
protected:
	ZExpr();

public:
	virtual ~ZExpr();

// From ZRefCountedWithFinalize
	virtual void Accept(ZVisitor& iVisitor);

// Our protocol
	virtual void Accept_Expr(ZVisitor_Expr& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr

class ZVisitor_Expr : public virtual ZVisitor
	{
public:
// Our protocol
	virtual void Visit_Expr(ZRef<ZExpr> iRep);
	};

NAMESPACE_ZOOLIB_END

#endif // __ZExpr__
