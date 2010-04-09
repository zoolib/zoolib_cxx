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

#ifndef __ZQL_Expr_Rel_Unary_Project__
#define __ZQL_Expr_Rel_Unary_Project__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_Expr_Rel_Unary.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_Expr_Rel_Unary_Project;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Unary_Project

class Expr_Rel_Unary_Project : public Expr_Rel_Unary
	{
public:
	Expr_Rel_Unary_Project(
		const ZRef<Expr_Rel>& iExpr_Rel, const ZRelHead& iRelHead);

	virtual ~Expr_Rel_Unary_Project();

// From Expr_Rel via Expr_Rel_Unary
	virtual ZRelHead GetRelHead();

// From Expr_Rel_Unary
	virtual void Accept_Expr_Rel_Unary(Visitor_Expr_Rel_Unary& iVisitor);

	virtual ZRef<Expr_Rel_Unary> Clone(ZRef<Expr_Rel> iExpr_Rel);

// Our protocol
	virtual void Accept_Expr_Rel_Unary_Project(
		Visitor_Expr_Rel_Unary_Project& iVisitor);

	ZRelHead GetProjectRelHead();

private:
	const ZRelHead fRelHead;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Unary_Project

class Visitor_Expr_Rel_Unary_Project : public virtual Visitor_Expr_Rel_Unary
	{
public:
	virtual void Visit_Expr_Rel_Unary_Project(ZRef<Expr_Rel_Unary_Project> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Unary_Project> sProject(const ZRef<Expr_Rel>& iExpr, const ZRelHead& iRelHead);

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Rel_Unary_Project__
