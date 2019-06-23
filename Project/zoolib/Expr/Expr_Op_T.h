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

#ifndef __ZooLib_Expr_Expr_Op_T_h__
#define __ZooLib_Expr_Expr_Op_T_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - Expr_Op0

template <class T>
class Visitor_Expr_Op0_T;

template <class T>
class Expr_Op0_T
:	public virtual Expr
	{
protected:
	Expr_Op0_T()
		{}

public:
	virtual ~Expr_Op0_T()
		{}

// From Visitee
	virtual void Accept(const Visitor& iVisitor)
		{
		if (Visitor_Expr_Op0_T<T>* theVisitor = sDynNonConst<Visitor_Expr_Op0_T<T>>(&iVisitor))
			this->Accept_Expr_Op0(*theVisitor);
		else
			Expr::Accept(iVisitor);
		}

// From Expr
	virtual void Accept_Expr(Visitor_Expr& iVisitor)
		{
		if (Visitor_Expr_Op0_T<T>* theVisitor = sDynNonConst<Visitor_Expr_Op0_T<T>>(&iVisitor))
			{
			this->Accept_Expr_Op0(*theVisitor);
			}
		else
			{
			Expr::Accept_Expr(iVisitor);
			}
		}

// Our protocol
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<T>& iVisitor)
		{ iVisitor.Visit_Expr_Op0(this); }

	virtual ZP<T> Self() = 0;
	virtual ZP<T> Clone() = 0;

	ZP<T> SelfOrClone()
		{ return this->Self(); }
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Op0

template <class T>
class Visitor_Expr_Op0_T
:	public virtual Visitor_Expr
	{
public:
// Our protocol
	virtual void Visit_Expr_Op0(const ZP<Expr_Op0_T<T>>& iExpr)
		{ this->Visit_Expr(iExpr); }
	};

// =================================================================================================
#pragma mark - Expr_Op1_T

template <class T>
class Visitor_Expr_Op1_T;

template <class T>
class Expr_Op1_T
:	public virtual Expr
	{
protected:
	Expr_Op1_T(const ZP<T>& iOp0)
	:	fOp0(iOp0)
		{
		ZAssert(fOp0);
		}

public:
	virtual ~Expr_Op1_T()
		{}

// From Visitee
	virtual void Accept(const Visitor& iVisitor)
		{
		if (Visitor_Expr_Op1_T<T>* theVisitor = sDynNonConst<Visitor_Expr_Op1_T<T>>(&iVisitor))
			this->Accept_Expr_Op1(*theVisitor);
		else
			Expr::Accept(iVisitor);
		}

// From Expr
	virtual void Accept_Expr(Visitor_Expr& iVisitor)
		{
		if (Visitor_Expr_Op1_T<T>* theVisitor = sDynNonConst<Visitor_Expr_Op1_T<T>>(&iVisitor))
			this->Accept_Expr_Op1(*theVisitor);
		else
			Expr::Accept_Expr(iVisitor);
		}

// Our protocol
	virtual void Accept_Expr_Op1(Visitor_Expr_Op1_T<T>& iVisitor)
		{ iVisitor.Visit_Expr_Op1(this); }

	virtual ZP<T> Self() = 0;
	virtual ZP<T> Clone(const ZP<T>& iOp0) = 0;

	ZP<T> SelfOrClone(const ZP<T>& iOp0)
		{
		if (iOp0 == fOp0)
			return this->Self();
		return this->Clone(iOp0);
		}

	const ZP<T>& GetOp0() const
		{ return fOp0; }

private:
	const ZP<T> fOp0;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Op1_T

template <class T>
class Visitor_Expr_Op1_T
:	public virtual Visitor_Expr
	{
public:
// Our protocol
	virtual void Visit_Expr_Op1(const ZP<Expr_Op1_T<T>>& iExpr)
		{ this->Visit_Expr(iExpr); }
	};

// =================================================================================================
#pragma mark - Expr_Op2_T

template <class T>
class Visitor_Expr_Op2_T;

template <class T>
class Expr_Op2_T
:	public virtual Expr
	{
protected:
	Expr_Op2_T(const ZP<T>& iOp0, const ZP<T>& iOp1)
	:	fOp0(iOp0)
	,	fOp1(iOp1)
		{
		ZAssert(fOp0 && fOp1);
		}

public:
	virtual ~Expr_Op2_T()
		{}

// From Visitee
	virtual void Accept(const Visitor& iVisitor)
		{
		if (Visitor_Expr_Op2_T<T>* theVisitor = sDynNonConst<Visitor_Expr_Op2_T<T>>(&iVisitor))
			this->Accept_Expr_Op2(*theVisitor);
		else
			Expr::Accept(iVisitor);
		}

// From Expr
	virtual void Accept_Expr(Visitor_Expr& iVisitor)
		{
		if (Visitor_Expr_Op2_T<T>* theVisitor = sDynNonConst<Visitor_Expr_Op2_T<T>>(&iVisitor))
			this->Accept_Expr_Op2(*theVisitor);
		else
			Expr::Accept_Expr(iVisitor);
		}

// Our protocol
	virtual void Accept_Expr_Op2(Visitor_Expr_Op2_T<T>& iVisitor)
		{ iVisitor.Visit_Expr_Op2(this); }

	virtual ZP<T> Self() = 0;
	virtual ZP<T> Clone(const ZP<T>& iOp0, const ZP<T>& iOp1) = 0;

	ZP<T> SelfOrClone(const ZP<T>& iOp0, const ZP<T>& iOp1)
		{
		if (iOp0 == fOp0 && iOp1 == fOp1)
			return this->Self();
		return this->Clone(iOp0, iOp1);
		}

	const ZP<T>& GetOp0() const
		{ return fOp0; }

	const ZP<T>& GetOp1() const
		{ return fOp1; }

private:
	const ZP<T> fOp0;
	const ZP<T> fOp1;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Op2_T

template <class T>
class Visitor_Expr_Op2_T
:	public virtual Visitor_Expr
	{
public:
// Our protocol
	virtual void Visit_Expr_Op2(const ZP<Expr_Op2_T<T>>& iExpr)
		{ this->Visit_Expr(iExpr); }
	};

// =================================================================================================
#pragma mark - Expr_OpN_T

template <class T>
class Visitor_Expr_OpN_T;

template <class T>
class Expr_OpN_T
:	public virtual Expr
	{
protected:
	Expr_OpN_T(const std::vector<ZP<T>>& iOps)
	:	fOps(iOps)
		{}

public:
	virtual ~Expr_OpN_T()
		{}

// From Visitee
	virtual void Accept(const Visitor& iVisitor)
		{
		if (Visitor_Expr_OpN_T<T>* theVisitor = sDynNonConst<Visitor_Expr_OpN_T<T>>(&iVisitor))
			this->Accept_Expr_OpN(*theVisitor);
		else
			Expr::Accept(iVisitor);
		}

// From Expr
	virtual void Accept_Expr(Visitor_Expr& iVisitor)
		{
		if (Visitor_Expr_OpN_T<T>* theVisitor = sDynNonConst<Visitor_Expr_OpN_T<T>>(&iVisitor))
			this->Accept_Expr_OpN(*theVisitor);
		else
			Expr::Accept_Expr(iVisitor);
		}

// Our protocol
	virtual void Accept_Expr_OpN(Visitor_Expr_OpN_T<T>& iVisitor)
		{ iVisitor.Visit_Expr_OpN(this); }

	virtual ZP<T> Self() = 0;
	virtual ZP<T> Clone(const std::vector<ZP<T>>& iOps) = 0;

	ZP<T> SelfOrClone(const std::vector<ZP<T>>& iOps)
		{
		if (iOps == fOps)
			return this->Self();
		return this->Clone(iOps);
		}

	const std::vector<ZP<T>>& GetOps() const
		{ return fOps; }

private:
	std::vector<ZP<T>> fOps;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_OpN_T

template <class T>
class Visitor_Expr_OpN_T
:	public virtual Visitor_Expr
	{
public:
// Our protocol
	virtual void Visit_Expr_OpN(const ZP<Expr_OpN_T<T>>& iExpr)
		{ this->Visit_Expr(iExpr); }
	};

} // namespace ZooLib

#endif // __ZooLib_Expr_Expr_Op_T_h__
