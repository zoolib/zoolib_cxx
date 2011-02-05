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

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Op0

template <class T>
class ZVisitor_Expr_Op0_T;

template <class T>
class ZExpr_Op0_T
:	public virtual ZExpr
	{
protected:
	ZExpr_Op0_T()
		{}

public:
	virtual ~ZExpr_Op0_T()
		{}

// From ZVisitee
	virtual void Accept(ZVisitor& iVisitor)
		{
		if (ZVisitor_Expr_Op0_T<T>* theVisitor = dynamic_cast<ZVisitor_Expr_Op0_T<T>*>(&iVisitor))
			this->Accept_Expr_Op0(*theVisitor);
		else
			ZExpr::Accept(iVisitor);
		}

// From ZExpr
	virtual void Accept_Expr(ZVisitor_Expr& iVisitor)
		{
		if (ZVisitor_Expr_Op0_T<T>* theVisitor = dynamic_cast<ZVisitor_Expr_Op0_T<T>*>(&iVisitor))
			this->Accept_Expr_Op0(*theVisitor);
		else
			ZExpr::Accept_Expr(iVisitor);
		}

// Our protocol
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<T>& iVisitor)
		{ iVisitor.Visit_Expr_Op0(this); }

	virtual ZRef<T> Self() = 0;
	virtual ZRef<T> Clone() = 0;
	
	ZRef<T> SelfOrClone()
		{ return this->Self(); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Op0

template <class T>
class ZVisitor_Expr_Op0_T
:	public virtual ZVisitor_Expr
	{
public:
// Our protocol
	virtual void Visit_Expr_Op0(const ZRef<ZExpr_Op0_T<T> >& iExpr)
		{ this->Visit_Expr(iExpr); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Op1_T

template <class T>
class ZVisitor_Expr_Op1_T;

template <class T>
class ZExpr_Op1_T
:	public virtual ZExpr
	{
protected:
	ZExpr_Op1_T(const ZRef<T>& iOp0)
	:	fOp0(iOp0)
		{}

public:
	virtual ~ZExpr_Op1_T()
		{}

// From ZVisitee
	virtual void Accept(ZVisitor& iVisitor)
		{
		if (ZVisitor_Expr_Op1_T<T>* theVisitor = dynamic_cast<ZVisitor_Expr_Op1_T<T>*>(&iVisitor))
			this->Accept_Expr_Op1(*theVisitor);
		else
			ZExpr::Accept(iVisitor);
		}

// From ZExpr
	virtual void Accept_Expr(ZVisitor_Expr& iVisitor)
		{
		if (ZVisitor_Expr_Op1_T<T>* theVisitor = dynamic_cast<ZVisitor_Expr_Op1_T<T>*>(&iVisitor))
			this->Accept_Expr_Op1(*theVisitor);
		else
			ZExpr::Accept_Expr(iVisitor);
		}

// Our protocol
	virtual void Accept_Expr_Op1(ZVisitor_Expr_Op1_T<T>& iVisitor)
		{ iVisitor.Visit_Expr_Op1(this); }

	virtual ZRef<T> Self() = 0;
	virtual ZRef<T> Clone(const ZRef<T>& iOp0) = 0;

	ZRef<T> SelfOrClone(const ZRef<T>& iOp0)
		{
		if (iOp0 == fOp0)
			return this->Self();
		return this->Clone(iOp0);
		}

	const ZRef<T>& GetOp0() const
		{ return fOp0; }

private:
	const ZRef<T> fOp0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Op1_T

template <class T>
class ZVisitor_Expr_Op1_T
:	public virtual ZVisitor_Expr
	{
public:
// Our protocol
	virtual void Visit_Expr_Op1(const ZRef<ZExpr_Op1_T<T> >& iExpr)
		{ this->Visit_Expr(iExpr); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Op2_T

template <class T>
class ZVisitor_Expr_Op2_T;

template <class T>
class ZExpr_Op2_T
:	public virtual ZExpr
	{
protected:
	ZExpr_Op2_T(const ZRef<T>& iOp0, const ZRef<T>& iOp1)
	:	fOp0(iOp0)
	,	fOp1(iOp1)
		{}

public:
	virtual ~ZExpr_Op2_T()
		{}

// From ZVisitee
	virtual void Accept(ZVisitor& iVisitor)
		{
		if (ZVisitor_Expr_Op2_T<T>* theVisitor = dynamic_cast<ZVisitor_Expr_Op2_T<T>*>(&iVisitor))
			this->Accept_Expr_Op2(*theVisitor);
		else
			ZExpr::Accept(iVisitor);
		}

// From ZExpr
	virtual void Accept_Expr(ZVisitor_Expr& iVisitor)
		{
		if (ZVisitor_Expr_Op2_T<T>* theVisitor = dynamic_cast<ZVisitor_Expr_Op2_T<T>*>(&iVisitor))
			this->Accept_Expr_Op2(*theVisitor);
		else
			ZExpr::Accept_Expr(iVisitor);
		}

// Our protocol
	virtual void Accept_Expr_Op2(ZVisitor_Expr_Op2_T<T>& iVisitor)
		{ iVisitor.Visit_Expr_Op2(this); }

	virtual ZRef<T> Self() = 0;
	virtual ZRef<T> Clone(const ZRef<T>& iOp0, const ZRef<T>& iOp1) = 0;

	ZRef<T> SelfOrClone(const ZRef<T>& iOp0, const ZRef<T>& iOp1)
		{
		if (iOp0 == fOp0 && iOp1 == fOp1)
			return this->Self();
		return this->Clone(iOp0, iOp1);
		}

	const ZRef<T>& GetOp0() const
		{ return fOp0; }

	const ZRef<T>& GetOp1() const
		{ return fOp1; }

private:
	const ZRef<T> fOp0;
	const ZRef<T> fOp1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Op2_T

template <class T>
class ZVisitor_Expr_Op2_T
:	public virtual ZVisitor_Expr
	{
public:
// Our protocol
	virtual void Visit_Expr_Op2(const ZRef<ZExpr_Op2_T<T> >& iExpr)
		{ this->Visit_Expr(iExpr); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_OpN_T

template <class T>
class ZVisitor_Expr_OpN_T;

template <class T>
class ZExpr_OpN_T
:	public virtual ZExpr
	{
protected:
	ZExpr_OpN_T(const std::vector<ZRef<T> >& iOps)
	:	fOps(iOps)
		{}

public:
	virtual ~ZExpr_OpN_T()
		{}

// From ZVisitee
	virtual void Accept(ZVisitor& iVisitor)
		{
		if (ZVisitor_Expr_OpN_T<T>* theVisitor = dynamic_cast<ZVisitor_Expr_OpN_T<T>*>(&iVisitor))
			this->Accept_Expr_OpN(*theVisitor);
		else
			ZExpr::Accept(iVisitor);
		}

// From ZExpr
	virtual void Accept_Expr(ZVisitor_Expr& iVisitor)
		{
		if (ZVisitor_Expr_OpN_T<T>* theVisitor = dynamic_cast<ZVisitor_Expr_OpN_T<T>*>(&iVisitor))
			this->Accept_Expr_OpN(*theVisitor);
		else
			ZExpr::Accept_Expr(iVisitor);
		}

// Our protocol
	virtual void Accept_Expr_OpN(ZVisitor_Expr_OpN_T<T>& iVisitor)
		{ iVisitor.Visit_Expr_OpN(this); }

	virtual ZRef<T> Self() = 0;
	virtual ZRef<T> Clone(const std::vector<ZRef<T> >& iOps) = 0;

	ZRef<T> SelfOrClone(const std::vector<ZRef<T> >& iOps)
		{
		if (iOps == fOps)
			return this->Self();
		return this->Clone(iOps);
		}

	const std::vector<ZRef<T> >& GetOps() const
		{ return fOps; }

private:
	std::vector<ZRef<T> > fOps;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_OpN_T

template <class T>
class ZVisitor_Expr_OpN_T
:	public virtual ZVisitor_Expr
	{
public:
// Our protocol
	virtual void Visit_Expr_OpN(const ZRef<ZExpr_OpN_T<T> >& iExpr)
		{ this->Visit_Expr(iExpr); }
	};

} // namespace ZooLib

#endif // __ZExpr_Op_T__
