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

#ifndef __ZExpr_Logic__
#define __ZExpr_Logic__ 1
#include "zconfig.h"

#include "zoolib/ZExpr.h"

NAMESPACE_ZOOLIB_BEGIN

class ZVisitor_Expr_Logic;

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr

class ZExpr_Logic : public ZExpr
	{
protected:
	ZExpr_Logic();

public:
	virtual ~ZExpr_Logic();

// From ZExpr
	virtual void Accept_Expr(ZVisitor_Expr& iVisitor);

// Our protocol
	virtual void Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_True

class ZExpr_Logic_True : public ZExpr_Logic
	{
public:
	ZExpr_Logic_True();

// From ZExpr_Logic
	virtual void Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_False

class ZExpr_Logic_False : public ZExpr_Logic
	{
public:
	ZExpr_Logic_False();

// From ZExpr_Logic
	virtual void Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_Not

class ZExpr_Logic_Not : public ZExpr_Logic
	{
public:
	ZExpr_Logic_Not(ZRef<ZExpr_Logic> iOperand);
	virtual ~ZExpr_Logic_Not();

// From ZExpr_Logic
	virtual void Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor);

// Our protocol
	ZRef<ZExpr_Logic> GetOperand();

private:
	ZRef<ZExpr_Logic> fOperand;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_Dyadic

class ZExpr_Logic_Dyadic : public ZExpr_Logic
	{
public:
	ZExpr_Logic_Dyadic(ZRef<ZExpr_Logic> iLHS, ZRef<ZExpr_Logic> iRHS);
	virtual ~ZExpr_Logic_Dyadic();

// Our protocol
	ZRef<ZExpr_Logic> GetLHS();
	ZRef<ZExpr_Logic> GetRHS();

protected:
	ZRef<ZExpr_Logic> fLHS;
	ZRef<ZExpr_Logic> fRHS;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_And

class ZExpr_Logic_And : public ZExpr_Logic_Dyadic
	{
public:
	ZExpr_Logic_And(ZRef<ZExpr_Logic> iLHS, ZRef<ZExpr_Logic> iRHS);

// From ZExpr_Logic
	virtual void Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_Or

class ZExpr_Logic_Or : public ZExpr_Logic_Dyadic
	{
public:
	ZExpr_Logic_Or(ZRef<ZExpr_Logic> iLHS, ZRef<ZExpr_Logic> iRHS);

// From ZExpr_Logic
	virtual void Accept_Expr_Logic(ZVisitor_Expr_Logic& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic

class ZVisitor_Expr_Logic : public virtual ZVisitor_Expr
	{
public:
	virtual void Visit_Logic_True(ZRef<ZExpr_Logic_True> iRep);
	virtual void Visit_Logic_False(ZRef<ZExpr_Logic_False> iRep);
	virtual void Visit_Logic_Not(ZRef<ZExpr_Logic_Not> iRep);
	virtual void Visit_Logic_And(ZRef<ZExpr_Logic_And> iRep);
	virtual void Visit_Logic_Or(ZRef<ZExpr_Logic_Or> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Operators

ZRef<ZExpr_Logic_Not> operator~(const ZRef<ZExpr_Logic>& iExpr_Logic);
ZRef<ZExpr_Logic> operator~(const ZRef<ZExpr_Logic_Not>& iExpr_Logic_Not);

ZRef<ZExpr_Logic> sNot(const ZRef<ZExpr_Logic_Not>& iExpr_Logic_Not);
ZRef<ZExpr_Logic_Not> sNot(const ZRef<ZExpr_Logic>& iExpr_Logic);

ZRef<ZExpr_Logic> sNot(const ZRef<ZExpr_Logic_Not>& iExpr_Logic_Not);

ZRef<ZExpr_Logic> operator&(bool iBool, const ZRef<ZExpr_Logic>& iExpr_Logic);
ZRef<ZExpr_Logic> operator&(const ZRef<ZExpr_Logic>& iExpr_Logic, bool iBool);

ZRef<ZExpr_Logic> operator|(bool iBool, const ZRef<ZExpr_Logic>& iExpr_Logic);
ZRef<ZExpr_Logic> operator|(const ZRef<ZExpr_Logic>& iExpr_Logic, bool iBool);

ZRef<ZExpr_Logic> operator&(const ZRef<ZExpr_Logic>& iLHS, const ZRef<ZExpr_Logic>& iRHS);
ZRef<ZExpr_Logic>& operator&=(ZRef<ZExpr_Logic>& iLHS, const ZRef<ZExpr_Logic>& iRHS);

ZRef<ZExpr_Logic> operator|(const ZRef<ZExpr_Logic>& iLHS, const ZRef<ZExpr_Logic>& iRHS);
ZRef<ZExpr_Logic>& operator|=(ZRef<ZExpr_Logic>& iLHS, const ZRef<ZExpr_Logic>& iRHS);

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Logic__
