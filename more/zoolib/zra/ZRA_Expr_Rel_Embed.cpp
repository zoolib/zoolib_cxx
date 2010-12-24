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

#include "zoolib/zra/ZRA_Expr_Rel_Embed.h"

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Embed

Expr_Rel_Embed::Expr_Rel_Embed(ZRef<Expr_Rel> iOp0, ZRef<Expr_Rel> iOp1, const RelName& iRelName)
:	inherited(iOp0, iOp1)
,	fRelName(iRelName)
	{}

void Expr_Rel_Embed::Accept_Expr_Op2(ZVisitor_Expr_Op2_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Embed* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Embed*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Embed(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op2(iVisitor);
		}
	}

ZRef<Expr_Rel> Expr_Rel_Embed::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Embed::Clone(ZRef<Expr_Rel> iOp0, ZRef<Expr_Rel> iOp1)
	{ return new Expr_Rel_Embed(iOp0, iOp1, fRelName); }

void Expr_Rel_Embed::Accept_Expr_Rel_Embed(Visitor_Expr_Rel_Embed& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Embed(this); }

const RelName& Expr_Rel_Embed::GetRelName()
	{ return fRelName; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Embed

void Visitor_Expr_Rel_Embed::Visit_Expr_Rel_Embed(ZRef<Expr_Rel_Embed> iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Embed> sEmbed(const ZRef<Expr_Rel>& iParent,
	const RelName& iRelName, const ZRef<Expr_Rel>& iChild)
	{ return new Expr_Rel_Embed(iParent, iChild, iRelName); }
	

} // namespace ZRA
} // namespace ZooLib
