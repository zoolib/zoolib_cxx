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
#pragma mark * Semantic Error Handling

static ZTSS::Key sKey = ZTSS::sCreate();

static ESemanticErrorMode spGet()
	{ return (ESemanticErrorMode)(long long)(ZTSS::sGet(sKey)); }

static void spSet(ESemanticErrorMode iMode)
	{ ZTSS::sSet(sKey, reinterpret_cast<ZTSS::Value>(iMode)); }

void sSemanticError(const string8& iMessage)
	{
	switch (spGet())
		{
		case eSemanticErrorMode_Throw:
			{
			throw SemanticError("ZRA Semantic Error: " + iMessage);
			}
		case eSemanticErrorMode_Log:
			{
			if (ZLOGF(s, eErr))
				s << "ZRA Semantic Error: " + iMessage;
			break;
			}
		case eSemanticErrorMode_Ignore:
			break;
		}
	}

SemanticErrorModeSetter::SemanticErrorModeSetter(ESemanticErrorMode iMode)
:	fPrior(spGet())
	{ spSet(iMode); }

SemanticErrorModeSetter::~SemanticErrorModeSetter()
	{ spSet(fPrior); }

} // namespace ZRA
} // namespace ZooLib
