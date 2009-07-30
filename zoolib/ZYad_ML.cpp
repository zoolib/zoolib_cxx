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

NAMESPACE_ZOOLIB_BEGIN

using std::string;
using std::vector;

/*
ZYad_ML provides a YadR interface to an XML/XHTML-type source. A tag's name is returned as
the entry's name, and any attributes on a tag are accessible from entry's Meta.
Because HTML has many tags that are not properly balanced (<P>, <BR>, <link> etc), and
are matched without case-sensitivity this code will need some extension to be really useful
for walking HTML.
*/

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

static void spThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_ML(iMessage);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_ML

ZYadParseException_ML::ZYadParseException_ML(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ZYadParseException_ML::ZYadParseException_ML(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_ML

class ZYadStrimR_ML
:	public ZYadStrimR
	{
public:
	ZYadStrimR_ML(ZML::Reader& iR);

// From ZYadR
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	virtual const ZStrimR& GetStrimR();

private:
	ZML::Reader& fR;
	};

ZYadStrimR_ML::ZYadStrimR_ML(ZML::Reader& iR)
:	fR(iR)
	{}

void ZYadStrimR_ML::Finish()
	{ fR.Advance(); }

const ZStrimR& ZYadStrimR_ML::GetStrimR()
	{ return fR.TextStrim(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_ML

ZYadMapR_ML::ZYadMapR_ML(ZML::Reader& iR)
:	fR(iR)
	{}

ZYadMapR_ML::ZYadMapR_ML(ZML::Reader& iR, const string& iTagName, const ZML::Attrs_t& iAttrs)
:	fR(iR),
	fTagName(iTagName),
	fAttrs(iAttrs)
	{}

ZYadMapR_ML::ZYadMapR_ML(ZML::Reader& iR, const ZML::Attrs_t& iAttrs)
:	ZYadMapR_Std(true),
	fR(iR),
	fAttrs(iAttrs)
	{}

ZRef<ZYadR> ZYadMapR_ML::Meta()
	{
	if (!fAttrs.empty())
		{
		ZValMap_Any theMap;
		for (ZML::Attrs_t::const_iterator i = fAttrs.begin(); i != fAttrs.end(); ++i)
			theMap.Set((*i).first, (*i).second);
		return new ZYadMapRPos_Any(theMap);
		}
	return ZRef<ZYadR>();
	}

ZML::Attrs_t ZYadMapR_ML::GetAttrs()
	{ return fAttrs; }

void ZYadMapR_ML::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
	{
	if (!fTagName.empty())
		{
		if (sTryRead_End(fR, fTagName))
			return;
		}

	oName = fR.Name();

	switch (fR.Current())
		{
		case ZML::eToken_TagBegin:
			{
			const ZML::Attrs_t theAttrs = fR.Attrs();
			fR.Advance();
			oYadR = new ZYadMapR_ML(fR, oName, theAttrs);
			break;
			}
		case ZML::eToken_TagEmpty:
			{
			const ZML::Attrs_t theAttrs = fR.Attrs();
			fR.Advance();
			oYadR = new ZYadMapR_ML(fR, theAttrs);
			break;
			}
		case ZML::eToken_Text:
			{
			oYadR = new ZYadStrimR_ML(fR);
			break;
			}
		}

	if (!oYadR && !fTagName.empty())
		spThrowParseException("Expected value or end tag '" + fTagName + "'");
	}

NAMESPACE_ZOOLIB_END
