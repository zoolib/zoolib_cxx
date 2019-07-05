/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ChanW_UTF_ML.h"

#include "zoolib/Memory.h"
#include "zoolib/Stringf.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_STL.h"
#include "zoolib/Util_STL_vector.h"
#include "zoolib/Util_string.h"
#include "zoolib/Util_Time.h"

#include <stdio.h> // For sprintf

namespace ZooLib {

#define kDebug_StrimW_ML 1

using std::min;
using std::pair;
using std::string;
using std::vector;

using namespace Util_Chan;
using namespace Util_STL;

// =================================================================================================
#pragma mark - ChanW_UTF_ML

/** \class ChanW_UTF_ML
ChanW_UTF_ML extends the ChanW_UTF protocol with methods to open and close
tags, attach attributes to those tags incrementally and pretty-print the
output. For example, assuming we're passed \c iStrimW:
\code
StrimW s(iStrimW);
s.Begin("html");

	s.Begin("head");

		s.Begin("title");
			s.Write("This is my title");
		s.End("title");

	s.End("head");

	s.Begin("body");

		s.Begin("h1");
			s.Write("This is a header, followed by a horizontal rule");
		s.End("h1");

		s.Tag("hr");
		s.Attr("width", "75%");

		s.Begin("a");
		s.Attr("href", "http://somedomain.com/");
			s.Write("This text links to somedomain");
		s.End("a");

	s.End("body");

s.End("html");
\endcode

will generate the html:

\verbatim
<html>
  <head>
    <title>
      This is my title
    </title>
  </head>
  <body>
    <h1>
      This is a header, followed by a horizontal rule
    </h1>
    <hr width="75%">
    <a href="http://somedomain.com/">
      This text links to somedomain
    </a>
  </body>
</html>
\endverbatim

It is not necessary to pass the tag name to \c End, but if you do then
StrimW can ensure that the tag you think you're closing is indeed
the last tag opened. Note that tag names, attribute names and attribute
values are stored, compared and emitted as-is -- no case conversion is performed,
so closing an 'HTML' begin tag with an 'html' end tag will trip an assertion.

ML::ChanW_UTF_ML::Tag exists to allow for the generation of tags that do not have their
balance checked, in particular common HTML tags such as BR, P, HR, LI, IMG etc.

To generate XML empty tags e.g. <sometag param="something"/> use ML::ChanW_UTF_ML::Empty.

After a call to ML::ChanW_UTF_ML::Begin, ML::ChanW_UTF_ML::Empty or ML::ChanW_UTF_ML::Tag
the strim is in a mode whereby attributes may be added to the tag. A call to
ML::ChanW_UTF_ML::End or any call to the normal write methods will cause
the pending tag plus its attributes to be emitted to the sink strim
followed by the actual write. ML::ChanW_UTF_ML::Attr trips an assertion if the strim
is not in the correct mode.

Note that ChanW_UTF_ML descends from NonCopyable, in order to enforce
that it can't be assigned or copy constructed from another ChanW_UTF_ML. Nevertheless
we declare a copy constructor and then don't define it. Why? In the words of
<a href="https://groups.google.com/d/embed/msg/comp.lang.c++/E7lRUiEYX8A/0ZqzGZV_0FkJ">
Pete C on comp.lang.c++</a>:

<tt>"When initialising a const reference to an rvalue (which we are in this
case), the compiler may either:<br>
a) Bind the reference directly to the object that the rvalue represents, or<br>
b) Construct a temporary const object from the rvalue, and bind the  reference
to the temporary.<br>
The choice is implementation-defined, but the standard says that the
constructor that would be used in (b) must be available anyway.<br>
...<br>
See section 8.5.3.5 of the Standard for gory details."</tt>

We often want to be able to pass a temporary ChanW_UTF_ML initialized with a
regular strim to a function taking a const ChanW_UTF_ML&. In gcc pre-4.0 this
worked just as we want, and gcc post-4.0 still does the same thing (a),
but now enforces the requirement that a copy constructor be visible.

The upshot is that if you get an error at link time, stating that no
implementation of ChanW_UTF_ML(const ChanW_UTF_ML&) can be found, you've
got yourself a compiler that's doing (b) and we'll need to rethink things.
*/

ChanW_UTF_ML::ChanW_UTF_ML(const ChanW_UTF& iStrimSink)
:	fStrimSink(iStrimSink)
,	fTagType(eTagTypeNone)
,	fWrittenSinceLastTag(true)
,	fLastWasBegin(false)
,	fLastWasEOL(false)
,	fIndentEnabled(true)
,	fString_EOL("\n")
,	fString_Indent("  ")
	{}

ChanW_UTF_ML::ChanW_UTF_ML(bool iIndent, const ChanW_UTF& iStrimSink)
:	fStrimSink(iStrimSink)
,	fTagType(eTagTypeNone)
,	fWrittenSinceLastTag(true)
,	fLastWasBegin(false)
,	fLastWasEOL(false)
,	fIndentEnabled(iIndent)
,	fString_EOL("\n")
,	fString_Indent("  ")
	{}

ChanW_UTF_ML::ChanW_UTF_ML(const string8& iEOL, const string8& iIndent, const ChanW_UTF& iStrimSink)
:	fStrimSink(iStrimSink)
,	fTagType(eTagTypeNone)
,	fWrittenSinceLastTag(true)
,	fLastWasBegin(false)
,	fLastWasEOL(false)
,	fIndentEnabled(true)
,	fString_EOL(iEOL)
,	fString_Indent(iIndent)
	{}

ChanW_UTF_ML::~ChanW_UTF_ML()
	{
	try
		{
		this->pWritePending();
		}
	catch (...)
		{}

	// We don't want to do this if we're being disposed as part of an
	// stack unwind initiated by an exception.
	// ZAssertLog(kDebug_StrimW_ML, fTags.empty());
	}

void ChanW_UTF_ML::Flush()
	{
	sNonConst(this)->pWritePending();
	sFlush(fStrimSink);
	}

static void spWriteIndent(const ChanW_UTF& iStrim, const string8& iString, size_t iCount)
	{
	while (iCount--)
		sEWrite(iStrim, iString);
	}

size_t ChanW_UTF_ML::WriteUTF8(const UTF8* iSource, size_t iCountCU)
	{
	this->pPreText();

	const UTF8* localSource = iSource;
	const UTF8* localSourceEnd = iSource + iCountCU;
	for (;;)
		{
		// Walk forward through the source till we find a entity CP.
		ZQ<UTF32> theEntityCPQ;
		const UTF8* current = localSource;
		const UTF8* priorToEntity;
		do	{
			priorToEntity = current;
			UTF32 theCP;
			if (not Unicode::sReadInc(current, localSourceEnd, theCP))
				break;

			switch (theCP)
				{
				case '\t':
				case '\n':
				case '\r':
					{
					// These whitespace characters are fine
					break;
					}
				case '"':
				case '&':
				case '<':
				case '>':
				case 0x00A0:
				case 0x007F:
					{
					theEntityCPQ = theCP;
					break;
					}
				default:
					{
					if (theCP < 0x20)
						theEntityCPQ = theCP;
					}
				}
			} while (not theEntityCPQ);

		if (theEntityCPQ)
			{
			// Write everything prior to the entity.
			if (size_t countToWrite = priorToEntity - localSource)
				{
				const size_t countWritten = sWrite(fStrimSink, localSource, countToWrite);
				if (not countWritten)
					break;

				localSource += countWritten;
				if (countWritten < countToWrite)
					{
					// We weren't able to write the whole chunk. Trust that the writer did a decent
					// job of moving to a usable UTF8 boundary (trivial if the fStrimSink is UTF-8 native),
					// return to the top of the for loop and try again.
					continue;
					}
				}

			// Write the substitution text.
			switch (*theEntityCPQ)
				{
				case '"': sEWrite(fStrimSink, "&quot;"); break;
				case '&': sEWrite(fStrimSink, "&amp;"); break;
				case '<': sEWrite(fStrimSink, "&lt;"); break;
				case '>': sEWrite(fStrimSink, "&gt;"); break;
				case 0x00A0: sEWrite(fStrimSink, "&nbsp;"); break;
				default: sEWritef(fStrimSink, "&#%u;", (unsigned int)*theEntityCPQ);
				}

			localSource = current;
			}
		else
			{
			size_t countToWrite = current - localSource;
			if (not countToWrite)
				break;
			size_t countWritten = sWrite(fStrimSink, localSource, countToWrite);
			if (not countWritten)
				break;
			localSource += countWritten;
			}
		}

	const size_t result = localSource - iSource;

	UTF32 theCP;
	if (Unicode::sDecRead(iSource, localSource, localSourceEnd, theCP))
		fLastWasEOL = Unicode::sIsEOL(theCP);

	return result;
	}

const ChanW_UTF& ChanW_UTF_ML::Raw() const
	{
	sNonConst(this)->pWritePending();
	return fStrimSink;
	}

// An easy way to directly include an NBSP in a string
// is with the code unit sequence "\xC2\xA0".
const ChanW_UTF_ML& ChanW_UTF_ML::WriteNBSP() const
	{
	sNonConst(this)->pPreText();

	sEWrite(fStrimSink, "&nbsp;");

	return *this;
	}

// The validity of iEntity is not checked -- be sure you
// use only legal characters.
const ChanW_UTF_ML& ChanW_UTF_ML::WriteEntity(const string8& iEntity) const
	{
	sNonConst(this)->pPreText();

	sEWrite(fStrimSink, "&");
	sEWrite(fStrimSink, iEntity);
	sEWrite(fStrimSink, ";");

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::WriteEntity(const UTF8* iEntity) const
	{
	sNonConst(this)->pPreText();

	sEWrite(fStrimSink, "&");
	sEWrite(fStrimSink, iEntity);
	sEWrite(fStrimSink, ";");

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::Begin(const string8& iTag) const
	{
	sNonConst(this)->pBegin(iTag, eTagTypeNormal);

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::End(const string8& iTag) const
	{
	sNonConst(this)->pWritePending();

	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());
	ZAssertStopf(kDebug_StrimW_ML, fTags.back() == iTag,
		"Expected \"%s\", got \"%s\"", fTags.back().c_str(), iTag.c_str());

	sNonConst(this)->pEnd();

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::End() const
	{
	sNonConst(this)->pWritePending();

	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());
	sNonConst(this)->pEnd();

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::Empty(const string8& iTag) const
	{
	sNonConst(this)->pBegin(iTag, eTagTypeEmpty);

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::PI(const string8& iTag) const
	{
	sNonConst(this)->pBegin(iTag, eTagTypePI);

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::Tag(const string8& iTag) const
	{
	sNonConst(this)->pBegin(iTag, eTagTypeNoEnd);

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::Attr(const string8& iName) const
	{
	sNonConst(this)->pAttr(iName, nullptr);

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::Attr(const string8& iName, const string8& iValue) const
	{
	string8* newValue = nullptr;
	if (iValue.empty())
		{
		newValue = new string8;
		}
	else
		{
		string8::const_iterator start = iValue.begin();
		string8::const_iterator end = iValue.end();
		string8::const_iterator current = start;
		for (;;)
			{
			UTF32 theCP;
			if (not Unicode::sReadInc(current, end, theCP))
				break;

			if (newValue)
				{
				// We've seen CPs that need entity encoding, so
				// append the appropriate entity or code units to newValue.
				switch (theCP)
					{
					case '"': newValue->append("&quot;"); break;
					case '&': newValue->append("&amp;"); break;
					case '<': newValue->append("&lt;"); break;
					case '>': newValue->append("&gt;"); break;
					case 0x00A0: newValue->append("&nbsp;"); break;
					default:
						{
						if (theCP < 0x20)
							{
							char smallEntity[7];
							sprintf(smallEntity, "&#x%02X;", (unsigned int)theCP);
							newValue->append(smallEntity);
							}
						else
							{
							*newValue += theCP;
							}
						break;
						}
					}
				}
			else
				{
				// We have not yet had to entity-encode anything.
				switch (theCP)
					{
					case '"':
					case '&':
					case '<':
					case '>':
					case 0x00A0:
						break;
					default:
						{
						if (theCP >= 0x20)
							{
							// This CP does not need entity encoding, and we've not
							// yet seen any CP that does, so just go back around the loop.
							continue;
							}
						}
					}
				// This is the first CP that needs entity encoding,
				// so allocate newValue and copy the CPs *prior* to
				// this CP into newValue. The entity CP itself will
				// get re-read on the next iteration in the branch above.
				Unicode::sDec(start, current, end);
				newValue = new string8(start, current);
				newValue->reserve(iValue.size());
				}
			}
		if (not newValue)
			{
			// We saw no CPs that need entity encoding, so never allocated newValue
			// nor did we accumulate values into it. So we can do so now and will
			// share the underlying rep held by iValue.
			newValue = new string8(iValue);
			}
		}

	sNonConst(this)->pAttr(iName, newValue);

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::Attr(const string8& iName, const UTF8* iValue) const
	{ return this->Attr(iName, string8(iValue)); }

const ChanW_UTF_ML& ChanW_UTF_ML::Attr(const string8& iName, int iValue) const
	{
	string8 theValue = sStringf("%d", iValue);
	sNonConst(this)->pAttr(iName, new string8(theValue));

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::Attrf(const string8& iName, const UTF8* iValue, ...) const
	{
	va_list args;
	va_start(args, iValue);
	string8 theValue = sStringf(iValue, args);
	va_end(args);

	sNonConst(this)->pAttr(iName, new string8(theValue));

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::Attrs(const ML::Attrs_t& iMap) const
	{
	for (ML::Attrs_t::const_iterator ii = iMap.begin(); ii != iMap.end(); ++ii)
		this->Attr(ii->first, ii->second);

	return *this;
	}

bool ChanW_UTF_ML::Indent(bool iIndent) const
	{
	sNonConst(this)->pWritePending();

	bool oldIndent = fIndentEnabled;
	sNonConst(this)->fIndentEnabled = iIndent;
	return oldIndent;
	}

void ChanW_UTF_ML::Abandon()
	{
	fTagType = eTagTypeNone;
	fTags.clear();
	fAttributeNames.clear();

	sDeleteAll(fAttributeValues.begin(), fAttributeValues.end());
	fAttributeValues.clear();
	}

const ChanW_UTF_ML& ChanW_UTF_ML::EndAll()
	{
	this->pWritePending();

	while (not fTags.empty())
		this->pEnd();

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::EndAllIfPresent(const string8& iTag)
	{
	this->pWritePending();

	if (sContains(fTags, iTag))
		{
		for (bool gotIt = false; !gotIt; /*no inc*/)
			{
			gotIt = (fTags.back() == iTag);
			this->pEnd();
			}
		}

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::EndAll(const string8& iTag)
	{
	this->pWritePending();

	for (bool gotIt = false; !gotIt && !fTags.empty(); /*no inc*/)
		{
		gotIt = (fTags.back() == iTag);
		this->pEnd();
		}

	return *this;
	}

const ChanW_UTF_ML& ChanW_UTF_ML::WritePending() const
	{
	sNonConst(this)->pWritePending();
	return *this;
	}

void ChanW_UTF_ML::pBegin(const string8& iTag, ETagType iTagType)
	{
	ZAssertStop(1, !iTag.empty());
	this->pWritePending();
	fTags.push_back(iTag);
	fTagType = iTagType;
	fLastWasBegin = true;
	}

void ChanW_UTF_ML::pPreText()
	{
	this->pWritePending();

	fWrittenSinceLastTag = true;
	fLastWasEOL = false;
	}

void ChanW_UTF_ML::pPreTag()
	{
	if (fIndentEnabled)
		{
		if (not fWrittenSinceLastTag)
			{
			if (not fLastWasEOL)
				sEWrite(fStrimSink, fString_EOL);
			spWriteIndent(fStrimSink, fString_Indent, fTags.size() - 1);
			}
		}
	fWrittenSinceLastTag = false;
	fLastWasEOL = false;
	}

void ChanW_UTF_ML::pWritePending()
	{
	if (fTagType == eTagTypeNone)
		return;

	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());

	if (fIndentEnabled)
		{
		if (not fWrittenSinceLastTag)
			{
			if (not fLastWasEOL)
				sEWrite(fStrimSink, fString_EOL);
			spWriteIndent(fStrimSink, fString_Indent, fTags.size() - 1);
			}
		}
	fWrittenSinceLastTag = false;
	fLastWasEOL = false;

	if (fTagType == eTagTypePI)
		sEWrite(fStrimSink, "<?");
	else
		sEWrite(fStrimSink, "<");

	sEWrite(fStrimSink, fTags.back());

	if (not fAttributeNames.empty())
		{
		for (size_t xx = 0; xx < fAttributeNames.size(); ++xx)
			{
			sEWrite(fStrimSink, " ");

			sEWrite(fStrimSink, fAttributeNames[xx]);

			if (fAttributeValues[xx])
				{
				sEWrite(fStrimSink, "=\"");
				sEWrite(fStrimSink, *fAttributeValues[xx]);
				sEWrite(fStrimSink, "\"");
				delete fAttributeValues[xx];
				}
			}
		fAttributeNames.clear();
		fAttributeValues.clear();
		}

	switch (fTagType)
		{
		case eTagTypeEmpty:
			sEWrite(fStrimSink, "/>");
			break;
		case eTagTypePI:
			sEWrite(fStrimSink, "?>");
			break;
		default:
			sEWrite(fStrimSink, ">");
			break;
		}

	if (fTagType != eTagTypeNormal)
		fTags.pop_back();

	fWrittenSinceLastTag = false;

	fTagType = eTagTypeNone;
	}

void ChanW_UTF_ML::pAttr(const string8& iName, string8* iValue)
	{
	// We can only add an attribute if we've called Begin and there's
	// been no intervening call to Imp_WriteUTF8.
	ZAssertStop(kDebug_StrimW_ML, fTagType != eTagTypeNone);
	fAttributeNames.push_back(iName);
	fAttributeValues.push_back(iValue);
	}

void ChanW_UTF_ML::pEnd()
	{
	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());

	if (fIndentEnabled)
		{
		if (not fWrittenSinceLastTag && not fLastWasBegin)
			{
			if (not fLastWasEOL)
				sEWrite(fStrimSink, fString_EOL);
			spWriteIndent(fStrimSink, fString_Indent, fTags.size() - 1);
			}
		}
	fWrittenSinceLastTag = false;
	fLastWasEOL = false;
	fLastWasBegin = false;

	sEWrite(fStrimSink, "</");
	sEWrite(fStrimSink, fTags.back());
	sEWrite(fStrimSink, ">");
	fTags.pop_back();

	fTagType = eTagTypeNone;
	}

// =================================================================================================
#pragma mark - ChanW_UTF_ML::Indenter

ChanW_UTF_ML::Indenter::Indenter(ChanW_UTF_ML& ioChanW, bool iIndent)
:	fChanW(ioChanW)
,	fPriorIndent(ioChanW.Indent(iIndent))
	{}

ChanW_UTF_ML::Indenter::~Indenter()
	{ fChanW.Indent(fPriorIndent); }

} // namespace ZooLib
