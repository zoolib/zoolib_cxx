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

#ifndef __ZQL_Expr_Rel_Binary_Union__
#define __ZQL_Expr_Rel_Binary_Union__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_Expr_Rel_Binary.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_Expr_Rel_Binary_Union;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Binary_Union

class Expr_Rel_Binary_Union : public Expr_Rel_Binary
	{
public:
	Expr_Rel_Binary_Union(ZRef<Expr_Rel> iLHS, ZRef<Expr_Rel> iRHS);
	virtual ~Expr_Rel_Binary_Union();

// From Expr_Rel via Expr_Rel_Binary
	virtual ZRelHead GetRelHead();

// From Expr_Rel_Binary
	virtual void Accept_Expr_Rel_Binary(Visitor_Expr_Rel_Binary& iVisitor);

	virtual ZRef<Expr_Rel_Binary> Clone(
		ZRef<Expr_Rel> iLHS, ZRef<Expr_Rel> iRHS);

// Our protocol
	virtual void Accept_Expr_Rel_Binary_Union(
		Visitor_Expr_Rel_Binary_Union& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Binary_Union

class Visitor_Expr_Rel_Binary_Union : public virtual Visitor_Expr_Rel_Binary
	{
public:
	virtual void Visit_Expr_Rel_Binary_Union(ZRef<Expr_Rel_Binary_Union> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Binary_Union> sUnion(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS);

ZRef<Expr_Rel_Binary_Union> operator|(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS);

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Rel_Binary__
