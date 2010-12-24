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

#ifndef __ZVisitor_Expr_Bool_Compare__
#define __ZVisitor_Expr_Bool_Compare__ 1
#include "zconfig.h"

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZExpr_Bool.h"
#include "zoolib/ZVisitor_Do_T.h"

namespace ZooLib {
namespace Visitor_Expr_Bool_Compare {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Bool_Compare::Comparer_Bootstrap

struct Comparer_Bootstrap
:	public virtual ZVisitor_Do_T<int>
,	public virtual ZVisitor_Expr_Bool_True
,	public virtual ZVisitor_Expr_Bool_False
,	public virtual ZVisitor_Expr_Bool_Not
,	public virtual ZVisitor_Expr_Bool_And
,	public virtual ZVisitor_Expr_Bool_Or
	{
protected:
	ZRef<ZExpr_Bool> fExpr;

public:
	Comparer_Bootstrap();

	int Compare(ZRef<ZExpr_Bool> iLHS, ZRef<ZExpr_Bool> iRHS);

	virtual void Visit_Expr_Bool_True(ZRef<ZExpr_Bool_True>);
	virtual void Visit_Expr_Bool_False(ZRef<ZExpr_Bool_False>);
	virtual void Visit_Expr_Bool_Not(ZRef<ZExpr_Bool_Not> iExpr);
	virtual void Visit_Expr_Bool_And(ZRef<ZExpr_Bool_And> iExpr);
	virtual void Visit_Expr_Bool_Or(ZRef<ZExpr_Bool_Or> iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Bool_Compare::Comparer

struct Comparer
:	public virtual ZVisitor_Do_T<int>
,	public virtual ZVisitor_Expr_Bool_True
,	public virtual ZVisitor_Expr_Bool_False
,	public virtual ZVisitor_Expr_Bool_Not
,	public virtual ZVisitor_Expr_Bool_And
,	public virtual ZVisitor_Expr_Bool_Or
	{
	Comparer_Bootstrap* fBootstrap;

	Comparer();
	Comparer(Comparer_Bootstrap* iBootstrap);

// From ZVisitor_Expr_Bool_XXX
	virtual void Visit_Expr_Bool_True(ZRef<ZExpr_Bool_True>);
	virtual void Visit_Expr_Bool_False(ZRef<ZExpr_Bool_False>);
	virtual void Visit_Expr_Bool_Not(ZRef<ZExpr_Bool_Not>);
	virtual void Visit_Expr_Bool_And(ZRef<ZExpr_Bool_And>);
	virtual void Visit_Expr_Bool_Or(ZRef<ZExpr_Bool_Or>);

	int CompareUnary(ZRef<ZExpr_Op1_T<ZExpr_Bool> > iLHS, ZRef<ZExpr_Op1_T<ZExpr_Bool> > iRHS);
	int CompareBinary(ZRef<ZExpr_Op2_T<ZExpr_Bool> > iLHS, ZRef<ZExpr_Op2_T<ZExpr_Bool> > iRHS);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Bool_Compare::Comparer_GT_XXX

struct Comparer_GT_True : public virtual Comparer
	{ virtual void Visit_Expr_Bool_True(ZRef<ZExpr_Bool_True>); };

struct Comparer_GT_False : public virtual Comparer_GT_True
	{ virtual void Visit_Expr_Bool_False(ZRef<ZExpr_Bool_False>); };

struct Comparer_GT_Not : public virtual Comparer_GT_False
	{ virtual void Visit_Expr_Bool_Not(ZRef<ZExpr_Bool_Not>); };

struct Comparer_GT_And : public virtual Comparer_GT_Not
	{ virtual void Visit_Expr_Bool_And(ZRef<ZExpr_Bool_And>); };

struct Comparer_GT_Or : public virtual Comparer_GT_And
	{ virtual void Visit_Expr_Bool_Or(ZRef<ZExpr_Bool_Or>); };

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Bool_Compare::Comparer_XXX

struct Comparer_True : public virtual Comparer
	{
	Comparer_True(Comparer_Bootstrap* iBootstrap);
	virtual void Visit_Expr_Bool_True(ZRef<ZExpr_Bool_True>);
	};

struct Comparer_False : public virtual Comparer_GT_True
	{
	Comparer_False(Comparer_Bootstrap* iBootstrap);
	virtual void Visit_Expr_Bool_False(ZRef<ZExpr_Bool_False>);
	};

struct Comparer_Not : public virtual Comparer_GT_False
	{
	ZRef<ZExpr_Bool_Not> fExpr;
	Comparer_Not(Comparer_Bootstrap* iBootstrap, ZRef<ZExpr_Bool_Not> iExpr);
	virtual void Visit_Expr_Bool_Not(ZRef<ZExpr_Bool_Not> iExpr);
	};

struct Comparer_And : public virtual Comparer_GT_Not
	{
	ZRef<ZExpr_Bool_And> fExpr;
	Comparer_And(Comparer_Bootstrap* iBootstrap, ZRef<ZExpr_Bool_And> iExpr);
	virtual void Visit_Expr_Bool_And(ZRef<ZExpr_Bool_And> iExpr);
	};

struct Comparer_Or : public virtual Comparer_GT_And
	{
	ZRef<ZExpr_Bool_Or> fExpr;
	Comparer_Or(Comparer_Bootstrap* iBootstrap, ZRef<ZExpr_Bool_Or> iExpr);
	virtual void Visit_Expr_Bool_Or(ZRef<ZExpr_Bool_Or> iExpr);
	};

} // namespace Visitor_Expr_Bool_Compare
} // namespace ZooLib

#endif // __ZVisitor_Expr_Bool_Compare__
