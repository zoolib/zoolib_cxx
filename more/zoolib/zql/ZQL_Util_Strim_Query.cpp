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
#include "zoolib/ZVisitor_ExprRep_Logic_ValCondition_DoToStrim.h"
#include "zoolib/zql/ZQL_Util_Strim_Query.h"
#include "zoolib/zql/ZQL_Visitor_ExprRep_Relation_DoToStrim.h"
#include "zoolib/zql/ZQL_Visitor_ExprRep_Relation_Restrict_DoToStrim.h"
#include "zoolib/zql/ZQL_Visitor_ExprRep_Relation_Select_DoToStrim.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Concrete.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {
namespace Util_Strim_Query {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoToStrim

namespace ZANONYMOUS {

class Visitor_DoToStrim
:	public virtual ZVisitor_ExprRep_Logic_ValCondition_DoToStrim
,	public virtual Visitor_ExprRep_Relation_DoToStrim
,	public virtual Visitor_ExprRep_Relation_Restrict_DoToStrim
,	public virtual Visitor_ExprRep_Relation_Select_DoToStrim
,	public virtual Visitor_ExprRep_Relation_Concrete
	{
public:
	Visitor_DoToStrim(const Options& iOptions, const ZStrimW& iStrimW);

// From Visitor_ExprRep_Relation_Concrete
	bool Visit_ExprRep_Relation_Concrete(ZRef<ExprRep_Relation_Concrete> iRep);
	};

} // anonymous namespace

Visitor_DoToStrim::Visitor_DoToStrim(const Options& iOptions, const ZStrimW& iStrimW)
:	ZVisitor_ExprRep_DoToStrim(iOptions, iStrimW)
,	ZVisitor_ExprRep_Logic_DoToStrim(iOptions, iStrimW)
,	ZVisitor_ExprRep_Logic_ValCondition_DoToStrim(iOptions, iStrimW)
,	Visitor_ExprRep_Relation_DoToStrim(iOptions, iStrimW)
,	Visitor_ExprRep_Relation_Restrict_DoToStrim(iOptions, iStrimW)
,	Visitor_ExprRep_Relation_Select_DoToStrim(iOptions, iStrimW)
	{}

bool Visitor_DoToStrim::Visit_ExprRep_Relation_Concrete(ZRef<ExprRep_Relation_Concrete> iRep)
	{
	fStrimW << "/* Relation_Concrete: " << typeid(*iRep.Get()).name() << " */";
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL

void sToStrim(const ZRef<ZExprRep>& iRep, const ZStrimW& iStrimW)
	{ sToStrim(iRep, ZVisitor_ExprRep_DoToStrim::Options(), iStrimW); }

void sToStrim(const ZRef<ZExprRep>& iRep,
	const ZVisitor_ExprRep_DoToStrim::Options& iOptions,
	const ZStrimW& iStrimW)
	{ Visitor_DoToStrim(iOptions, iStrimW).DoToStrim(iRep); }

} // namespace Util_Strim_Query
} // namespace ZQL
NAMESPACE_ZOOLIB_END
