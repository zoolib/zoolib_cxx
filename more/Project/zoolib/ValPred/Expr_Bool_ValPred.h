/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZooLib_ValPred_Expr_Bool_ValPred_h__
#define __ZooLib_ValPred_Expr_Bool_ValPred_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr_Bool.h"
#include "zoolib/ValPred/ValPred.h"

namespace ZooLib {

class Visitor_Expr_Bool_ValPred;

// =================================================================================================
// MARK: - Expr_Bool_ValPred

class Expr_Bool_ValPred
:	public virtual Expr_Bool
,	public virtual Expr_Op0_T<Expr_Bool>
	{
	typedef Expr_Op0_T<Expr_Bool> inherited;
public:
	Expr_Bool_ValPred(const ValPred& iValPred);
	virtual ~Expr_Bool_ValPred();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op0
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Bool>& iVisitor);

	virtual ZRef<Expr_Bool> Self();
	virtual ZRef<Expr_Bool> Clone();

// Our protocol
	virtual void Accept_Expr_Bool_ValPred(Visitor_Expr_Bool_ValPred& iVisitor);

	const ValPred& GetValPred() const;

private:
	const ValPred fValPred;
	};

template <>
int sCompare_T(const Expr_Bool_ValPred& iL, const Expr_Bool_ValPred& iR);

// =================================================================================================
// MARK: - Visitor_Expr_Bool_ValPred

class Visitor_Expr_Bool_ValPred
:	public virtual Visitor_Expr_Op0_T<Expr_Bool>
	{
public:
	virtual void Visit_Expr_Bool_ValPred(const ZRef<Expr_Bool_ValPred >& iExpr);
	};

// =================================================================================================
// MARK: - Operators

ZRef<Expr_Bool> sExpr_Bool(const ValPred& iValPred);

ZRef<Expr_Bool> operator~(const ValPred& iValPred);

ZRef<Expr_Bool> operator&(bool iBool, const ValPred& iValPred);

ZRef<Expr_Bool> operator&(const ValPred& iValPred, bool iBool);

ZRef<Expr_Bool> operator|(bool iBool, const ValPred& iValPred);

ZRef<Expr_Bool> operator|(const ValPred& iValPred, bool iBool);

ZRef<Expr_Bool> operator&(const ValPred& iLHS, const ValPred& iRHS);

ZRef<Expr_Bool> operator&(const ValPred& iLHS, const ZRef<Expr_Bool>& iRHS);

ZRef<Expr_Bool> operator&(const ZRef<Expr_Bool>& iLHS, const ValPred& iRHS);

ZRef<Expr_Bool>& operator&=(ZRef<Expr_Bool>& ioLHS, const ValPred& iRHS);

ZRef<Expr_Bool> operator|(const ValPred& iLHS, const ValPred& iRHS);

ZRef<Expr_Bool> operator|(const ValPred& iLHS, const ZRef<Expr_Bool>& iRHS);

ZRef<Expr_Bool> operator|(const ZRef<Expr_Bool>& iLHS, const ValPred& iRHS);

ZRef<Expr_Bool>& operator|=(ZRef<Expr_Bool>& ioLHS, const ValPred& iRHS);

} // namespace ZooLib

#endif // __ZooLib_ValPred_Expr_Bool_ValPred_h__
