// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Visitor_DoMakeWalker.h"

#include "zoolib/Log.h"

#include "zoolib/QueryEngine/Walker_Calc.h"
#include "zoolib/QueryEngine/Walker_Comment.h"
#include "zoolib/QueryEngine/Walker_Const.h"
#include "zoolib/QueryEngine/Walker_Dee.h"
#include "zoolib/QueryEngine/Walker_Dum.h"
#include "zoolib/QueryEngine/Walker_Embed.h"
#include "zoolib/QueryEngine/Walker_Product.h"
#include "zoolib/QueryEngine/Walker_Project.h"
#include "zoolib/QueryEngine/Walker_Rename.h"
#include "zoolib/QueryEngine/Walker_Restrict.h"
#include "zoolib/QueryEngine/Walker_Union.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h"

namespace ZooLib {
namespace QueryEngine {

namespace RA = RelationalAlgebra;

// =================================================================================================
#pragma mark - Visitor_DoMakeWalker

void Visitor_DoMakeWalker::Visit(const ZP<Visitee>& iRep)
	{
	if (ZLOGPF(w, eErr))
		{
		Visitee* asPointer = iRep.Get();
		w << "Unimplemented for visitee: " << typeid(*asPointer).name();
		}

	ZUnimplemented();
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Calc(const ZP<RA::Expr_Rel_Calc>& iExpr)
	{
	if (ZP<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Calc(op0, iExpr->GetColName(), iExpr->GetCallable()));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Comment(const ZP<RA::Expr_Rel_Comment>& iExpr)
	{
	if (ZP<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Comment(op0, iExpr->GetComment(), iExpr->GetCallable()));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Const(const ZP<RA::Expr_Rel_Const>& iExpr)
	{ this->pSetResult(new Walker_Const(iExpr->GetColName(), iExpr->GetVal())); }

void Visitor_DoMakeWalker::Visit_Expr_Rel_Dee(const ZP<RA::Expr_Rel_Dee>& iExpr)
	{ this->pSetResult(new Walker_Dee); }

void Visitor_DoMakeWalker::Visit_Expr_Rel_Dum(const ZP<RA::Expr_Rel_Dum>& iExpr)
	{ this->pSetResult(new Walker_Dum); }

void Visitor_DoMakeWalker::Visit_Expr_Rel_Embed(const ZP<RA::Expr_Rel_Embed>& iExpr)
	{
	if (ZP<Walker> op0 = this->Do(iExpr->GetOp0()))
		{
		if (ZP<Walker> op1 = this->Do(iExpr->GetOp1()))
			this->pSetResult(new Walker_Embed(op0, iExpr->GetBoundNames(), iExpr->GetColName(), op1));
		}
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Product(const ZP<RA::Expr_Rel_Product>& iExpr)
	{
	if (ZP<Walker> op0 = this->Do(iExpr->GetOp0()))
		{
		if (ZP<Walker> op1 = this->Do(iExpr->GetOp1()))
			this->pSetResult(new Walker_Product(op0, op1));
		}
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Project(const ZP<RA::Expr_Rel_Project>& iExpr)
	{
	if (ZP<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Project(op0, iExpr->GetProjectRelHead()));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Rename(const ZP<RA::Expr_Rel_Rename>& iExpr)
	{
	if (ZP<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Rename(op0, iExpr->GetNew(), iExpr->GetOld()));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Restrict(const ZP<RA::Expr_Rel_Restrict>& iExpr)
	{
	if (ZP<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Restrict(op0, iExpr->GetExpr_Bool()));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Union(const ZP<RA::Expr_Rel_Union>& iExpr)
	{
	if (ZP<Walker> op0 = this->Do(iExpr->GetOp0()))
		{
		if (ZP<Walker> op1 = this->Do(iExpr->GetOp1()))
			this->pSetResult(new Walker_Union(op0, op1));
		else
			this->pSetResult(op0);
		}
	else if (ZP<Walker> op1 = this->Do(iExpr->GetOp1()))
		this->pSetResult(op1);
	}

} // namespace QueryEngine
} // namespace ZooLib
