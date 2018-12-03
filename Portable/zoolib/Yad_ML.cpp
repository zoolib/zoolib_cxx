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

#include "zoolib/Yad_ML.h"
#include "zoolib/Yad_Any.h"

namespace ZooLib {
namespace Yad_ML {

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
#pragma mark - ChanR_NameRefYad

ChanR_NameRefYad::ChanR_NameRefYad(ZRef<ML::ChannerRU_UTF> iChannerRU_UTF)
:	fChannerRU_UTF(iChannerRU_UTF)
,	fLastWasText(false)
	{}

ChanR_NameRefYad::ChanR_NameRefYad(
	ZRef<ML::ChannerRU_UTF> iChannerRU_UTF, const string& iTagName, const ML::Attrs_t& iAttrs)
:	fChannerRU_UTF(iChannerRU_UTF)
,	fTagName(iTagName)
,	fAttrs(iAttrs)
,	fLastWasText(false)
	{}

ChanR_NameRefYad::ChanR_NameRefYad(ZRef<ML::ChannerRU_UTF> iChannerRU_UTF, const ML::Attrs_t& iAttrs)
:	fChannerRU_UTF(iChannerRU_UTF)
,	fAttrs(iAttrs)
,	fLastWasText(false)
	{}

ZRef<YadR> ChanR_NameRefYad::Meta()
	{
	if (!fAttrs.empty())
		{
		Map_Any theMap;
		for (ML::Attrs_t::const_iterator ii = fAttrs.begin(); ii != fAttrs.end(); ++ii)
			theMap.Set(ii->first, ii->second);
		return sYadR(theMap);
		}

	return null;
	}

ML::Attrs_t ChanR_NameRefYad::GetAttrs()
	{ return fAttrs; }

void ChanR_NameRefYad::Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR)
	{
	ML::ChanRU_UTF& theChan = *fChannerRU_UTF;

	if (sGetSet(fLastWasText, false))
		theChan.Advance();

	if (!fTagName.empty())
		{
		if (sTryRead_End(theChan, fTagName))
			return;
		}

	oName = theChan.Name();

	switch (theChan.Current())
		{
		case ML::eToken_TagBegin:
			{
			const ML::Attrs_t theAttrs = theChan.Attrs();
			theChan.Advance();
			oYadR = sChanner_T<ChanR_NameRefYad>(fChannerRU_UTF, oName, theAttrs);
			break;
			}
		case ML::eToken_TagEmpty:
			{
			const ML::Attrs_t theAttrs = theChan.Attrs();
			theChan.Advance();
			oYadR = sChanner_T<ChanR_NameRefYad>(fChannerRU_UTF, theAttrs);
			break;
			}
		case ML::eToken_Text:
			{
			fLastWasText = true;
			oYadR = fChannerRU_UTF;
			break;
			}
		default:
			break;
		}

	if (not oYadR && not fTagName.empty())
		sThrow_ParseException("Expected value or end tag '" + fTagName + "'");
	}

// =================================================================================================
#pragma mark - 

ZRef<YadR> sYadR(ZRef<ML::ChannerRU_UTF> iChannerRU_UTF)
	{
	if (iChannerRU_UTF)
		return sChanner_T<ChanR_NameRefYad>(iChannerRU_UTF);
	return null;
	}

} // namespace Yad_ML
} // namespace ZooLib
