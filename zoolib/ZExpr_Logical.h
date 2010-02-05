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

#ifndef __ZExpr_Logical__
#define __ZExpr_Logical__ 1

#include "zconfig.h"

#include "zoolib/ZExpr.h"

NAMESPACE_ZOOLIB_BEGIN

class ZVisitor_ExprRep_Logical;

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep

class ZExprRep_Logical : public ZExprRep
	{
protected:
	ZExprRep_Logical();

public:
	virtual ~ZExprRep_Logical();

// From ZExprRep
	virtual bool Accept(ZVisitor_ExprRep& iVisitor);

// Our protocol
	virtual bool Accept(ZVisitor_ExprRep_Logical& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logical_True

class ZExprRep_Logical_True : public ZExprRep_Logical
	{
public:
	ZExprRep_Logical_True();

// From ZExprRep_Logical
	virtual bool Accept(ZVisitor_ExprRep_Logical& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logical_False

class ZExprRep_Logical_False : public ZExprRep_Logical
	{
public:
	ZExprRep_Logical_False();

// From ZExprRep_Logical
	virtual bool Accept(ZVisitor_ExprRep_Logical& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logical_Not

class ZExprRep_Logical_Not : public ZExprRep_Logical
	{
public:
	ZExprRep_Logical_Not(ZRef<ZExprRep_Logical> iOperand);
	virtual ~ZExprRep_Logical_Not();

// From ZExprRep_Logical
	virtual bool Accept(ZVisitor_ExprRep_Logical& iVisitor);

// Our protocol
	ZRef<ZExprRep_Logical> GetOperand();

private:
	ZRef<ZExprRep_Logical> fOperand;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logical_Dyadic

class ZExprRep_Logical_Dyadic : public ZExprRep_Logical
	{
public:
	ZExprRep_Logical_Dyadic(ZRef<ZExprRep_Logical> iLHS, ZRef<ZExprRep_Logical> iRHS);
	virtual ~ZExprRep_Logical_Dyadic();

// Our protocol
	ZRef<ZExprRep_Logical> GetLHS();
	ZRef<ZExprRep_Logical> GetRHS();

protected:
	ZRef<ZExprRep_Logical> fLHS;
	ZRef<ZExprRep_Logical> fRHS;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logical_And

class ZExprRep_Logical_And : public ZExprRep_Logical_Dyadic
	{
public:
	ZExprRep_Logical_And(ZRef<ZExprRep_Logical> iLHS, ZRef<ZExprRep_Logical> iRHS);

// From ZExprRep_Logical
	virtual bool Accept(ZVisitor_ExprRep_Logical& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExprRep_Logical_Or

class ZExprRep_Logical_Or : public ZExprRep_Logical_Dyadic
	{
public:
	ZExprRep_Logical_Or(ZRef<ZExprRep_Logical> iLHS, ZRef<ZExprRep_Logical> iRHS);

// From ZExprRep_Logical
	virtual bool Accept(ZVisitor_ExprRep_Logical& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Logical

class ZVisitor_ExprRep_Logical : public ZVisitor_ExprRep
	{
public:
	virtual bool Visit_Logical_True(ZRef<ZExprRep_Logical_True> iRep);
	virtual bool Visit_Logical_False(ZRef<ZExprRep_Logical_False> iRep);
	virtual bool Visit_Logical_Not(ZRef<ZExprRep_Logical_Not> iRep);
	virtual bool Visit_Logical_And(ZRef<ZExprRep_Logical_And> iRep);
	virtual bool Visit_Logical_Or(ZRef<ZExprRep_Logical_Or> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logical

class ZExpr_Logical : public ZExpr
	{
	typedef ZExpr inherited;

	ZExpr_Logical operator=(const ZExpr&);
	ZExpr_Logical operator=(const ZRef<ZExprRep>&);

public:
	ZExpr_Logical();
	ZExpr_Logical(const ZExpr_Logical& iOther);
	~ZExpr_Logical();
	ZExpr_Logical& operator=(const ZExpr_Logical& iOther);

	ZExpr_Logical(const ZRef<ZExprRep_Logical>& iRep);

	operator ZRef<ZExprRep_Logical>() const;
	};

ZExpr_Logical operator&(const ZExpr_Logical& iLHS, const ZExpr_Logical& iRHS);
ZExpr_Logical& operator&=(ZExpr_Logical& iLHS, const ZExpr_Logical& iRHS);

ZExpr_Logical operator|(const ZExpr_Logical& iLHS, const ZExpr_Logical& iRHS);
ZExpr_Logical& operator|=(ZExpr_Logical& iLHS, const ZExpr_Logical& iRHS);

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Logical__
