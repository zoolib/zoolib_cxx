// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel.h"

#include "zoolib/Callable_Function.h"
#include "zoolib/Log.h"
#include "zoolib/ZThread.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel

Expr_Rel::Expr_Rel()
	{}

// =================================================================================================
#pragma mark - SemanticError

SemanticError::SemanticError(const string8& iMessage)
:	runtime_error(iMessage)
	{}

// =================================================================================================
#pragma mark - SemanticError, helpers

static void spThrow(const string8& iMessage)
	{ throw SemanticError("RelationalAlgebra Semantic Error: " + iMessage); }

static void spLog(const string8& iMessage)
	{
	if (ZLOG(s, eErr, "RelationalAlgebra Semantic Error"))
		s << iMessage;
	}

// =================================================================================================
#pragma mark - SemanticError, global and per-thread handlers

const ZP<Callable_SemanticError> sCallable_SemanticError_Ignore;
const ZP<Callable_SemanticError> sCallable_SemanticError_Throw = sCallable(&spThrow);

Safe<ZP<Callable_SemanticError>> sCallable_SemanticError_Default = sCallable(&spLog);

void sSemanticError(const string8& iMessage)
	{
	if (ZP<Callable_SemanticError> theCallable = ThreadVal_SemanticError::sGet())
		theCallable->Call(iMessage);
	else if (ZP<Callable_SemanticError> theCallable = sCallable_SemanticError_Default)
		theCallable->Call(iMessage);
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
