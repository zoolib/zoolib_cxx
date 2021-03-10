// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Safe.h"
#include "zoolib/ThreadVal.h"
#include "zoolib/UnicodeString.h"

#include "zoolib/Expr/Expr.h"

#include "zoolib/RelationalAlgebra/RelHead.h"

#include <stdexcept>

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel

class Expr_Rel
:	public virtual Expr
	{
protected:
	Expr_Rel();
	};

// =================================================================================================
#pragma mark - SemanticError

class SemanticError : public std::runtime_error
	{
public:
	SemanticError(const string8& iMessage);
	};

typedef Callable<void(const string8& iMessage)> Callable_SemanticError;

extern const ZP<Callable_SemanticError> sCallable_SemanticError_Ignore;
extern const ZP<Callable_SemanticError> sCallable_SemanticError_Throw;
extern Safe<ZP<Callable_SemanticError>> sCallable_SemanticError_Default;

void sSemanticError(const string8& iMessage);

typedef ThreadVal<ZP<Callable_SemanticError>> ThreadVal_SemanticError;

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_h__
