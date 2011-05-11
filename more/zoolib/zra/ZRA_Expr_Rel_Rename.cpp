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

#include "zoolib/ZCompare_Ref.h"
#include "zoolib/ZCompare_String.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"

using std::string;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * sCompare_T

template <>
int sCompare_T(const ZRA::Expr_Rel_Rename& iL, const ZRA::Expr_Rel_Rename& iR)
	{
	if (int compare = sCompare_T(iL.GetOld(), iR.GetOld()))
		return compare;

	if (int compare = sCompare_T(iL.GetNew(), iR.GetNew()))
		return compare;

	return sCompare_T(iL.GetOp0(), iR.GetOp0());
	}

ZMACRO_CompareRegistration_T(ZRA::Expr_Rel_Rename)

namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Rename

Expr_Rel_Rename::Expr_Rel_Rename(const ZRef<Expr_Rel>& iOp0,
	const RelName& iNew, const RelName& iOld)
:	inherited(iOp0)
,	fNew(iNew)
,	fOld(iOld)
	{}

Expr_Rel_Rename::~Expr_Rel_Rename()
	{}

void Expr_Rel_Rename::Accept(ZVisitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Rename* theVisitor = dynamic_cast<Visitor_Expr_Rel_Rename*>(&iVisitor))
		this->Accept_Expr_Rel_Rename(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Rel_Rename::Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Rename* theVisitor = dynamic_cast<Visitor_Expr_Rel_Rename*>(&iVisitor))
		this->Accept_Expr_Rel_Rename(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZRef<Expr_Rel> Expr_Rel_Rename::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Rename::Clone(const ZRef<Expr_Rel>& iOp0)
	{ return new Expr_Rel_Rename(iOp0, fNew, fOld); }

void Expr_Rel_Rename::Accept_Expr_Rel_Rename(Visitor_Expr_Rel_Rename& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Rename(this); }

const RelName& Expr_Rel_Rename::GetNew() const
	{ return fNew; }

const RelName& Expr_Rel_Rename::GetOld() const
	{ return fOld; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Rename

void Visitor_Expr_Rel_Rename::Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel> sRename(const ZRef<Expr_Rel>& iExpr,
	const RelName& iNewPropName, const RelName& iOldPropName)
	{
	if (iExpr)
		return new Expr_Rel_Rename(iExpr, iNewPropName, iOldPropName);
	sSemanticError("sRename, rel is null");
	return null;
	}

} // namespace ZRA
} // namespace ZooLib
