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

#ifndef __ZVisitor_ExprRep_Logic_ValCondition_DoToStrim__
#define __ZVisitor_ExprRep_Logic_ValCondition_DoToStrim__
#include "zconfig.h"

#include "zoolib/ZVisitor_ExprRep_Logic_DoToStrim.h"
#include "zoolib/ZExprRep_Logic_ValCondition.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Logic_ValCondition_DoToStrim

class ZVisitor_ExprRep_Logic_ValCondition_DoToStrim
:	public virtual ZVisitor_ExprRep_Logic_DoToStrim
,	public virtual ZVisitor_ExprRep_Logic_ValCondition
	{
public:
	ZVisitor_ExprRep_Logic_ValCondition_DoToStrim(const Options& iOptions, const ZStrimW& iStrimW);

// From ZVisitor_ExprRep_Logic_ValCondition
	virtual bool Visit_Logic_ValCondition(ZRef<ZExprRep_Logic_ValCondition> iRep);
	};

NAMESPACE_ZOOLIB_END

#endif // __ZVisitor_ExprRep_Logic_ValCondition_DoToStrim__
