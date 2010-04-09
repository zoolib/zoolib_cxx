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

#ifndef __ZQL_Visitor_ExprRep_Relation_Unary_DoTransform__
#define __ZQL_Visitor_ExprRep_Relation_Unary_DoTransform__
#include "zconfig.h"

#include "zoolib/ZVisitor_ExprRep_DoTransform.h"

#include "zoolib/zql/ZQL_ExprRep_Relation_Unary.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Query_DoTransform

class Visitor_ExprRep_Relation_Unary_DoTransform
:	public virtual ZVisitor_ExprRep_DoTransform
,	public virtual Visitor_ExprRep_Relation_Unary
	{
public:
// From Visitor_ExprRep_Unary_Relation
	virtual bool Visit_ExprRep_Relation_Unary(ZRef<ExprRep_Relation_Unary> iRep);
	};

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Visitor_ExprRep_Relation_DoTransform__
