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

#include "zoolib/ZExpr.h"
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

public:
	virtual RelHead GetRelHead() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Rel

// A useful typedef.
typedef ZRef<Expr_Rel> Rel;

// =================================================================================================
#pragma mark -
#pragma mark * Semantic Error Handling

class SemanticError : public std::runtime_error
	{
public:
	SemanticError(const string8& iMessage)
	:	runtime_error(iMessage)
		{}
	};

void sSemanticError(const string8& iMessage);

enum ESemanticErrorMode
	{
	eSemanticErrorMode_Throw,
	eSemanticErrorMode_Log,
	eSemanticErrorMode_Ignore
	};

class SemanticErrorModeSetter
	{
public:
	SemanticErrorModeSetter(ESemanticErrorMode iMode);
	~SemanticErrorModeSetter();

private:
	ESemanticErrorMode fPrior;
	};

} // namespace ZRA
} // namespace ZooLib

#endif // __ZRA_Expr_Rel__
