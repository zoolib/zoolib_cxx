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

#ifndef __ZQL_Visitor_ExprRep_Relation_Restrict_DoToStrim__
#define __ZQL_Visitor_ExprRep_Relation_Restrict_DoToStrim__
#include "zconfig.h"

#include "zoolib/ZValCondition.h"
#include "zoolib/ZVisitor_ExprRep_DoToStrim.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Restrict.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Restrict_DoToStrim

class Visitor_ExprRep_Relation_Restrict_DoToStrim
:	public virtual ZVisitor_ExprRep_DoToStrim
,	public virtual Visitor_ExprRep_Relation_Restrict
	{
public:
	Visitor_ExprRep_Relation_Restrict_DoToStrim(const Options& iOptions, const ZStrimW& iStrimW);

// From Visitor_ExprRep_Relation_Restrict
	virtual bool Visit_ExprRep_Relation_Restrict(ZRef<ExprRep_Relation_Restrict> iRep);
	};

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Visitor_ExprRep_Relation_Restrict_DoToStrim__
