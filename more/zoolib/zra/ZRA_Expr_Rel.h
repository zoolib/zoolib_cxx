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

#ifndef __ZRA_Expr_Rel__
#define __ZRA_Expr_Rel__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZExpr.h"
#include "zoolib/ZSafe.h"
#include "zoolib/ZUnicodeString.h"

#include "zoolib/zra/ZRA_RelHead.h"

#include <stdexcept>

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel

class Expr_Rel
:	public virtual ZExpr
	{
protected:
	Expr_Rel();
	};

// =================================================================================================
#pragma mark -
#pragma mark * SemanticError

class SemanticError : public std::runtime_error
	{
public:
	SemanticError(const string8& iMessage)
	:	runtime_error(iMessage)
		{}
	};

typedef ZCallable<void(const string8& iMessage)> Callable_SemanticError;

extern const ZRef<Callable_SemanticError> sCallable_Ignore;
extern const ZRef<Callable_SemanticError> sCallable_Throw;
extern ZSafe<ZRef<Callable_SemanticError> > sCallable_Default;

void sSemanticError(const string8& iMessage);

class SetRestore_SemanticError
	{
public:
	SetRestore_SemanticError(ZRef<Callable_SemanticError> iCallable);
	~SetRestore_SemanticError();

private:
	ZRef<Callable_SemanticError> fCallable;
	ZRef<Callable_SemanticError> fPrior;
	};

} // namespace ZRA
} // namespace ZooLib

#endif // __ZRA_Expr_Rel__
