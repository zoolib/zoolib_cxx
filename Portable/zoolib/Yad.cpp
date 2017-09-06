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

#include "zoolib/Chan_UTF_string.h"
#include "zoolib/Yad.h"

#include "zoolib/Log.h"

#define ZMACRO_UseSafeStack 0

namespace ZooLib {

using std::min;
using std::string;

/**
\defgroup Yad Yad (Yet Another Data)
Yad is a neologism by <a href="http://www.linkedin.com/in/ericbwcooper">Eric Cooper</a>.
It's an acronym for <em>Yet Another Data</em>, but interestingly is also a Hebrew word.
From <a href="http://en.wikipedia.org/wiki/Yad">Wikipedia</a>:

	\par
	<em>A yad (Hebrew: יד‎), literally, 'hand,' is a Jewish ritual pointer, used to point to the text
	during the Torah reading from the parchment Torah scrolls. It is intended to prevent anyone
	from touching the parchment, which is considered sacred; additionally, the fragile parchment
	can be damaged by the oils of the skin. While not required when chanting from the Torah, it
	is used frequently.</em>

An instance of YadR points within some data source, can be moved through that source, and can
return further instances of YadR referencing substructures of that source. Basically it's a
generic facility for accessing data that looks like ZooLib ZVal suite -- CFDictionary,
NSDictionary, PList, XMLRPC, JSON, Javascript types etc.

The idea is that there are a Map-like and Seq-like entities in many APIs, and that abstracting
access to them allows code to be applied to any of them.
*/

// =================================================================================================
#pragma mark -
#pragma mark YadOptions

YadOptions::YadOptions(bool iDoIndentation)
:	fRawChunkSize(64),
	fRawByteSeparator(" "),
	fRawAsASCII(iDoIndentation),
	fBreakStrings(true),
	fStringLineLength(80),
	fIDsHaveDecimalVersionComment(iDoIndentation),
	fTimesHaveUserLegibleComment(true)
	{
	if (iDoIndentation)
		{
		fEOLString = "\n";
		fIndentString = "  ";
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark YadParseException

YadParseException::YadParseException(const string& iWhat)
:	runtime_error(iWhat)
	{}

YadParseException::YadParseException(const char* iWhat)
:	runtime_error(iWhat)
	{}

} // namespace ZooLib
