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

NAMESPACE_ZOOLIB_BEGIN

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

static void sThrowParseException(const string& iMessage)
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
#pragma mark * ZYadListMapR_ML

ZYadListMapR_ML::ZYadListMapR_ML(ZML::Reader& iR)
:	fR(iR)
	{}

ZYadListMapR_ML::ZYadListMapR_ML(ZML::Reader& iR, const string& iTagName, const ZTuple& iAttrs)
:	fR(iR),
	fTagName(iTagName),
	fAttrs(iAttrs)
	{}

ZYadListMapR_ML::ZYadListMapR_ML(ZML::Reader& iR, const ZTuple& iAttrs)
:	ZYadListMapR_Std(true),
	fR(iR),
	fAttrs(iAttrs)
	{}

ZTuple ZYadListMapR_ML::GetAttrs()
	{ return fAttrs; }

void ZYadListMapR_ML::Imp_Advance(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR)
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
			const ZTuple theAttrs = fR.Attrs();
			fR.Advance();
			oYadR = new ZYadListMapR_ML(fR, oName, theAttrs);
			break;
			}
		case ZML::eToken_TagEmpty:
			{
			const ZTuple theAttrs = fR.Attrs();
			fR.Advance();
			oYadR = new ZYadListMapR_ML(fR, theAttrs);
			break;
			}
		case ZML::eToken_Text:
			{
			string theString;
			ZStrimW_String(theString).CopyAllFrom(fR.Text());
			fR.Advance();
			oYadR = new ZYadPrimR_Std(theString);
			break;
			}
		}

	if (!oYadR && !fTagName.empty())
		sThrowParseException("Expected value or end tag '" + fTagName + "'");
	}

NAMESPACE_ZOOLIB_END
