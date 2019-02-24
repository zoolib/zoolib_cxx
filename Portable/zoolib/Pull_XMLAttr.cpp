/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#include "zoolib/Pull_XMLAttr.h"

#include "zoolib/Log.h"
#include "zoolib/NameUniquifier.h" // For sName
#include "zoolib/ParseException.h"
#include "zoolib/ZMACRO_foreach.h"

/*
sPull_XMLAttr_Push turns this:
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
  "!text" = "\n    ";
  tag2 = "val3";
  tag4 =
    {
    at5 = "5";
    at6 = "6";
    };
  tag7 =
    {
    at8 = "8";
    "!text" = "\n      ";
    tag9 =
      {
      };
    tag10 = "";
    };
  };
};
*/

namespace ZooLib {

using namespace PullPush;
using std::string;

// =================================================================================================
#pragma mark -

static void spPush_Attrs(const ML::Attrs_t& iAttrs, const ChanW_Any& iChanW)
	{
	foreacha (entry, iAttrs)
		sPush(entry.first, entry.second, iChanW);
	}

/*
We enter the function having supposed been passed a tagname. Which will be empty for the top most.

for (;;)
	{
	<Name/> --> Name={}
	<Name a=b c=d/> --> Name={ a="b"; c="d"; }

	<Name>text</Name>, --> Name="text"
	<Name a=b c=d>text</Name>, --> Name={ a="b";c="d"; !text="text"}

	<Name> || <BeginOrEmpty/> --> Name={ --> recurse passing Name, BeginOrEmpty to read
	<Name>text || <BeginOrEmpty/> --> Name={!text=text --> recurse passing Name, BeginOrEmpty to read
	<Name a=b c=d> || <BeginOrEmpty> --> Name={ a="b"; c="d" --> recurse passing Name, BeginOrEmpty to read
	<Name a=b c=d>text || <BeginOrEmpty> --> Name={ a="b"; c="d" !text=test, --> recurse passing Name, BeginOrEmpty to read

	</OuterName> --> } --> return
	Token_Exhausted, with OuterName == "" is legal end of document.
	}
*/

static void spPull_XMLAttr_Push(const string& iOuterName,
	ML::ChanRU_UTF& ioChanRU,
	const ChanW_Any& iChanW)
	{
	for (;;)
		{
		sSkipText(ioChanRU);

		if (ioChanRU.Current() == ML::eToken_Exhausted)
			{
			if (not iOuterName.empty())
				sThrow_ParseException("Unexpected end of source");
			break;
			}
		else if (ioChanRU.Current() == ML::eToken_TagEnd)
			{
			if (iOuterName != ioChanRU.Name())
				{
				sThrow_ParseException("Expected end tag '"
					+ iOuterName + "', got '" + ioChanRU.Name() + "'");
				}
			ioChanRU.Advance();
			break;
			}
		else if (ioChanRU.Current() == ML::eToken_TagEmpty)
			{
			sPush(sName(ioChanRU.Name()), iChanW);
			sPush(kStartMap, iChanW);
				spPush_Attrs(ioChanRU.Attrs(), iChanW);
			sPush(kEnd, iChanW);
			ioChanRU.Advance();
			}
		else
			{
			ZAssert(ioChanRU.Current() == ML::eToken_TagBegin);

			const string theName = ioChanRU.Name();
			sPush(sName(theName), iChanW);
			const ML::Attrs_t theAttrs = ioChanRU.Attrs();
			ioChanRU.Advance();

			const string theText = sReadAllUTF8(ioChanRU);
			sSkipText(ioChanRU); // Won't have text to skip, but will advance to next token.

			if (ioChanRU.Current() == ML::eToken_TagEnd)
				{
				if (ioChanRU.Name() != theName)
					{
					sThrow_ParseException("Expected end tag '"
						+ theName + "', got '" + ioChanRU.Name() + "'");
					}

				ioChanRU.Advance();

				if (theAttrs.empty())
					{
					sPush(theText, iChanW);
					}
				else
					{
					sPush(kStartMap, iChanW);
						spPush_Attrs(ioChanRU.Attrs(), iChanW);
						sPush("!text", theText, iChanW);
					sPush(kEnd, iChanW);
					}
				}
			else if (ioChanRU.Current() == ML::eToken_TagBegin
				|| ioChanRU.Current() == ML::eToken_TagEmpty)
				{
				sPush(kStartMap, iChanW);
					spPush_Attrs(theAttrs, iChanW);
					if (theText.size())
						sPush("!text", theText, iChanW);
					spPull_XMLAttr_Push(theName, ioChanRU, iChanW);
				sPush(kEnd, iChanW);
				}
			else
				{
				ZAssert(ioChanRU.Current() == ML::eToken_Exhausted);
				sThrow_ParseException("Unexpected end of source");
				}
			}
		}
	}

bool sPull_XMLAttr_Push(ML::ChanRU_UTF& ioChanRU, const ChanW_Any& iChanW)
	{
	sSkipText(ioChanRU);
	if (ioChanRU.Current() == ML::eToken_Exhausted)
		return false;

	sPush(kStartMap, iChanW);
	spPull_XMLAttr_Push(string(), ioChanRU, iChanW);
	sPush(kEnd, iChanW);
	return true;
	}

} // namespace ZooLib
