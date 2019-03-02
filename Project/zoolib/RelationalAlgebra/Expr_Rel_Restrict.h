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

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_Restrict_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_Restrict_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr_Bool.h"
#include "zoolib/Expr/Expr_Op_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Restrict;

// =================================================================================================
#pragma mark - Expr_Rel_Restrict

class Expr_Rel_Restrict
:	public virtual Expr_Rel
,	public virtual Expr_Op1_T<Expr_Rel>
	{
	typedef Expr_Op1_T<Expr_Rel> inherited;
public:
	Expr_Rel_Restrict(const ZRef<Expr_Rel>& iOp0, const ZRef<Expr_Bool>& iExpr_Bool);
	virtual ~Expr_Rel_Restrict();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op1_T<Expr_Rel>
	virtual void Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Rel>& iVisitor);

	virtual ZRef<Expr_Rel> Self();
	virtual ZRef<Expr_Rel> Clone(const ZRef<Expr_Rel>& iOp0);

// Our protocol
	virtual void Accept_Expr_Rel_Restrict(Visitor_Expr_Rel_Restrict& iVisitor);

	const ZRef<Expr_Bool>& GetExpr_Bool() const;

private:
	const ZRef<Expr_Bool> fExpr_Bool;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Restrict

class Visitor_Expr_Rel_Restrict
:	public virtual Visitor_Expr_Op1_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr);
	};

// =================================================================================================
#pragma mark - Relational operators

ZRef<Expr_Rel_Restrict> sRestrict(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZRef<Expr_Bool>& iExpr_Bool);

ZRef<Expr_Rel> operator&(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZRef<Expr_Bool>& iExpr_Bool);

ZRef<Expr_Rel> operator&(
	const ZRef<Expr_Bool>& iExpr_Bool, const ZRef<Expr_Rel>& iExpr_Rel);

ZRef<Expr_Rel>& operator&=(ZRef<Expr_Rel>& ioExpr_Rel, const ZRef<Expr_Bool>& iExpr_Bool);

} // namespace RelationalAlgebra

template <>
int sCompare_T(const RelationalAlgebra::Expr_Rel_Restrict& iL,
	const RelationalAlgebra::Expr_Rel_Restrict& iR);

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_Restrict_h__

