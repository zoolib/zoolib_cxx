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

#include "zoolib/ZVisitor_ExprRep_DoToStrim.h"

#include <typeinfo>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_DoToStrim::Options

ZVisitor_ExprRep_DoToStrim::Options::Options()
:	fEOLString("\n")
,	fIndentString("  ")
,	fInitialIndent(0)
,	fDebuggingOutput(false)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_DoToStrim
static ZStrimW_Null sNull;

ZVisitor_ExprRep_DoToStrim::ZVisitor_ExprRep_DoToStrim()
:	fOptions(nullptr)
,	fStrimW(nullptr)
,	fIndent(0)
	{}

void ZVisitor_ExprRep_DoToStrim::Visit_ExprRep(ZRef<ZExprRep> iRep)
	{
	if (iRep)
		pStrimW() << "/* unhandled ZExprRep: " << typeid(*iRep.Get()).name() << " */";
	else
		pStrimW() << "/*null ZExprRep*/";
	}

void ZVisitor_ExprRep_DoToStrim::StartToStrim(
	const Options& iOptions, const ZStrimW& iStrimW, ZRef<ZExprRep> iExprRep)
	{
	ZAssert(!fOptions && !fStrimW);
	fOptions = &iOptions;
	fStrimW = &iStrimW;
	try
		{
		this->DoToStrim(iExprRep);
		}
	catch (...)
		{
		fOptions = nullptr;
		fStrimW = nullptr;
		fIndent = 0;
		throw;
		}
	}

void ZVisitor_ExprRep_DoToStrim::DoToStrim(ZRef<ZExprRep> iExprRep)
	{
	ZAssert(fOptions && fStrimW);
	if (iExprRep)
		{
		++fIndent;
		iExprRep->Accept(*this);
		--fIndent;
		}
	}

const  ZVisitor_ExprRep_DoToStrim::Options& ZVisitor_ExprRep_DoToStrim::pOptions()
	{
	ZAssert(fOptions);
	return *fOptions;
	}

const ZStrimW& ZVisitor_ExprRep_DoToStrim::pStrimW()
	{
	ZAssert(fStrimW);
	return *fStrimW;
	}

void ZVisitor_ExprRep_DoToStrim::pWriteLFIndent()
	{
	pStrimW().Write(pOptions().fEOLString);
	for (size_t x = 0; x < fIndent; ++x)
		pStrimW().Write(pOptions().fIndentString);
	}

NAMESPACE_ZOOLIB_END
