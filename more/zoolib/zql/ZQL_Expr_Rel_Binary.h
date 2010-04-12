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

#ifndef __ZQL_Expr_Rel_Binary__
#define __ZQL_Expr_Rel_Binary__ 1
#include "zconfig.h"

#include "zoolib/zql/ZQL_Expr_Rel.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_Expr_Rel_Binary;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Binary

class Expr_Rel_Binary : public Expr_Rel
	{
protected:
	Expr_Rel_Binary(ZRef<Expr_Rel> iLHS, ZRef<Expr_Rel> iRHS);

public:
	virtual ~Expr_Rel_Binary();

// From Expr_Rel
	virtual void Accept_Expr_Rel(Visitor_Expr_Rel& iVisitor);

// Our protocol
	virtual void Accept_Expr_Rel_Binary(Visitor_Expr_Rel_Binary& iVisitor);
	
	virtual ZRef<Expr_Rel_Binary> Clone (
		ZRef<Expr_Rel> iLHS, ZRef<Expr_Rel> iRHS) = 0;

	ZRef<Expr_Rel> GetLHS();
	ZRef<Expr_Rel> GetRHS();

protected:
	const ZRef<Expr_Rel> fLHS;
	const ZRef<Expr_Rel> fRHS;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Binary

class Visitor_Expr_Rel_Binary : public virtual Visitor_Expr_Rel
	{
public:
	virtual void Visit_Expr_Rel_Binary(ZRef<Expr_Rel_Binary> iExpr);
	};

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Rel_Binary__
