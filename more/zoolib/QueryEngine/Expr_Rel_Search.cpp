/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ZCompare_Ref.h"
#include "zoolib/QueryEngine/Expr_Rel_Search.h"

namespace ZooLib {

// =================================================================================================
// MARK: - sCompare_T

template <>
int sCompare_T(const QueryEngine::Expr_Rel_Search& iL, const QueryEngine::Expr_Rel_Search& iR)
	{
	if (int compare = sCompare_T(iL.GetRename(), iR.GetRename()))
		return compare;

	if (int compare = sCompare_T(iL.GetRelHead_Optional(), iR.GetRelHead_Optional()))
		return compare;

	return sCompare_T(iL.GetExpr_Bool(), iR.GetExpr_Bool());
	}

ZMACRO_CompareRegistration_T(QueryEngine::Expr_Rel_Search)

namespace QueryEngine {

// =================================================================================================
// MARK: - Expr_Rel_Search

Expr_Rel_Search::Expr_Rel_Search(const RelationalAlgebra::Rename& iRename,
	const RelationalAlgebra::RelHead& iRelHead_Optional,
	const ZRef<ZExpr_Bool>& iExpr_Bool)
:	fRename(iRename)
,	fRelHead_Optional(iRelHead_Optional)
,	fExpr_Bool(iExpr_Bool)
	{}

void Expr_Rel_Search::Accept(const ZVisitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Search* theVisitor = sDynNonConst<Visitor_Expr_Rel_Search>(&iVisitor))
		this->Accept_Expr_Rel_Search(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Rel_Search::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<RelationalAlgebra::Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Search* theVisitor = sDynNonConst<Visitor_Expr_Rel_Search>(&iVisitor))
		this->Accept_Expr_Rel_Search(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZRef<RelationalAlgebra::Expr_Rel> Expr_Rel_Search::Self()
	{ return this; }

ZRef<RelationalAlgebra::Expr_Rel> Expr_Rel_Search::Clone()
	{ return this; }

void Expr_Rel_Search::Accept_Expr_Rel_Search(Visitor_Expr_Rel_Search& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Search(this); }

const RelationalAlgebra::Rename& Expr_Rel_Search::GetRename() const
	{ return fRename; }

const RelationalAlgebra::RelHead& Expr_Rel_Search::GetRelHead_Optional() const
	{ return fRelHead_Optional; }

const ZRef<ZExpr_Bool>& Expr_Rel_Search::GetExpr_Bool() const
	{ return fExpr_Bool; }

// =================================================================================================
// MARK: - Visitor_Expr_Rel_Search

void Visitor_Expr_Rel_Search::Visit_Expr_Rel_Search(
	const ZRef<Expr_Rel_Search>& iExpr)
	{ this->Visit_Expr(iExpr); }

} // namespace QueryEngine
} // namespace ZooLib
