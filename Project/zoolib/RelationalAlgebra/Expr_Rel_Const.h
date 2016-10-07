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

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_Const_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_Const_h__ 1
#include "zconfig.h"

#include "zoolib/Val_Any.h"

#include "zoolib/Expr/Expr_Op_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Const;

// =================================================================================================
#pragma mark -
#pragma mark Expr_Rel_Const

class Expr_Rel_Const
:	public virtual Expr_Rel
,	public virtual Expr_Op0_T<Expr_Rel>
	{
	typedef Expr_Op0_T<Expr_Rel> inherited;
public:
	Expr_Rel_Const(const ColName& iColName, const Val_Any& iVal);

	virtual ~Expr_Rel_Const();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZRef<Expr_Rel> Self();
	virtual ZRef<Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Expr_Rel_Const(Visitor_Expr_Rel_Const& iVisitor);

	const ColName& GetColName() const;
	const Val_Any& GetVal() const;

private:
	const ColName fColName;
	const Val_Any fVal;
	};

// =================================================================================================
#pragma mark -
#pragma mark Visitor_Expr_Rel_Const

class Visitor_Expr_Rel_Const
:	public virtual Visitor_Expr_Op0_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark Relational operators

ZRef<Expr_Rel> sConst(const ColName& iColName, const Val_Any& iVal);

} // namespace RelationalAlgebra

template <>
int sCompare_T(const RelationalAlgebra::Expr_Rel_Const& iL,
	const RelationalAlgebra::Expr_Rel_Const& iR);

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_Const_h__
