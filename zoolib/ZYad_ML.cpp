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

NAMESPACE_ZOOLIB_USING

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
:	ZYadParseException(iWhat)
	{}

ZYadParseException_ML::ZYadParseException_ML(const char* iWhat)
:	ZYadParseException(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_ML

ZYadPrimR_ML::ZYadPrimR_ML(const string& iString)
:	ZYadR_TValue(iString)
	{}

void ZYadPrimR_ML::Finish()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapR_ML

ZYadListMapR_ML::ZYadListMapR_ML(ZML::Reader& iR)
:	fR(iR),
	fPosition(0)
	{}

ZYadListMapR_ML::ZYadListMapR_ML(
	ZML::Reader& iR, const string& iTagName, const ZTuple& iAttrs)
:	fR(iR),
	fTagName(iTagName),
	fAttrs(iAttrs),
	fPosition(0)
	{}

void ZYadListMapR_ML::Finish()
	{
	if (!fTagName.empty())
		{
		if (!sSkip(fR, fTagName))
			sThrowParseException("Expected end tag '" + fTagName + "'");
		fTagName.clear();
		}
	}

bool ZYadListMapR_ML::HasChild()
	{
	this->pMoveIfNecessary();
	return fValue_Current;
	}

ZRef<ZYadR> ZYadListMapR_ML::NextChild()
	{
	this->pMoveIfNecessary();

	if (fValue_Current)
		{
		fValue_Prior = fValue_Current;
		fValue_Current.Clear();
		++fPosition;
		}

	return fValue_Prior;
	}

size_t ZYadListMapR_ML::GetPosition()
	{ return fPosition; }

string ZYadListMapR_ML::Name()
	{ return fChildName; }

ZTuple ZYadListMapR_ML::GetAttrs()
	{ return fAttrs; }

void ZYadListMapR_ML::pMoveIfNecessary()
	{
	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}

	fChildName = fR.Name();
	switch (fR.Current())
		{
		case ZML::eToken_TagBegin:
			{
			ZTuple theAttrs = fR.Attrs();
			fR.Advance();
			fValue_Current = new ZYadListMapR_ML(fR, fChildName, theAttrs);
			break;
			}
		case ZML::eToken_TagEmpty:
			{
			ZTuple theAttrs = fR.Attrs();
			fR.Advance();
			fValue_Current = new ZYadListMapR_ML(fR, "", theAttrs);
			break;
			}
		case ZML::eToken_Text:
			{
			string theString;
			ZStrimW_String(theString).CopyAllFrom(fR.Text());
			fR.Advance();
			fValue_Current = new ZYadPrimR_ML(theString);
			break;
			}
		}
	}
