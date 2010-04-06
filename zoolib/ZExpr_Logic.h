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

class ZVisitor_ExprRep_Logic;

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep

class ZExprRep_Logic : public ZExprRep
	{
protected:
	ZExprRep_Logic();

public:
	virtual ~ZExprRep_Logic();

// From ZExprRep
	virtual bool Accept(ZVisitor_ExprRep& iVisitor);

// Our protocol
	virtual bool Accept(ZVisitor_ExprRep_Logic& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_True

class ZExprRep_Logic_True : public ZExprRep_Logic
	{
public:
	ZExprRep_Logic_True();

// From ZExprRep_Logic
	virtual bool Accept(ZVisitor_ExprRep_Logic& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_False

class ZExprRep_Logic_False : public ZExprRep_Logic
	{
public:
	ZExprRep_Logic_False();

// From ZExprRep_Logic
	virtual bool Accept(ZVisitor_ExprRep_Logic& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_Not

class ZExprRep_Logic_Not : public ZExprRep_Logic
	{
public:
	ZExprRep_Logic_Not(ZRef<ZExprRep_Logic> iOperand);
	virtual ~ZExprRep_Logic_Not();

// From ZExprRep_Logic
	virtual bool Accept(ZVisitor_ExprRep_Logic& iVisitor);

// Our protocol
	ZRef<ZExprRep_Logic> GetOperand();

private:
	ZRef<ZExprRep_Logic> fOperand;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_Dyadic

class ZExprRep_Logic_Dyadic : public ZExprRep_Logic
	{
public:
	ZExprRep_Logic_Dyadic(ZRef<ZExprRep_Logic> iLHS, ZRef<ZExprRep_Logic> iRHS);
	virtual ~ZExprRep_Logic_Dyadic();

// Our protocol
	ZRef<ZExprRep_Logic> GetLHS();
	ZRef<ZExprRep_Logic> GetRHS();

protected:
	ZRef<ZExprRep_Logic> fLHS;
	ZRef<ZExprRep_Logic> fRHS;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_And

class ZExprRep_Logic_And : public ZExprRep_Logic_Dyadic
	{
public:
	ZExprRep_Logic_And(ZRef<ZExprRep_Logic> iLHS, ZRef<ZExprRep_Logic> iRHS);

// From ZExprRep_Logic
	virtual bool Accept(ZVisitor_ExprRep_Logic& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logic_Or

class ZExprRep_Logic_Or : public ZExprRep_Logic_Dyadic
	{
public:
	ZExprRep_Logic_Or(ZRef<ZExprRep_Logic> iLHS, ZRef<ZExprRep_Logic> iRHS);

// From ZExprRep_Logic
	virtual bool Accept(ZVisitor_ExprRep_Logic& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Logic

class ZVisitor_ExprRep_Logic : public virtual ZVisitor_ExprRep
	{
public:
	virtual bool Visit_Logic_True(ZRef<ZExprRep_Logic_True> iRep);
	virtual bool Visit_Logic_False(ZRef<ZExprRep_Logic_False> iRep);
	virtual bool Visit_Logic_Not(ZRef<ZExprRep_Logic_Not> iRep);
	virtual bool Visit_Logic_And(ZRef<ZExprRep_Logic_And> iRep);
	virtual bool Visit_Logic_Or(ZRef<ZExprRep_Logic_Or> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic

class ZExpr_Logic : public ZExpr
	{
	typedef ZExpr inherited;

	ZExpr_Logic operator=(const ZExpr&);
	ZExpr_Logic operator=(const ZRef<ZExprRep>&);

public:
	ZExpr_Logic();
	ZExpr_Logic(const ZExpr_Logic& iOther);
	~ZExpr_Logic();
	ZExpr_Logic& operator=(const ZExpr_Logic& iOther);

	ZExpr_Logic(const ZRef<ZExprRep_Logic>& iRep);

	operator ZRef<ZExprRep_Logic>() const;
	};

ZExpr_Logic operator&(const ZExpr_Logic& iLHS, const ZExpr_Logic& iRHS);
ZExpr_Logic& operator&=(ZExpr_Logic& iLHS, const ZExpr_Logic& iRHS);

ZExpr_Logic operator|(const ZExpr_Logic& iLHS, const ZExpr_Logic& iRHS);
ZExpr_Logic& operator|=(ZExpr_Logic& iLHS, const ZExpr_Logic& iRHS);

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Logic__
