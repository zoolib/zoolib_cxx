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

#include "zoolib/Util_Chan_UTF_Operators.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZVisitor_ToStrim.h"

#include <typeinfo>

namespace ZooLib {

// =================================================================================================
// MARK: - ZVisitor_ToStrim::Options

ZVisitor_ToStrim::Options::Options()
:	fEOLString("\n")
,	fIndentString("  ")
,	fInitialIndent(0)
,	fDebuggingOutput(false)
	{}

// =================================================================================================
// MARK: - ZVisitor_ToStrim

ZVisitor_ToStrim::ZVisitor_ToStrim()
:	fOptions(nullptr)
,	fStrimW(nullptr)
,	fIndent(0)
	{}

void ZVisitor_ToStrim::Visit(const ZRef<ZVisitee>& iRep)
	{
	if (iRep)
		{
		pStrimW()
			<< "/*unhandled ZVisitee: "
			<< (void*)iRep.Get()
			<< "/"
			<< typeid(*iRep.Get()).name()
			<< "*/";
		}
	else
		{
		pStrimW() << "/*null ZVisitee*/";
		}
	}

void ZVisitor_ToStrim::ToStrim(
	const Options& iOptions, const ChanW_UTF& iStrimW, const ZRef<ZVisitee>& iRep)
	{
	SaveSetRestore<const Options*> ssr1(fOptions, &iOptions);
	SaveSetRestore<const ChanW_UTF*> ssr2(fStrimW, &iStrimW);
	SaveSetRestore<size_t> ssr3(fIndent, iOptions.fInitialIndent);

	this->pToStrim(iRep);
	}

void ZVisitor_ToStrim::pToStrim(const ZRef<ZVisitee>& iRep)
	{
	ZAssert(fOptions && fStrimW);
	if (iRep)
		{
		++fIndent;
		iRep->Accept(*this);
		--fIndent;
		}
	}

const ZVisitor_ToStrim::Options& ZVisitor_ToStrim::pOptions()
	{
	ZAssert(fOptions);
	return *fOptions;
	}

const ChanW_UTF& ZVisitor_ToStrim::pStrimW()
	{
	ZAssert(fStrimW);
	return *fStrimW;
	}

void ZVisitor_ToStrim::pWriteLFIndent()
	{
	pStrimW() << pOptions().fEOLString;
	for (size_t xx = 0; xx < fIndent; ++xx)
		pStrimW() << pOptions().fIndentString;
	}

} // namespace ZooLib
