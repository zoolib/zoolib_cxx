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

#ifndef __ZQL_Expr_Relation_Binary_Difference__
#define __ZQL_Expr_Relation_Binary_Difference__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_Expr_Relation_Binary.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_Expr_Relation_Binary_Difference;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Relation_Binary_Difference

class Expr_Relation_Binary_Difference : public Expr_Relation_Binary
	{
public:
	Expr_Relation_Binary_Difference(ZRef<Expr_Relation> iLHS, ZRef<Expr_Relation> iRHS);
	virtual ~Expr_Relation_Binary_Difference();

// From Expr_Relation via Expr_Relation_Binary
	virtual ZRelHead GetRelHead();

// From Expr_Relation_Binary
	virtual void Accept_Expr_Relation_Binary(Visitor_Expr_Relation_Binary& iVisitor);

	virtual ZRef<Expr_Relation_Binary> Clone(
		ZRef<Expr_Relation> iLHS, ZRef<Expr_Relation> iRHS);

// Our protocol
	virtual void Accept_Expr_Relation_Binary_Difference(
		Visitor_Expr_Relation_Binary_Difference& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Relation_Binary_Difference

class Visitor_Expr_Relation_Binary_Difference : public virtual Visitor_Expr_Relation_Binary
	{
public:
	virtual void Visit_Expr_Relation_Binary_Difference(
		ZRef<Expr_Relation_Binary_Difference> iRep);
	};

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Relation_Binary__
