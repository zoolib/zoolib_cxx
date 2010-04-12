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

#ifndef __ZVisitor_Expr_Do_T__
#define __ZVisitor_Expr_Do_T__
#include "zconfig.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZExpr.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Do_T

template <class Result_t>
class ZVisitor_Expr_Do_T
:	public virtual ZVisitor_Expr
	{
public:
	ZVisitor_Expr_Do_T()
	:	fHasResult(false)
		{}

// Our protocol
	bool Do(ZRef<ZExpr> iExpr, Result_t& oResult)
		{
		if (iExpr)
			{
			iExpr->Accept(*this);
			if (fHasResult)
				{
				fHasResult = false;
				oResult = fResult;
				return true;
				}
			}
		return false;
		}

	Result_t Do(ZRef<ZExpr> iExpr)
		{
		Result_t result;
		this->Do(iExpr, result);
		return result;
		}

protected:
	void pSetResult(const Result_t& iResult)
		{
		ZAssert(!fHasResult);
		fHasResult = true;
		fResult = iResult;
		}

private:
	bool fHasResult;
	Result_t fResult;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZVisitor_Expr_Do_T__
