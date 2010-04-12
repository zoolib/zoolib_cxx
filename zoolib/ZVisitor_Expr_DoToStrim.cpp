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

#include "zoolib/ZDebug.h"
#include "zoolib/ZVisitor_Expr_DoToStrim.h"

#include <typeinfo>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_DoToStrim::Options

ZVisitor_Expr_DoToStrim::Options::Options()
:	fEOLString("\n")
,	fIndentString("  ")
,	fInitialIndent(0)
,	fDebuggingOutput(false)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_DoToStrim
static ZStrimW_Null sNull;

ZVisitor_Expr_DoToStrim::ZVisitor_Expr_DoToStrim()
:	fOptions(nullptr)
,	fStrimW(nullptr)
,	fIndent(0)
	{}

void ZVisitor_Expr_DoToStrim::Visit_Expr(ZRef<ZExpr> iRep)
	{
	if (iRep)
		pStrimW() << "/* unhandled ZExpr: " << typeid(*iRep.Get()).name() << " */";
	else
		pStrimW() << "/*null ZExpr*/";
	}

void ZVisitor_Expr_DoToStrim::DoToStrim(
	const Options& iOptions, const ZStrimW& iStrimW, ZRef<ZExpr> iExpr)
	{
	const Options* priorOptions = fOptions;
	const ZStrimW* priorStrimW = fStrimW;
	int priorIndent = fIndent;

	fOptions = &iOptions;
	fStrimW = &iStrimW;

	try
		{
		this->DoToStrim(iExpr);
		}
	catch (...)
		{
		fOptions = priorOptions;
		fStrimW = priorStrimW;
		fIndent = priorIndent;
		throw;
		}

	fOptions = priorOptions;
	fStrimW = priorStrimW;
	fIndent = priorIndent;
	}

void ZVisitor_Expr_DoToStrim::DoToStrim(ZRef<ZExpr> iExpr)
	{
	ZAssert(fOptions && fStrimW);
	++fIndent;
	this->Do(iExpr);
	--fIndent;
	}

const  ZVisitor_Expr_DoToStrim::Options& ZVisitor_Expr_DoToStrim::pOptions()
	{
	ZAssert(fOptions);
	return *fOptions;
	}

const ZStrimW& ZVisitor_Expr_DoToStrim::pStrimW()
	{
	ZAssert(fStrimW);
	return *fStrimW;
	}

void ZVisitor_Expr_DoToStrim::pWriteLFIndent()
	{
	pStrimW().Write(pOptions().fEOLString);
	for (size_t x = 0; x < fIndent; ++x)
		pStrimW().Write(pOptions().fIndentString);
	}

NAMESPACE_ZOOLIB_END
