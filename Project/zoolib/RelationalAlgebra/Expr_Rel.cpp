/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/Callable_Function.h"
#include "zoolib/Compare_Ref.h"
#include "zoolib/Log.h"
#include "zoolib/ZThread.h"

#include "zoolib/RelationalAlgebra/Expr_Rel.h"

// =================================================================================================
#pragma mark -
#pragma mark sCompare_T

namespace ZooLib {

template <>
int sCompare_T<ZRef<RelationalAlgebra::Expr_Rel> >(
	const ZRef<RelationalAlgebra::Expr_Rel>& iL,
	const ZRef<RelationalAlgebra::Expr_Rel>& iR)
	{ return sCompare_Ref_T(iL, iR); }

ZMACRO_CompareRegistration_T(ZRef<RelationalAlgebra::Expr_Rel>)

} // namespace ZooLib

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark -
#pragma mark Expr_Rel

Expr_Rel::Expr_Rel()
	{}

// =================================================================================================
#pragma mark -
#pragma mark SemanticError

SemanticError::SemanticError(const string8& iMessage)
:	runtime_error(iMessage)
	{}

// =================================================================================================
#pragma mark -
#pragma mark SemanticError, helpers

static void spThrow(const string8& iMessage)
	{ throw SemanticError("RelationalAlgebra Semantic Error: " + iMessage); }

static void spLog(const string8& iMessage)
	{
	if (ZLOG(s, eErr, "RelationalAlgebra Semantic Error"))
		s << iMessage;
	}

// =================================================================================================
#pragma mark -
#pragma mark SemanticError, global and per-thread handlers

const ZRef<Callable_SemanticError> sCallable_SemanticError_Ignore;
const ZRef<Callable_SemanticError> sCallable_SemanticError_Throw = sCallable(&spThrow);

Safe<ZRef<Callable_SemanticError> > sCallable_SemanticError_Default = sCallable(&spLog);

void sSemanticError(const string8& iMessage)
	{
	if (ZRef<Callable_SemanticError> theCallable = ThreadVal_SemanticError::sGet())
		theCallable->Call(iMessage);
	else if (ZRef<Callable_SemanticError> theCallable = sCallable_SemanticError_Default)
		theCallable->Call(iMessage);
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
