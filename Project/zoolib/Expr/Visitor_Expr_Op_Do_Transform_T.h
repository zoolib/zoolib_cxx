// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Expr_Visitor_Expr_Op_Do_Transform_T_h__
#define __ZooLib_Expr_Visitor_Expr_Op_Do_Transform_T_h__ 1
#include "zconfig.h"

#include "zoolib/Visitor_Do_T.h"

#include "zoolib/Expr/Expr_Op_T.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_Expr_Op_Do_Transform_T

template <class T>
class Visitor_Expr_Op_Do_Transform_T
:	public virtual Visitor_Do_T<ZP<T>>
,	public virtual Visitor_Expr_Op0_T<T>
,	public virtual Visitor_Expr_Op1_T<T>
,	public virtual Visitor_Expr_Op2_T<T>
	{
public:
// From Visitor
	virtual void Visit(const ZP<Visitee>& iRep)
		{ ZUnimplemented(); }

// From Visitor_Expr_Op0_T
	virtual void Visit_Expr_Op0(const ZP<Expr_Op0_T<T>>& iExpr)
		{ this->pSetResult(iExpr->SelfOrClone()); }

// From Visitor_Expr_Op1_T
	virtual void Visit_Expr_Op1(const ZP<Expr_Op1_T<T>>& iExpr)
		{
		if (ZQ<ZP<T>> theQ0 = this->QDo(iExpr->GetOp0()))
			this->pSetResult(iExpr->SelfOrClone(*theQ0));
		}

// From Visitor_Expr_Op2_T
	virtual void Visit_Expr_Op2(const ZP<Expr_Op2_T<T>>& iExpr)
		{
		if (ZQ<ZP<T>> theQ0 = this->QDo(iExpr->GetOp0()))
			{
			if (ZQ<ZP<T>> theQ1 = this->QDo(iExpr->GetOp1()))
				this->pSetResult(iExpr->SelfOrClone(*theQ0, *theQ1));
			}
		}
	};

} // namespace ZooLib

#endif // __ZooLib_Expr_Visitor_Expr_Op_Do_Transform_T_h__
