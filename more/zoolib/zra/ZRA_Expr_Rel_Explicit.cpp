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

#include "zoolib/zra/ZRA_Expr_Rel_Explicit.h"

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Explicit

Expr_Rel_Explicit::Expr_Rel_Explicit(const ZMap_Any& iMap)
:	fMap(iMap)
	{}

Expr_Rel_Explicit::~Expr_Rel_Explicit()
	{}

void Expr_Rel_Explicit::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Explicit* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Explicit*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Explicit(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op0(iVisitor);
		}
	}

ZRef<Expr_Rel> Expr_Rel_Explicit::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Explicit::Clone()
	{ return new Expr_Rel_Explicit(fMap); }

RelHead Expr_Rel_Explicit::GetRelHead()
	{
	RelHead result;
	for (ZMap_Any::Index_t i = fMap.Begin(); i != fMap.End(); ++i)
		result.Insert(fMap.NameOf(i));
	return result;
	}

void Expr_Rel_Explicit::Accept_Expr_Rel_Explicit(Visitor_Expr_Rel_Explicit& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Explicit(this); }

ZMap_Any Expr_Rel_Explicit::GetMap()
	{ return fMap; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Explicit

void Visitor_Expr_Rel_Explicit::Visit_Expr_Rel_Explicit(ZRef<Expr_Rel_Explicit> iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Explicit> sExplicit(const ZMap_Any& iMap)
	{ return new Expr_Rel_Explicit(iMap); }

} // namespace ZRA
} // namespace ZooLib
