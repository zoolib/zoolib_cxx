// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Visitor_ToStrim.h"

#include "zoolib/Util_Chan_UTF_Operators.h"

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

void Visitor_ToStrim::Visit(const ZP<Visitee>& iRep)
	{
	if (iRep)
		{
		// Putting the pointer into a local var elides clang's warning:
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

void Visitor_ToStrim::ToStrim(const ChanW_UTF& iStrimW,
	const Options& iOptions, const ZP<Visitee>& iRep)
	{
	SaveSetRestore<const Options*> ssr1(fOptions, &iOptions);
	SaveSetRestore<const ChanW_UTF*> ssr2(fStrimW, &iStrimW);
	SaveSetRestore<size_t> ssr3(fIndent, iOptions.fInitialIndent);

	this->pToStrim(iRep);
	}

void Visitor_ToStrim::pToStrim(const ZP<Visitee>& iRep)
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
