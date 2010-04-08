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

#include "zoolib/ZUtil_Strim_RelHead.h"
#include "zoolib/ZVisitor_ExprRep_ValCondition_ToStrim.h"
#include "zoolib/zql/ZQL_Util_Strim_Query.h"
#include "zoolib/zql/ZQL_Visitor_ExprRep_Relation_ToStrim.h"
#include "zoolib/zql/ZQL_Visitor_ExprRep_Restrict_ToStrim.h"
#include "zoolib/zql/ZQL_Visitor_ExprRep_Select_ToStrim.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {
namespace Util_Strim_Query {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Query_ToStrim

class Visitor_Query_ToStrim
:	public virtual ZVisitor_ExprRep_ValCondition_ToStrim
,	public virtual Visitor_ExprRep_Relation_ToStrim
,	public virtual Visitor_ExprRep_Restrict_ToStrim
,	public virtual Visitor_ExprRep_Select_ToStrim
	{
public:
	Visitor_Query_ToStrim(const Options& iOptions, const ZStrimW& iStrimW);
	};

Visitor_Query_ToStrim::Visitor_Query_ToStrim(const Options& iOptions, const ZStrimW& iStrimW)
:	ZVisitor_ExprRep_ToStrim(iOptions, iStrimW)
,	ZVisitor_ExprRep_Logic_ToStrim(iOptions, iStrimW)
,	ZVisitor_ExprRep_ValCondition_ToStrim(iOptions, iStrimW)
,	Visitor_ExprRep_Relation_ToStrim(iOptions, iStrimW)
,	Visitor_ExprRep_Restrict_ToStrim(iOptions, iStrimW)
,	Visitor_ExprRep_Select_ToStrim(iOptions, iStrimW)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL

void sToStrim(const ZRef<ZExprRep>& iRep, const ZStrimW& iStrimW)
	{ sToStrim(iRep, ZVisitor_ExprRep_ToStrim::Options(), iStrimW); }

void sToStrim(const ZRef<ZExprRep>& iRep,
	const ZVisitor_ExprRep_ToStrim::Options& iOptions,
	const ZStrimW& iStrimW)
	{ Visitor_Query_ToStrim(iOptions, iStrimW).Write(iRep); }

} // namespace Util_Strim_Query
} // namespace ZQL
NAMESPACE_ZOOLIB_END
