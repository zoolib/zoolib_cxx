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

#ifndef __ZExpr_Op_T__
#define __ZExpr_Op_T__ 1
#include "zconfig.h"

#include "zoolib/ZExpr.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Op0

template <class T>
class ZVisitor_Expr_Op0_T;

template <class T>
class ZExpr_Op0_T : public virtual ZExpr
	{
protected:
	ZExpr_Op0_T()
		{}

public:
	virtual ~ZExpr_Op0_T()
		{}

// From ZExpr
	virtual void Accept_Expr(ZVisitor_Expr& iVisitor)
		{
		if (ZVisitor_Expr_Op0_T<T>* theVisitor =
			dynamic_cast<ZVisitor_Expr_Op0_T<T>*>(&iVisitor))
			{
			this->Accept_Expr_Op0(*theVisitor);
			}
		else
			{
			ZExpr::Accept_Expr(iVisitor);
			}
		}

// Our protocol
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<T>& iVisitor)
		{ iVisitor.Visit_Expr_Op0(this); }

	virtual ZRef<T> Self() = 0;
	virtual ZRef<T> Clone() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Op0

template <class T>
class ZVisitor_Expr_Op0_T : public virtual ZVisitor_Expr
	{
public:
// Our protocol
	virtual void Visit_Expr_Op0(ZRef<ZExpr_Op0_T<T> > iExpr)
		{ ZVisitor_Expr::Visit_Expr(iExpr); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Op1_T

template <class T>
class ZVisitor_Expr_Op1_T;

template <class T>
class ZExpr_Op1_T : public virtual ZExpr
	{
protected:
	ZExpr_Op1_T(ZRef<T> iOp0)
	:	fOp0(iOp0)
		{}

public:
	virtual ~ZExpr_Op1_T()
		{}

// From ZExpr
	virtual void Accept_Expr(ZVisitor_Expr& iVisitor)
		{
		if (ZVisitor_Expr_Op1_T<T>* theVisitor =
			dynamic_cast<ZVisitor_Expr_Op1_T<T>*>(&iVisitor))
			{
			this->Accept_Expr_Op1(*theVisitor);
			}
		else
			{
			ZExpr::Accept_Expr(iVisitor);
			}
		}

// Our protocol
	virtual void Accept_Expr_Op1(ZVisitor_Expr_Op1_T<T>& iVisitor)
		{ iVisitor.Visit_Expr_Op1(this); }

	virtual ZRef<T> Self() = 0;
	virtual ZRef<T> Clone(ZRef<T> iOp0) = 0;

	ZRef<T> GetOp0()
		{ return fOp0; }

private:
	const ZRef<T> fOp0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Op1_T

template <class T>
class ZVisitor_Expr_Op1_T : public virtual ZVisitor_Expr
	{
public:
// Our protocol
	virtual void Visit_Expr_Op1(ZRef<ZExpr_Op1_T<T> > iExpr)
		{ ZVisitor_Expr::Visit_Expr(iExpr); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Op2_T

template <class T>
class ZVisitor_Expr_Op2_T;

template <class T>
class ZExpr_Op2_T : public virtual ZExpr
	{
protected:
	ZExpr_Op2_T(ZRef<T> iOp0, ZRef<T> iOp1)
	:	fOp0(iOp0)
	,	fOp1(iOp1)
		{}

public:
	virtual ~ZExpr_Op2_T()
		{}

// From ZExpr
	virtual void Accept_Expr(ZVisitor_Expr& iVisitor)
		{
		if (ZVisitor_Expr_Op2_T<T>* theVisitor =
			dynamic_cast<ZVisitor_Expr_Op2_T<T>*>(&iVisitor))
			{
			this->Accept_Expr_Op2(*theVisitor);
			}
		else
			{
			ZExpr::Accept_Expr(iVisitor);
			}
		}

// Our protocol
	virtual void Accept_Expr_Op2(ZVisitor_Expr_Op2_T<T>& iVisitor)
		{ iVisitor.Visit_Expr_Op2(this); }

	virtual ZRef<T> Self() = 0;
	virtual ZRef<T> Clone(ZRef<T> iOp0, ZRef<T> iOp1) = 0;

	ZRef<T> GetOp0()
		{ return fOp0; }

	ZRef<T> GetOp1()
		{ return fOp1; }

private:
	const ZRef<T> fOp0;
	const ZRef<T> fOp1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Op2_T

template <class T>
class ZVisitor_Expr_Op2_T : public virtual ZVisitor_Expr
	{
public:
// Our protocol
	virtual void Visit_Expr_Op2(ZRef<ZExpr_Op2_T<T> > iExpr)
		{ ZVisitor_Expr::Visit_Expr(iExpr); }
	};

NAMESPACE_ZOOLIB_END

#endif // __ZExpr_Op_T__
