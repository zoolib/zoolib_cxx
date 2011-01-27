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
#include "zoolib/zra/ZRA_Expr_Rel_Embed.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * sCompare_T

template <>
int sCompare_T(const ZRA::Expr_Rel_Embed& iL, const ZRA::Expr_Rel_Embed& iR)
	{
	if (int compare = sCompare_T(iL.GetRelName(), iR.GetRelName()))
		return compare;

	if (int compare = sCompare_T(iL.GetBindings(), iR.GetBindings()))
		return compare;

	return sCompare_T(iL.GetOp0(), iR.GetOp0());
	}

ZMACRO_CompareRegistration_T(ZRA::Expr_Rel_Embed)

namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Embed

Expr_Rel_Embed::Expr_Rel_Embed(const RelName& iRelName,
	const Rename& iBindings, const ZRef<Expr_Rel>& iChild)
:	inherited(iChild)
,	fRelName(iRelName)
,	fBindings(iBindings)
	{}

void Expr_Rel_Embed::Accept(ZVisitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Embed* theVisitor = dynamic_cast<Visitor_Expr_Rel_Embed*>(&iVisitor))
		this->Accept_Expr_Rel_Embed(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Rel_Embed::Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Embed* theVisitor = dynamic_cast<Visitor_Expr_Rel_Embed*>(&iVisitor))
		this->Accept_Expr_Rel_Embed(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZRef<Expr_Rel> Expr_Rel_Embed::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Embed::Clone(const ZRef<Expr_Rel>& iOp0)
	{ return new Expr_Rel_Embed(fRelName, fBindings, iOp0); }

void Expr_Rel_Embed::Accept_Expr_Rel_Embed(Visitor_Expr_Rel_Embed& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Embed(this); }

const RelName& Expr_Rel_Embed::GetRelName() const
	{ return fRelName; }

const Rename& Expr_Rel_Embed::GetBindings() const
	{ return fBindings; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Embed

void Visitor_Expr_Rel_Embed::Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel> sEmbed(const RelName& iRelName,
	const ZRef<Expr_Rel>& iChild)
	{ return sEmbed(iRelName, RelHead(), iChild); }

ZRef<Expr_Rel> sEmbed(const RelName& iRelName,
	const RelHead& iBindings, const ZRef<Expr_Rel>& iChild)
	{
	if (!iChild)
		sSemanticError("sCalc, iChild is null");

	Rename theRename;
	for (RelHead::const_iterator i = iBindings.begin(); i != iBindings.end(); ++i)
		theRename[*i] = *i;
		
	return new Expr_Rel_Embed(iRelName, theRename, iChild);
	}

} // namespace ZRA
} // namespace ZooLib
