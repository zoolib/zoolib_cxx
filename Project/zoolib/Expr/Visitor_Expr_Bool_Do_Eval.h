// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Expr_Visitor_Expr_Bool_Do_Eval_h__
#define __ZooLib_Expr_Visitor_Expr_Bool_Do_Eval_h__ 1
#include "zconfig.h"

#include "zoolib/Visitor_Do_T.h"

#include "zoolib/Expr/Expr_Bool.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_Do_Eval

class Visitor_Expr_Bool_Do_Eval
:	public virtual Visitor_Do_T<bool>
,	public virtual Visitor_Expr_Bool_True
,	public virtual Visitor_Expr_Bool_False
,	public virtual Visitor_Expr_Bool_Not
,	public virtual Visitor_Expr_Bool_And
,	public virtual Visitor_Expr_Bool_Or
	{
public:
// From Visitor_Expr_Bool_XXX
	virtual void Visit_Expr_Bool_True(const ZP<Expr_Bool_True>& iRep);
	virtual void Visit_Expr_Bool_False(const ZP<Expr_Bool_False>& iRep);
	virtual void Visit_Expr_Bool_Not(const ZP<Expr_Bool_Not>& iRep);
	virtual void Visit_Expr_Bool_And(const ZP<Expr_Bool_And>& iRep);
	virtual void Visit_Expr_Bool_Or(const ZP<Expr_Bool_Or>& iRep);
	};

} // namespace ZooLib

#endif // __ZooLib_Expr_Visitor_Expr_Bool_Do_Eval_h__
