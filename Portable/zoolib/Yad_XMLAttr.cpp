/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_Std.h"
#include "zoolib/Yad_XMLAttr.h"

/*
ZYad_XMLAttr turns this:
  <tag1>
    <tag2>val3</tag2>
    <tag4 at5="5" at6="6"/>
    <tag7 at8="8">
      <tag9/>
      <tag10></tag10>
    </tag7>
  </tag1>

into this:
  {
  tag1 =
    {
    tag2 = "val3";
    tag4 =
      {
      at5 = "5";
      at6 = "6";
      };
    tag7 =
      {
      at8 = "8";
      tag9 = {};
      tag10 = "";
      };
    };
  }
*/

namespace ZooLib {
namespace Yad_XMLAttr {

using std::pair;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark Static parsing functions

static void spThrowParseException(const string& iMessage)
	{
	throw ParseException(iMessage);
	}

// =================================================================================================
#pragma mark -
#pragma mark ParseException

ParseException::ParseException(const string& iWhat)
:	YadParseException(iWhat)
	{}

ParseException::ParseException(const char* iWhat)
:	YadParseException(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark YadMapR

class YadMapR
:	public Channer_T<ChanR_NameRefYad_Std>
	{
public:
	YadMapR(const ML::Attrs_t& iAttrs);
	YadMapR(ZRef<ML::ChannerRU_UTF> iChannerRU_UTF);
	YadMapR(ZRef<ML::ChannerRU_UTF> iChannerRU_UTF,
		const string& iOuterName, const ML::Attrs_t& iAttrs);

// From ChanR_NameRefYad_Std
	virtual void Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR);

private:
	ZRef<ML::ChannerRU_UTF> fChannerRU_UTF;
	const string fOuterName;
	const ML::Attrs_t fAttrs;
	ML::Attrs_t::const_iterator fIter;
	};

YadMapR::YadMapR(const ML::Attrs_t& iAttrs)
:	fAttrs(iAttrs)
,	fIter(fAttrs.begin())
	{}

YadMapR::YadMapR(ZRef<ML::ChannerRU_UTF> iChannerRU_UTF)
:	fChannerRU_UTF(iChannerRU_UTF)
,	fIter(fAttrs.begin())
	{}

YadMapR::YadMapR(
	ZRef<ML::ChannerRU_UTF> iChannerRU_UTF, const string& iOuterName, const ML::Attrs_t& iAttrs)
:	fChannerRU_UTF(iChannerRU_UTF)
,	fOuterName(iOuterName)
,	fAttrs(iAttrs)
,	fIter(fAttrs.begin())
	{}

void YadMapR::Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR)
	{
	if (fIter != fAttrs.end())
		{
		oName = fIter->first;
		oYadR = ZooLib::sYadR(fIter->second);
		++fIter;
		return;
		}

	if (not fChannerRU_UTF)
		return;

	ML::ChanRU_UTF& theR = *fChannerRU_UTF;

	// We have read the begin, and have returned
	// any attributes on that begin.
	sSkipText(theR);

	if (fOuterName.empty())
		{
		if (theR.Current() == ML::eToken_Exhausted)
			return;
		}
	else if (sTryRead_End(theR, fOuterName))
		{
		return;
		}

	if (theR.Current() == ML::eToken_TagEmpty)
		{
		oName = theR.Name();
		oYadR = new YadMapR(theR.Attrs());
		theR.Advance();
		return;
		}

	if (theR.Current() != ML::eToken_TagBegin)
		spThrowParseException("Expected begin tag");

	ML::Attrs_t theAttrs = theR.Attrs();
	const string theName = theR.Name();
	theR.Advance();

	oName = theName;
	const string8 theText = sReadAllUTF8(theR);
	sSkipText(theR);

	if (theR.Current() == ML::eToken_TagEnd)
		{
		if (theR.Name() != theName)
			spThrowParseException("Expected end tag '" + theName + "'");

		if (theAttrs.empty())
			{
			theR.Advance();
			oYadR = ZooLib::sYadR(theText);
			return;
			}
		else if (!theText.empty())
			{
			theAttrs.push_back(pair<string, string>("!text", theText));
			}
		}

	oYadR = new YadMapR(fChannerRU_UTF, theName, theAttrs);
	}

// =================================================================================================
#pragma mark -
#pragma mark sYadR

ZRef<YadR> sYadR(ZRef<ML::ChannerRU_UTF> iChannerRU_UTF)
	{
	sSkipText(*iChannerRU_UTF);
	if (iChannerRU_UTF->Current() != ML::eToken_Exhausted)
		return new YadMapR(iChannerRU_UTF);
	return null;
	}

ZRef<YadR> sYadR(ZRef<ML::ChannerRU_UTF> iChannerRU_UTF,
	const string& iOuterName, const ML::Attrs_t& iAttrs)
	{ return new YadMapR(iChannerRU_UTF, iOuterName, iAttrs); }

} // namespace Yad_XMLAttr
} // namespace ZooLib
