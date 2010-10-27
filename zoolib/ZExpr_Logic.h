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

#include "zoolib/ZExpr_Op_T.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic

class ZExpr_Logic
:	public virtual ZExpr
	{
protected:
	ZExpr_Logic();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_True

class ZVisitor_Expr_Logic_True;

class ZExpr_Logic_True
:	public virtual ZExpr_Logic
,	public virtual ZExpr_Op0_T<ZExpr_Logic>
	{
	typedef ZExpr_Op0_T<ZExpr_Logic> inherited;
public:
	ZExpr_Logic_True();

// From ZExpr_Op0_T
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Logic>& iVisitor);

	virtual ZRef<ZExpr_Logic> Self();
	virtual ZRef<ZExpr_Logic> Clone();

// Our protocol
	virtual void Accept_Expr_Logic_True(ZVisitor_Expr_Logic_True& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_True

class ZVisitor_Expr_Logic_True
:	public virtual ZVisitor_Expr_Op0_T<ZExpr_Logic>
	{
	typedef ZVisitor_Expr_Op0_T<ZExpr_Logic> inherited;
public:
	virtual void Visit_Expr_Logic_True(ZRef<ZExpr_Logic_True> iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_False

class ZVisitor_Expr_Logic_False;

class ZExpr_Logic_False
:	public virtual ZExpr_Logic
,	public virtual ZExpr_Op0_T<ZExpr_Logic>
	{
	typedef ZExpr_Op0_T<ZExpr_Logic> inherited;
public:
	ZExpr_Logic_False();

// From ZExpr_Op0_T
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Logic>& iVisitor);

	virtual ZRef<ZExpr_Logic> Self();
	virtual ZRef<ZExpr_Logic> Clone();

// Our protocol
	virtual void Accept_Expr_Logic_False(ZVisitor_Expr_Logic_False& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_False

class ZVisitor_Expr_Logic_False
:	public virtual ZVisitor_Expr_Op0_T<ZExpr_Logic>
	{
	typedef ZVisitor_Expr_Op0_T<ZExpr_Logic> inherited;
public:
	virtual void Visit_Expr_Logic_False(ZRef<ZExpr_Logic_False> iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_Not

class ZVisitor_Expr_Logic_Not;

class ZExpr_Logic_Not
:	public virtual ZExpr_Logic
,	public virtual ZExpr_Op1_T<ZExpr_Logic>
	{
	typedef ZExpr_Op1_T<ZExpr_Logic> inherited;
public:
	ZExpr_Logic_Not(ZRef<ZExpr_Logic> iOp0);

// From ZExpr_Op1_T
	virtual void Accept_Expr_Op1(ZVisitor_Expr_Op1_T<ZExpr_Logic>& iVisitor);

	virtual ZRef<ZExpr_Logic> Self();
	virtual ZRef<ZExpr_Logic> Clone(ZRef<ZExpr_Logic> iOp0);

// Our protocol
	virtual void Accept_Expr_Logic_Not(ZVisitor_Expr_Logic_Not& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_Not

class ZVisitor_Expr_Logic_Not
:	public virtual ZVisitor_Expr_Op1_T<ZExpr_Logic>
	{
	typedef ZVisitor_Expr_Op1_T<ZExpr_Logic> inherited;
public:
	virtual void Visit_Expr_Logic_Not(ZRef<ZExpr_Logic_Not> iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_And

class ZVisitor_Expr_Logic_And;

class ZExpr_Logic_And
:	public virtual ZExpr_Logic
,	public virtual ZExpr_Op2_T<ZExpr_Logic>
	{
	typedef ZExpr_Op2_T<ZExpr_Logic> inherited;
public:
	ZExpr_Logic_And(ZRef<ZExpr_Logic> iOp0, ZRef<ZExpr_Logic> iOp1);

// From ZExpr_Op2_T
	virtual void Accept_Expr_Op2(ZVisitor_Expr_Op2_T<ZExpr_Logic>& iVisitor);

	virtual ZRef<ZExpr_Logic> Self();
	virtual ZRef<ZExpr_Logic> Clone(ZRef<ZExpr_Logic> iOp0, ZRef<ZExpr_Logic> iOp1);

// Our protocol
	virtual void Accept_Expr_Logic_And(ZVisitor_Expr_Logic_And& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_And

class ZVisitor_Expr_Logic_And
:	public virtual ZVisitor_Expr_Op2_T<ZExpr_Logic>
	{
	typedef ZVisitor_Expr_Op2_T<ZExpr_Logic> inherited;
public:
	virtual void Visit_Expr_Logic_And(ZRef<ZExpr_Logic_And> iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Logic_Or

class ZVisitor_Expr_Logic_Or;

class ZExpr_Logic_Or
:	public virtual ZExpr_Logic
,	public virtual ZExpr_Op2_T<ZExpr_Logic>
	{
	typedef ZExpr_Op2_T<ZExpr_Logic> inherited;
public:
	ZExpr_Logic_Or(ZRef<ZExpr_Logic> iOp0, ZRef<ZExpr_Logic> iOp1);

// From ZExpr_Op2_T
	virtual void Accept_Expr_Op2(ZVisitor_Expr_Op2_T<ZExpr_Logic>& iVisitor);

	virtual ZRef<ZExpr_Logic> Self();
	virtual ZRef<ZExpr_Logic> Clone(ZRef<ZExpr_Logic> iOp0, ZRef<ZExpr_Logic> iOp1);

// Our protocol
	virtual void Accept_Expr_Logic_Or(ZVisitor_Expr_Logic_Or& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_Or

class ZVisitor_Expr_Logic_Or
:	public virtual ZVisitor_Expr_Op2_T<ZExpr_Logic>
	{
	typedef ZVisitor_Expr_Op2_T<ZExpr_Logic> inherited;
public:
	virtual void Visit_Expr_Logic_Or(ZRef<ZExpr_Logic_Or> iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Operators

ZRef<ZExpr_Logic> sTrue();
ZRef<ZExpr_Logic> sFalse();

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

} // namespace ZooLib

#endif // __ZExpr_Logic__
