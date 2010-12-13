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

#ifndef __ZExpr_Bool__
#define __ZExpr_Bool__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Op_T.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool

class ZExpr_Bool
:	public virtual ZExpr
	{
protected:
	ZExpr_Bool();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool_True

class ZVisitor_Expr_Bool_True;

class ZExpr_Bool_True
:	public virtual ZExpr_Bool
,	public virtual ZExpr_Op0_T<ZExpr_Bool>
	{
	typedef ZExpr_Op0_T<ZExpr_Bool> inherited;
public:
	ZExpr_Bool_True();

// From ZExpr_Op0_T
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Bool>& iVisitor);

	virtual ZRef<ZExpr_Bool> Self();
	virtual ZRef<ZExpr_Bool> Clone();

// Our protocol
	virtual void Accept_Expr_Bool_True(ZVisitor_Expr_Bool_True& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_True

class ZVisitor_Expr_Bool_True
:	public virtual ZVisitor_Expr_Op0_T<ZExpr_Bool>
	{
	typedef ZVisitor_Expr_Op0_T<ZExpr_Bool> inherited;
public:
	virtual void Visit_Expr_Bool_True(ZRef<ZExpr_Bool_True> iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool_False

class ZVisitor_Expr_Bool_False;

class ZExpr_Bool_False
:	public virtual ZExpr_Bool
,	public virtual ZExpr_Op0_T<ZExpr_Bool>
	{
	typedef ZExpr_Op0_T<ZExpr_Bool> inherited;
public:
	ZExpr_Bool_False();

// From ZExpr_Op0_T
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Bool>& iVisitor);

	virtual ZRef<ZExpr_Bool> Self();
	virtual ZRef<ZExpr_Bool> Clone();

// Our protocol
	virtual void Accept_Expr_Bool_False(ZVisitor_Expr_Bool_False& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_False

class ZVisitor_Expr_Bool_False
:	public virtual ZVisitor_Expr_Op0_T<ZExpr_Bool>
	{
	typedef ZVisitor_Expr_Op0_T<ZExpr_Bool> inherited;
public:
	virtual void Visit_Expr_Bool_False(ZRef<ZExpr_Bool_False> iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool_Not

class ZVisitor_Expr_Bool_Not;

class ZExpr_Bool_Not
:	public virtual ZExpr_Bool
,	public virtual ZExpr_Op1_T<ZExpr_Bool>
	{
	typedef ZExpr_Op1_T<ZExpr_Bool> inherited;
public:
	ZExpr_Bool_Not(ZRef<ZExpr_Bool> iOp0);

// From ZExpr_Op1_T
	virtual void Accept_Expr_Op1(ZVisitor_Expr_Op1_T<ZExpr_Bool>& iVisitor);

	virtual ZRef<ZExpr_Bool> Self();
	virtual ZRef<ZExpr_Bool> Clone(ZRef<ZExpr_Bool> iOp0);

// Our protocol
	virtual void Accept_Expr_Bool_Not(ZVisitor_Expr_Bool_Not& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_Not

class ZVisitor_Expr_Bool_Not
:	public virtual ZVisitor_Expr_Op1_T<ZExpr_Bool>
	{
	typedef ZVisitor_Expr_Op1_T<ZExpr_Bool> inherited;
public:
	virtual void Visit_Expr_Bool_Not(ZRef<ZExpr_Bool_Not> iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool_And

class ZVisitor_Expr_Bool_And;

class ZExpr_Bool_And
:	public virtual ZExpr_Bool
,	public virtual ZExpr_Op2_T<ZExpr_Bool>
	{
	typedef ZExpr_Op2_T<ZExpr_Bool> inherited;
public:
	ZExpr_Bool_And(ZRef<ZExpr_Bool> iOp0, ZRef<ZExpr_Bool> iOp1);

// From ZExpr_Op2_T
	virtual void Accept_Expr_Op2(ZVisitor_Expr_Op2_T<ZExpr_Bool>& iVisitor);

	virtual ZRef<ZExpr_Bool> Self();
	virtual ZRef<ZExpr_Bool> Clone(ZRef<ZExpr_Bool> iOp0, ZRef<ZExpr_Bool> iOp1);

// Our protocol
	virtual void Accept_Expr_Bool_And(ZVisitor_Expr_Bool_And& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_And

class ZVisitor_Expr_Bool_And
:	public virtual ZVisitor_Expr_Op2_T<ZExpr_Bool>
	{
	typedef ZVisitor_Expr_Op2_T<ZExpr_Bool> inherited;
public:
	virtual void Visit_Expr_Bool_And(ZRef<ZExpr_Bool_And> iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZExpr_Bool_Or

class ZVisitor_Expr_Bool_Or;

class ZExpr_Bool_Or
:	public virtual ZExpr_Bool
,	public virtual ZExpr_Op2_T<ZExpr_Bool>
	{
	typedef ZExpr_Op2_T<ZExpr_Bool> inherited;
public:
	ZExpr_Bool_Or(ZRef<ZExpr_Bool> iOp0, ZRef<ZExpr_Bool> iOp1);

// From ZExpr_Op2_T
	virtual void Accept_Expr_Op2(ZVisitor_Expr_Op2_T<ZExpr_Bool>& iVisitor);

	virtual ZRef<ZExpr_Bool> Self();
	virtual ZRef<ZExpr_Bool> Clone(ZRef<ZExpr_Bool> iOp0, ZRef<ZExpr_Bool> iOp1);

// Our protocol
	virtual void Accept_Expr_Bool_Or(ZVisitor_Expr_Bool_Or& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_Or

class ZVisitor_Expr_Bool_Or
:	public virtual ZVisitor_Expr_Op2_T<ZExpr_Bool>
	{
	typedef ZVisitor_Expr_Op2_T<ZExpr_Bool> inherited;
public:
	virtual void Visit_Expr_Bool_Or(ZRef<ZExpr_Bool_Or> iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Operators

ZRef<ZExpr_Bool> sTrue();
ZRef<ZExpr_Bool> sFalse();

ZRef<ZExpr_Bool_Not> operator~(const ZRef<ZExpr_Bool>& iExpr_Bool);
ZRef<ZExpr_Bool> operator~(const ZRef<ZExpr_Bool_Not>& iExpr_Bool_Not);

ZRef<ZExpr_Bool_Not> sNot(const ZRef<ZExpr_Bool>& iExpr_Bool);
ZRef<ZExpr_Bool> sNot(const ZRef<ZExpr_Bool_Not>& iExpr_Bool_Not);

ZRef<ZExpr_Bool> operator&(bool iBool, const ZRef<ZExpr_Bool>& iExpr_Bool);
ZRef<ZExpr_Bool> operator&(const ZRef<ZExpr_Bool>& iExpr_Bool, bool iBool);

ZRef<ZExpr_Bool> operator|(bool iBool, const ZRef<ZExpr_Bool>& iExpr_Bool);
ZRef<ZExpr_Bool> operator|(const ZRef<ZExpr_Bool>& iExpr_Bool, bool iBool);

ZRef<ZExpr_Bool> operator&(const ZRef<ZExpr_Bool>& iLHS, const ZRef<ZExpr_Bool>& iRHS);
ZRef<ZExpr_Bool>& operator&=(ZRef<ZExpr_Bool>& iLHS, const ZRef<ZExpr_Bool>& iRHS);

ZRef<ZExpr_Bool> operator|(const ZRef<ZExpr_Bool>& iLHS, const ZRef<ZExpr_Bool>& iRHS);
ZRef<ZExpr_Bool>& operator|=(ZRef<ZExpr_Bool>& iLHS, const ZRef<ZExpr_Bool>& iRHS);

} // namespace ZooLib

#endif // __ZExpr_Bool__
