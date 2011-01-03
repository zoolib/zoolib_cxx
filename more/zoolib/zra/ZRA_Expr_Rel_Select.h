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

#ifndef __ZRA_Expr_Rel_Select__
#define __ZRA_Expr_Rel_Select__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Bool.h"
#include "zoolib/ZExpr_Op_T.h"
#include "zoolib/zra/ZRA_Expr_Rel.h"

namespace ZooLib {
namespace ZRA {

class Visitor_Expr_Rel_Select;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Select

class Expr_Rel_Select
:	public virtual Expr_Rel
,	public virtual ZExpr_Op1_T<Expr_Rel>
	{
	typedef ZExpr_Op1_T<Expr_Rel> inherited;
public:
	Expr_Rel_Select(const ZRef<Expr_Rel>& iOp0, const ZRef<ZExpr_Bool>& iExpr_Bool);
	virtual ~Expr_Rel_Select();

// From ZVisitee
	virtual void Accept(ZVisitor& iVisitor);

// From ZExpr_Op1_T<Expr_Rel>
	virtual void Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor);

	virtual ZRef<Expr_Rel> Self();
	virtual ZRef<Expr_Rel> Clone(const ZRef<Expr_Rel>& iOp0);

// Our protocol
	virtual void Accept_Expr_Rel_Select(Visitor_Expr_Rel_Select& iVisitor);

	const ZRef<ZExpr_Bool>& GetExpr_Bool() const;

private:
	const ZRef<ZExpr_Bool> fExpr_Bool;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Select

class Visitor_Expr_Rel_Select
:	public virtual ZVisitor_Expr_Op1_T<Expr_Rel>
	{
	typedef ZVisitor_Expr_Op1_T<Expr_Rel> inherited;
public:
	virtual void Visit_Expr_Rel_Select(const ZRef<Expr_Rel_Select>& iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

// The order of parameters here differs from that in sRestrict_T precisely to keep them distinct.
ZRef<Expr_Rel_Select> sSelect(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZRef<ZExpr_Bool>& iExpr_Bool);

ZRef<Expr_Rel> operator&(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZRef<ZExpr_Bool>& iExpr_Bool);

ZRef<Expr_Rel> operator&(
	const ZRef<ZExpr_Bool>& iExpr_Bool, const ZRef<Expr_Rel>& iExpr_Rel);

ZRef<Expr_Rel>& operator&=(ZRef<Expr_Rel>& ioExpr_Rel, const ZRef<ZExpr_Bool>& iExpr_Bool);

} // namespace ZRA

template<>
int sCompare_T(const ZRA::Expr_Rel_Select& iL, const ZRA::Expr_Rel_Select& iR);

} // namespace ZooLib

#endif // __ZRA_Expr_Rel_Select__
