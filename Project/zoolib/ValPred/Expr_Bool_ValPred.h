// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ValPred_Expr_Bool_ValPred_h__
#define __ZooLib_ValPred_Expr_Bool_ValPred_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr_Bool.h"
#include "zoolib/ValPred/ValPred.h"

namespace ZooLib {

class Visitor_Expr_Bool_ValPred;

// =================================================================================================
#pragma mark - Expr_Bool_ValPred

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

// From Expr
	virtual int Compare(const ZP<Expr>& iOther);

// From Expr_Op0
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Bool>& iVisitor);

	virtual ZP<Expr_Bool> Self();
	virtual ZP<Expr_Bool> Clone();

// Our protocol
	virtual void Accept_Expr_Bool_ValPred(Visitor_Expr_Bool_ValPred& iVisitor);

	const ValPred& GetValPred() const;

private:
	const ValPred fValPred;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_ValPred

class Visitor_Expr_Bool_ValPred
:	public virtual Visitor_Expr_Op0_T<Expr_Bool>
	{
public:
	virtual void Visit_Expr_Bool_ValPred(const ZP<Expr_Bool_ValPred >& iExpr);
	};

// =================================================================================================
#pragma mark - Operators

ZP<Expr_Bool> sExpr_Bool(const ValPred& iValPred);

ZP<Expr_Bool> operator~(const ValPred& iValPred);

ZP<Expr_Bool> operator&(bool iBool, const ValPred& iValPred);

ZP<Expr_Bool> operator&(const ValPred& iValPred, bool iBool);

ZP<Expr_Bool> operator|(bool iBool, const ValPred& iValPred);

ZP<Expr_Bool> operator|(const ValPred& iValPred, bool iBool);

ZP<Expr_Bool> operator&(const ValPred& iLHS, const ValPred& iRHS);

ZP<Expr_Bool> operator&(const ValPred& iLHS, const ZP<Expr_Bool>& iRHS);

ZP<Expr_Bool> operator&(const ZP<Expr_Bool>& iLHS, const ValPred& iRHS);

ZP<Expr_Bool>& operator&=(ZP<Expr_Bool>& ioLHS, const ValPred& iRHS);

ZP<Expr_Bool> operator|(const ValPred& iLHS, const ValPred& iRHS);

ZP<Expr_Bool> operator|(const ValPred& iLHS, const ZP<Expr_Bool>& iRHS);

ZP<Expr_Bool> operator|(const ZP<Expr_Bool>& iLHS, const ValPred& iRHS);

ZP<Expr_Bool>& operator|=(ZP<Expr_Bool>& ioLHS, const ValPred& iRHS);

} // namespace ZooLib

#endif // __ZooLib_ValPred_Expr_Bool_ValPred_h__
