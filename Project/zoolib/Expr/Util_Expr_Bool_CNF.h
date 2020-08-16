// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Expr_Util_Expr_Bool_CNF_h__
#define __ZooLib_Expr_Util_Expr_Bool_CNF_h__ 1
#include "zconfig.h"

#include "zoolib/TagVal.h"

#include "zoolib/Expr/Expr_Bool.h"

#include <set>

namespace ZooLib {
namespace Util_Expr_Bool {

// =================================================================================================
#pragma mark - Util_Expr_Bool

// CNF == Conjunctive Normal Form (an AND of ORs of terms, ie an AND of DClauses).
// DClause is a disjunction of literals (an OR of terms).
// Term is a literal or the negation of a literal.

typedef TagVal<ZP<Expr_Bool>,struct Tag_Clause> Term;
typedef std::set<Term> DClause;
typedef std::set<DClause> CNF;

ZP<Expr_Bool> sFromCNF(const CNF& iCNF);
CNF sAsCNF(const ZP<Expr_Bool>& iExpr);

} // namespace Util_Expr_Bool
} // namespace ZooLib

#endif // __ZooLib_Expr_Util_Expr_Bool_CNF_h__
