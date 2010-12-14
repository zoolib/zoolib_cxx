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

#include "zoolib/ZCallable_Function.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZThread.h"

#include "zoolib/zra/ZRA_Expr_Rel.h"

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel

Expr_Rel::Expr_Rel()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * SemanticError, helpers

static ZTSS::Key spKey = ZTSS::sCreate();

static Callable_SemanticError* spGet()
	{ return (Callable_SemanticError*)(long long)(ZTSS::sGet(spKey)); }

static void spSet(ZRef<Callable_SemanticError> iCallable)
	{ ZTSS::sSet(spKey, reinterpret_cast<ZTSS::Value>(iCallable.Get())); }

static void spThrow(const string8& iMessage)
	{ throw SemanticError("ZRA Semantic Error: " + iMessage); }

static void spLog(const string8& iMessage)
	{
	if (ZLOG(s, eErr, "ZRA Semantic Error"))
		s << iMessage;
	}

// =================================================================================================
#pragma mark -
#pragma mark * SemanticError, helpers

const ZRef<Callable_SemanticError> sCallable_Ignore;
const ZRef<Callable_SemanticError> sCallable_Throw = MakeCallable(spThrow);

ZSafe<ZRef<Callable_SemanticError> > sCallable_Default = MakeCallable(spLog);

void sSemanticError(const string8& iMessage)
	{
	if (ZRef<Callable_SemanticError> theCallable = spGet())
		theCallable->Call(iMessage);
	else if (ZRef<Callable_SemanticError> theCallable = sCallable_Default)
		theCallable->Call(iMessage);		
	}

SetRestore_SemanticError::SetRestore_SemanticError(ZRef<Callable_SemanticError> iCallable)
:	fCallable(iCallable)
,	fPrior(spGet())
	{
	spSet(iCallable);
	}

SetRestore_SemanticError::~SetRestore_SemanticError()
	{
	spSet(fPrior);
	}

} // namespace ZRA
} // namespace ZooLib
