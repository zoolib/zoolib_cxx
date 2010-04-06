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

#include "zoolib/ZVisitor_ExprRep_ToStrim.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_ToStrim::Options

ZVisitor_ExprRep_ToStrim::Options::Options()
:	fEOLString("\n")
,	fIndentString("  ")
,	fInitialIndent(0)
,	fDebuggingOutput(false)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_ToStrim

ZVisitor_ExprRep_ToStrim::ZVisitor_ExprRep_ToStrim(const Options& iOptions, const ZStrimW& iStrimW)
:	fOptions(iOptions)
,	fStrimW(iStrimW)
,	fIndent(0)
	{}

bool ZVisitor_ExprRep_ToStrim::Visit_ExprRep(ZRef<ZExprRep> iRep)
	{
	if (iRep)
		fStrimW << "/* unhandled ZExprRep: " << typeid(*iRep.Get()).name() << " */";
	else
		fStrimW << "/*null ZExprRep*/";
	return true;
	}

void ZVisitor_ExprRep_ToStrim::Write(ZRef<ZExprRep> iExprRep)
	{
	if (iExprRep)
		{
		++fIndent;
		iExprRep->Accept(*this);
		--fIndent;
		}
	}

void ZVisitor_ExprRep_ToStrim::pWriteLFIndent()
	{
	fStrimW.Write(fOptions.fEOLString);
	for (size_t x = 0; x < fIndent; ++x)
		fStrimW.Write(fOptions.fIndentString);
	}

NAMESPACE_ZOOLIB_END
