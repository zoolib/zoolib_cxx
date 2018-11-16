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
#include "zoolib/Visitor_ToStrim.h"

#include "zoolib/ZDebug.h"

#include <typeinfo>

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_ToStrim::Options

Visitor_ToStrim::Options::Options()
:	fEOLString("\n")
,	fIndentString("  ")
,	fInitialIndent(0)
,	fDebuggingOutput(false)
	{}

// =================================================================================================
#pragma mark - Visitor_ToStrim

Visitor_ToStrim::Visitor_ToStrim()
:	fOptions(nullptr)
,	fStrimW(nullptr)
,	fIndent(0)
	{}

void Visitor_ToStrim::Visit(const ZRef<Visitee>& iRep)
	{
	if (iRep)
		{
		// We put the pointer into a local var to elide clang's warning:
		// `expression with side effects will be evaluated despite
		// being used as an operand to 'typeid'"`
		Visitee* asPointer = iRep.Get();
		pStrimW()
			<< "/*unhandled Visitee: "
			<< (void*)asPointer
			<< "/"
			<< typeid(*asPointer).name()
			<< "*/";
		}
	else
		{
		pStrimW() << "/*null Visitee*/";
		}
	}

void Visitor_ToStrim::ToStrim(
	const Options& iOptions, const ChanW_UTF& iStrimW, const ZRef<Visitee>& iRep)
	{
	SaveSetRestore<const Options*> ssr1(fOptions, &iOptions);
	SaveSetRestore<const ChanW_UTF*> ssr2(fStrimW, &iStrimW);
	SaveSetRestore<size_t> ssr3(fIndent, iOptions.fInitialIndent);

	this->pToStrim(iRep);
	}

void Visitor_ToStrim::pToStrim(const ZRef<Visitee>& iRep)
	{
	ZAssert(fOptions && fStrimW);
	if (iRep)
		{
		++fIndent;
		iRep->Accept(*this);
		--fIndent;
		}
	}

const Visitor_ToStrim::Options& Visitor_ToStrim::pOptions()
	{
	ZAssert(fOptions);
	return *fOptions;
	}

const ChanW_UTF& Visitor_ToStrim::pStrimW()
	{
	ZAssert(fStrimW);
	return *fStrimW;
	}

void Visitor_ToStrim::pWriteLFIndent()
	{
	pStrimW() << pOptions().fEOLString;
	for (size_t xx = 0; xx < fIndent; ++xx)
		pStrimW() << pOptions().fIndentString;
	}

} // namespace ZooLib
