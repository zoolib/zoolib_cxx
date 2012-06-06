/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZYad_ML.h"
#include "zoolib/ZYad_Any.h"

namespace ZooLib {

using std::string;
using std::vector;

/*
ZYad_ML provides a YadR interface to an XML/XHTML-type source. A tag's name is returned as
the entry's name, and any attributes on a tag are in the entry's Meta.
Because HTML has many tags that are not properly balanced (<P>, <BR>, <link> etc), and
are matched without case-sensitivity this code will need some extension to be really useful
for walking HTML.
*/

// =================================================================================================
// MARK: - Static helpers

static void spThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_ML(iMessage);
	}

// =================================================================================================
// MARK: - ZYadParseException_ML

ZYadParseException_ML::ZYadParseException_ML(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ZYadParseException_ML::ZYadParseException_ML(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
// MARK: - ZYadStrimmerR_ML

ZYadStrimmerR_ML::ZYadStrimmerR_ML(ZRef<ZML::StrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
	{}

void ZYadStrimmerR_ML::Finish()
	{ sSkipText(fStrimmerU->GetStrim()); }

const ZStrimR& ZYadStrimmerR_ML::GetStrimR()
	{ return fStrimmerU->GetStrimR(); }

ZML::StrimU& ZYadStrimmerR_ML::GetStrim()
	{ return fStrimmerU->GetStrim(); }

// =================================================================================================
// MARK: - ZYadMapR_ML

ZYadMapR_ML::ZYadMapR_ML(ZRef<ZML::StrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
	{}

ZYadMapR_ML::ZYadMapR_ML
	(ZRef<ZML::StrimmerU> iStrimmerU, const string& iTagName, const ZML::Attrs_t& iAttrs)
:	fStrimmerU(iStrimmerU),
	fTagName(iTagName),
	fAttrs(iAttrs)
	{}

ZYadMapR_ML::ZYadMapR_ML(ZRef<ZML::StrimmerU> iStrimmerU, const ZML::Attrs_t& iAttrs)
:	ZYadMapR_Std(true),
	fStrimmerU(iStrimmerU),
	fAttrs(iAttrs)
	{}

ZRef<ZYadR> ZYadMapR_ML::Meta()
	{
	if (!fAttrs.empty())
		return sYadR(ZMap_Any(fAttrs.begin(), fAttrs.end()));

	return null;
	}

ZML::Attrs_t ZYadMapR_ML::GetAttrs()
	{ return fAttrs; }

void ZYadMapR_ML::Imp_ReadInc(bool iIsFirst, ZName& oName, ZRef<ZYadR>& oYadR)
	{
	ZML::StrimU& theR = fStrimmerU->GetStrim();

	if (!fTagName.empty())
		{
		if (sTryRead_End(theR, fTagName))
			return;
		}

	oName = theR.Name();

	switch (theR.Current())
		{
		case ZML::eToken_TagBegin:
			{
			const ZML::Attrs_t theAttrs = theR.Attrs();
			theR.Advance();
			oYadR = new ZYadMapR_ML(fStrimmerU, oName, theAttrs);
			break;
			}
		case ZML::eToken_TagEmpty:
			{
			const ZML::Attrs_t theAttrs = theR.Attrs();
			theR.Advance();
			oYadR = new ZYadMapR_ML(fStrimmerU, theAttrs);
			break;
			}
		case ZML::eToken_Text:
			{
			oYadR = new ZYadStrimmerR_ML(fStrimmerU);
			break;
			}
		default:
			break;
		}

	if (!oYadR && !fTagName.empty())
		spThrowParseException("Expected value or end tag '" + fTagName + "'");
	}

} // namespace ZooLib
