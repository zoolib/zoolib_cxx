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
	ZYadStrimR_ML(ZRef<ZML::Readerer> iReaderer);

// From ZYadR
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	virtual const ZStrimR& GetStrimR();

private:
	ZRef<ZML::Readerer> fReaderer;
	};

ZYadStrimR_ML::ZYadStrimR_ML(ZRef<ZML::Readerer> iReaderer)
:	fReaderer(iReaderer)
	{}

void ZYadStrimR_ML::Finish()
	{ fReaderer->GetReader().Advance(); }

const ZStrimR& ZYadStrimR_ML::GetStrimR()
	{ return fReaderer->GetReader().TextStrim(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_ML

ZYadMapR_ML::ZYadMapR_ML(ZRef<ZML::Readerer> iReaderer)
:	fReaderer(iReaderer)
	{}

ZYadMapR_ML::ZYadMapR_ML(
	ZRef<ZML::Readerer> iReaderer, const string& iTagName, const ZML::Attrs_t& iAttrs)
:	fReaderer(iReaderer),
	fTagName(iTagName),
	fAttrs(iAttrs)
	{}

ZYadMapR_ML::ZYadMapR_ML(ZRef<ZML::Readerer> iReaderer, const ZML::Attrs_t& iAttrs)
:	ZYadMapR_Std(true),
	fReaderer(iReaderer),
	fAttrs(iAttrs)
	{}

ZRef<ZYadR> ZYadMapR_ML::Meta()
	{
	if (!fAttrs.empty())
		return new ZYadMapRPos_Any(fAttrs);

	return ZRef<ZYadR>();
	}

ZML::Attrs_t ZYadMapR_ML::GetAttrs()
	{ return fAttrs; }

void ZYadMapR_ML::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
	{
	ZML::Reader& theR = fReaderer->GetReader();

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
			oYadR = new ZYadMapR_ML(fReaderer, oName, theAttrs);
			break;
			}
		case ZML::eToken_TagEmpty:
			{
			const ZML::Attrs_t theAttrs = theR.Attrs();
			theR.Advance();
			oYadR = new ZYadMapR_ML(fReaderer, theAttrs);
			break;
			}
		case ZML::eToken_Text:
			{
			oYadR = new ZYadStrimR_ML(fReaderer);
			break;
			}
		}

	if (!oYadR && !fTagName.empty())
		spThrowParseException("Expected value or end tag '" + fTagName + "'");
	}

NAMESPACE_ZOOLIB_END
