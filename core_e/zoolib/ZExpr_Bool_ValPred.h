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

#ifndef __ZExpr_Bool_ValPred_h__
#define __ZExpr_Bool_ValPred_h__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Bool.h"
#include "zoolib/ZValPred.h"

namespace ZooLib {

class ZVisitor_Expr_Bool_ValPred;

// =================================================================================================
// MARK: - ZExpr_Bool_ValPred

class ZExpr_Bool_ValPred
:	public virtual ZExpr_Bool
,	public virtual ZExpr_Op0_T<ZExpr_Bool>
	{
	typedef ZExpr_Op0_T<ZExpr_Bool> inherited;
public:
	ZExpr_Bool_ValPred(const ZValPred& iValPred);
	virtual ~ZExpr_Bool_ValPred();

// From ZVisitee
	virtual void Accept(const ZVisitor& iVisitor);

// From ZExpr_Op0
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<ZExpr_Bool>& iVisitor);

	virtual ZRef<ZExpr_Bool> Self();
	virtual ZRef<ZExpr_Bool> Clone();

// Our protocol
	virtual void Accept_Expr_Bool_ValPred(ZVisitor_Expr_Bool_ValPred& iVisitor);

	const ZValPred& GetValPred() const;

private:
	const ZValPred fValPred;
	};

template <>
int sCompare_T(const ZExpr_Bool_ValPred& iL, const ZExpr_Bool_ValPred& iR);

// =================================================================================================
// MARK: - ZVisitor_Expr_Bool_ValPred

class ZVisitor_Expr_Bool_ValPred
:	public virtual ZVisitor_Expr_Op0_T<ZExpr_Bool>
	{
public:
	virtual void Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred >& iExpr);
	};

// =================================================================================================
// MARK: - Operators

ZRef<ZExpr_Bool> sExpr_Bool(const ZValPred& iValPred);

ZRef<ZExpr_Bool> operator~(const ZValPred& iValPred);

ZRef<ZExpr_Bool> operator&(bool iBool, const ZValPred& iValPred);

ZRef<ZExpr_Bool> operator&(const ZValPred& iValPred, bool iBool);

ZRef<ZExpr_Bool> operator|(bool iBool, const ZValPred& iValPred);

ZRef<ZExpr_Bool> operator|(const ZValPred& iValPred, bool iBool);

ZRef<ZExpr_Bool> operator&(const ZValPred& iLHS, const ZValPred& iRHS);

ZRef<ZExpr_Bool> operator&(const ZValPred& iLHS, const ZRef<ZExpr_Bool>& iRHS);

ZRef<ZExpr_Bool> operator&(const ZRef<ZExpr_Bool>& iLHS, const ZValPred& iRHS);

ZRef<ZExpr_Bool>& operator&=(ZRef<ZExpr_Bool>& ioLHS, const ZValPred& iRHS);

ZRef<ZExpr_Bool> operator|(const ZValPred& iLHS, const ZValPred& iRHS);

ZRef<ZExpr_Bool> operator|(const ZValPred& iLHS, const ZRef<ZExpr_Bool>& iRHS);

ZRef<ZExpr_Bool> operator|(const ZRef<ZExpr_Bool>& iLHS, const ZValPred& iRHS);

ZRef<ZExpr_Bool>& operator|=(ZRef<ZExpr_Bool>& ioLHS, const ZValPred& iRHS);

} // namespace ZooLib

#endif // __ZExpr_Bool_ValPred_h__
