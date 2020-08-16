// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ValPred/Util_Expr_Bool_ValPred_Rename.h"

#include "zoolib/Expr/Visitor_Expr_Op_Do_Transform_T.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h"
#include "zoolib/ValPred/ValPred_Renamed.h"

using std::map;
using std::string;

namespace ZooLib {
namespace Util_Expr_Bool {

// =================================================================================================
#pragma mark - DoRename (anonymous)

namespace { // anonymous

class DoRename
:	public virtual Visitor_Expr_Op_Do_Transform_T<Expr_Bool>
,	public virtual Visitor_Expr_Bool_ValPred
	{
public:
	DoRename(const map<string,string>& iRename)
	:	fRename(iRename)
		{}

	virtual void Visit_Expr_Bool_ValPred(const ZP<Expr_Bool_ValPred>& iExpr)
		{ this->pSetResult(new Expr_Bool_ValPred(sRenamed(fRename, iExpr->GetValPred()))); }

private:
	const map<string,string>& fRename;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - Util_Expr_Bool

ZP<Expr_Bool> sRenamed(const std::map<std::string,std::string>& iRename,
	const ZP<Expr_Bool>& iExpr)
	{ return DoRename(iRename).Do(iExpr); }

} // namespace Util_Expr_Bool
} // namespace ZooLib
