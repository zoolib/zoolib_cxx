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

#include "zoolib/Compare_Ref.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Project.h"

namespace ZooLib {

// =================================================================================================
// MARK: - sCompare_T

template <>
int sCompare_T(const RelationalAlgebra::Expr_Rel_Project& iL,
	const RelationalAlgebra::Expr_Rel_Project& iR)
	{
	if (int compare = sCompare_T(iL.GetProjectRelHead(), iR.GetProjectRelHead()))
		return compare;

	return sCompare_T(iL.GetOp0(), iR.GetOp0());
	}

ZMACRO_CompareRegistration_T(RelationalAlgebra::Expr_Rel_Project)

namespace RelationalAlgebra {

// =================================================================================================
// MARK: - Expr_Rel_Project

Expr_Rel_Project::Expr_Rel_Project(const ZRef<Expr_Rel>& iOp0, const RelHead& iRelHead)
:	inherited(iOp0)
,	fRelHead(iRelHead)
	{}

Expr_Rel_Project::~Expr_Rel_Project()
	{}

void Expr_Rel_Project::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Project* theVisitor = sDynNonConst<Visitor_Expr_Rel_Project>(&iVisitor))
		this->Accept_Expr_Rel_Project(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Rel_Project::Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Project* theVisitor = sDynNonConst<Visitor_Expr_Rel_Project>(&iVisitor))
		this->Accept_Expr_Rel_Project(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZRef<Expr_Rel> Expr_Rel_Project::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Project::Clone(const ZRef<Expr_Rel>& iOp0)
	{ return new Expr_Rel_Project(iOp0, fRelHead); }

void Expr_Rel_Project::Accept_Expr_Rel_Project(Visitor_Expr_Rel_Project& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Project(this); }

const RelHead& Expr_Rel_Project::GetProjectRelHead() const
	{ return fRelHead; }

// =================================================================================================
// MARK: - Visitor_Expr_Rel_Project

void Visitor_Expr_Rel_Project::Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
// MARK: - Relational operators

ZRef<Expr_Rel_Project> sProject(const ZRef<Expr_Rel>& iExpr, const RelHead& iRelHead)
	{
	if (iExpr)
		return new Expr_Rel_Project(iExpr, iRelHead);
	sSemanticError("sProject, rel is null");
	return null;
	}

ZRef<Expr_Rel> operator&(const ZRef<Expr_Rel>& iExpr, const RelHead& iRelHead)
	{ return sProject(iExpr, iRelHead); }

ZRef<Expr_Rel> operator&(const RelHead& iRelHead, const ZRef<Expr_Rel>& iExpr)
	{ return sProject(iExpr, iRelHead); }

ZRef<Expr_Rel>& operator&=(ZRef<Expr_Rel>& ioExpr, const RelHead& iRelHead)
	{ return ioExpr = ioExpr & iRelHead; }

} // namespace RelationalAlgebra
} // namespace ZooLib
