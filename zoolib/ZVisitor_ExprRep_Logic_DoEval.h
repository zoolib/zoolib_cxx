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

#ifndef __ZVisitor_ExprRep_Logic_DoEval__
#define __ZVisitor_ExprRep_Logic_DoEval__
#include "zconfig.h"

#include "zoolib/ZExprRep_Logic.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Logic_DoEval

class ZVisitor_ExprRep_Logic_DoEval
:	public virtual ZVisitor_ExprRep_Logic
	{
public:
	ZVisitor_ExprRep_Logic_DoEval();

// From ZVisitor_ExprRep_Logic
	virtual void Visit_Logic_True(ZRef<ZExprRep_Logic_True> iRep);
	virtual void Visit_Logic_False(ZRef<ZExprRep_Logic_False> iRep);
	virtual void Visit_Logic_Not(ZRef<ZExprRep_Logic_Not> iRep);
	virtual void Visit_Logic_And(ZRef<ZExprRep_Logic_And> iRep);
	virtual void Visit_Logic_Or(ZRef<ZExprRep_Logic_Or> iRep);

// Our protocol
	bool DoEval(ZRef<ZExprRep> iExprRep);

protected:
	bool fResult;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZVisitor_ExprRep_Logic_DoEval__
