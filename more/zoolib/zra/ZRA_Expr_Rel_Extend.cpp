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

#include "zoolib/zra/ZRA_Expr_Rel_Extend.h"

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Extension

Extension::Extension()
	{}

Extension::~Extension()
	{}

Extension_Rel::Extension_Rel(const ZRef<Expr_Rel>& iRel)
:	fRel(iRel)
	{}

ZRef<Expr_Rel> Extension_Rel::GetRel()
	{ return fRel; }

Extension_Calculate::Extension_Calculate(const ZRef<Callable_Extend>& iCallable)
:	fCallable(iCallable)
	{}
ZRef<Callable_Extend> Extension_Calculate::GetCallable()
	{ return fCallable; }

Extension_Val::Extension_Val(const ZVal_Any& iVal)
:	fVal(iVal)
	{}

ZVal_Any Extension_Val::GetVal()
	{ return fVal; }

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Extend

Expr_Rel_Extend::Expr_Rel_Extend(
	const ZRef<Expr_Rel>& iOp0, const RelName& iRelName, const ZRef<Extension>& iExtension)
:	inherited(iOp0)
,	fRelName(iRelName)
,	fExtension(iExtension)
	{}

Expr_Rel_Extend::~Expr_Rel_Extend()
	{}

void Expr_Rel_Extend::Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Extend* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Extend*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Extend(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op1(iVisitor);
		}
	}

ZRef<Expr_Rel> Expr_Rel_Extend::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Extend::Clone(ZRef<Expr_Rel> iOp0)
	{ return new Expr_Rel_Extend(iOp0, fRelName, fExtension); }

void Expr_Rel_Extend::Accept_Expr_Rel_Extend(Visitor_Expr_Rel_Extend& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Extend(this); }

RelName Expr_Rel_Extend::GetRelName()
	{ return fRelName; }

ZRef<Extension> Expr_Rel_Extend::GetExtension()
	{ return fExtension; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Extend

void Visitor_Expr_Rel_Extend::Visit_Expr_Rel_Extend(ZRef<Expr_Rel_Extend> iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Extend> sExtend(
	const ZRef<Expr_Rel>& iParent, const RelName& iRelName, const ZRef<Expr_Rel>& iRel)
	{ return new Expr_Rel_Extend(iParent, iRelName, new Extension_Rel(iRel)); }

ZRef<Expr_Rel_Extend> sExtend(
	const ZRef<Expr_Rel>& iParent, const RelName& iRelName, const ZRef<Callable_Extend>& iCallable)
	{ return new Expr_Rel_Extend(iParent, iRelName, new Extension_Calculate(iCallable)); }

ZRef<Expr_Rel_Extend> sExtend(
	const ZRef<Expr_Rel>& iParent, const RelName& iRelName, const ZVal_Any& iVal)
	{ return new Expr_Rel_Extend(iParent, iRelName, new Extension_Val(iVal)); }

} // namespace ZRA
} // namespace ZooLib
