// Copyright (c) 2010-2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_DB_ToStrim.h"

#include "zoolib/ValPred/Util_Strim_ValPred_DB.h"

#include "zoolib/pdesc.h"
#if defined(ZMACRO_pdesc)
	#include "zoolib/StdIO.h"
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_ValPred_ToStrim

void Visitor_Expr_Bool_ValPred_DB_ToStrim::Visit_Expr_Bool_ValPred(
	const ZP<Expr_Bool_ValPred>& iRep)
	{ Util_Strim_ValPred_DB::sToStrim(iRep->GetValPred(), pStrimW()); }

} // namespace ZooLib

// =================================================================================================
#pragma mark - pdesc

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const ZP<Expr_Bool>& iExpr)
	{
	Visitor_Expr_Bool_ValPred_DB_ToStrim().ToStrim(sDefault(), StdIO::sChan_UTF_Err, iExpr);
	}

#endif // defined(ZMACRO_pdesc)
