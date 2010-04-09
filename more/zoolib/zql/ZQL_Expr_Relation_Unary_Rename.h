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

#ifndef __ZQL_Expr_Relation_Unary_Rename__
#define __ZQL_Expr_Relation_Unary_Rename__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_Expr_Relation_Unary.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_Expr_Relation_Unary_Rename;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Relation_Unary_Rename

class Expr_Relation_Unary_Rename : public Expr_Relation_Unary
	{
public:
	Expr_Relation_Unary_Rename(ZRef<Expr_Relation> iExpr_Relation,
		const std::string& iOld, const std::string& iNew);

	virtual ~Expr_Relation_Unary_Rename();

// From Expr_Relation via Expr_Relation_Unary
	virtual ZRelHead GetRelHead();

// From Expr_Relation_Unary
	virtual void Accept_Expr_Relation_Unary(Visitor_Expr_Relation_Unary& iVisitor);

	virtual ZRef<Expr_Relation_Unary> Clone(ZRef<Expr_Relation> iExpr_Relation);

// Our protocol
	virtual void Accept_Expr_Relation_Unary_Rename(
		Visitor_Expr_Relation_Unary_Rename& iVisitor);

	const std::string& GetOld();
	const std::string& GetNew();

private:
	const std::string fOld;
	const std::string fNew;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Relation_Unary_Rename

class Visitor_Expr_Relation_Unary_Rename : public virtual Visitor_Expr_Relation_Unary
	{
public:
	virtual void Visit_Expr_Relation_Unary_Rename(ZRef<Expr_Relation_Unary_Rename> iRep);
	};

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Relation_Unary_Rename__
