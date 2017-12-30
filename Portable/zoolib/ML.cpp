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

#include "zoolib/Memory.h"
#include "zoolib/ML.h"
#include "zoolib/Stringf.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_STL.h"
#include "zoolib/Util_STL_vector.h"
#include "zoolib/Util_string.h"
#include "zoolib/Util_Time.h"

#include <stdio.h> // For sprintf

namespace ZooLib {
namespace ML {

#define kDebug_StrimW_ML 1

using std::min;
using std::pair;
using std::string;
using std::vector;

using namespace Util_Chan;
using namespace Util_STL;

// =================================================================================================
#pragma mark -
#pragma mark Static helper functions

static string spReadReference(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	ZRef<Callable_Entity> iCallable)
	{
	string result;

	if (sTryRead_CP('#', iChanR, iChanU))
		{
		// It's a character reference.
		int64 theInt;
		bool gotIt = false;
		if (sTryRead_CP('x', iChanR, iChanU) || sTryRead_CP('X', iChanR, iChanU))
			gotIt = sTryRead_HexInteger(iChanR, iChanU, theInt);
		else
			gotIt = sTryRead_DecimalInteger(iChanR, iChanU, theInt);

		if (gotIt && sTryRead_CP(';', iChanR, iChanU))
			result += UTF32(theInt);
		}
	else
		{
		string theEntity;
		theEntity.reserve(8);
		for (;;)
			{
			if (NotQ<UTF32> theCPQ = sQRead(iChanR))
				{
				theEntity.clear();
				break;
				}
			else if (*theCPQ == ';')
				{ break; }
			else if (Unicode::sIsWhitespace((*theCPQ)))
				{
				theEntity.clear();
				break;
				}
			else
				{
				theEntity += *theCPQ;
				}
			}

		if (not theEntity.empty())
			{
			if (iCallable)
				result = iCallable->Call(theEntity);
			else if (theEntity == "quot")
				result = "\"";
			else if (theEntity == "lt")
				result = "<";
			else if (theEntity == "gt")
				result = ">";
			else if (theEntity == "amp")
				result = "&";
			else if (theEntity == "apos")
				result = "'";
			else if (theEntity == "nbsp")
				result = "\xC2\xA0";
			}
		}

	return result;
	}

static bool spReadMLIdentifier(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	string& oText)
	{
	oText.resize(0);

	if (NotQ<UTF32> theCPQ = sQRead(iChanR))
		{ return false; }
	else if (not Unicode::sIsAlpha(*theCPQ) && *theCPQ != '_' && *theCPQ != '?' && *theCPQ != '!')
		{
		sUnread(iChanU, *theCPQ);
		return false;
		}
	else
		{
		oText += *theCPQ;

		for (;;)
			{
			if (NotQ<UTF32> theCPQ = sQRead(iChanR))
				{
				break;
				}
			else if (not Unicode::sIsAlphaDigit(*theCPQ)
				&& *theCPQ != '_' && *theCPQ != '-' && *theCPQ != ':')
				{
				sUnread(iChanU, *theCPQ);
				break;
				}
			else
				{
				oText += *theCPQ;
				}
			}
		}

	return true;
	}

static bool spReadUntil(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, UTF32 iTerminator,
	string& oText)
	{
	oText.resize(0);

	for (;;)
		{
		if (NotQ<UTF32> theCPQ = sQRead(iChanR))
			return false;
		else if (*theCPQ == iTerminator)
			return true;
		else
			oText += *theCPQ;
		}
	}

static bool spReadUntil(
	const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	bool iRecognizeEntities, ZRef<Callable_Entity> iCallable,
	UTF32 iTerminator, string& oText)
	{
	oText.resize(0);

	for (;;)
		{
		if (NotQ<UTF32> theCPQ = sQRead(iChanR))
			return false;
		else if (*theCPQ == iTerminator)
			return true;
		else if (*theCPQ == '&' && iRecognizeEntities)
			oText += spReadReference(iChanR, iChanU, iCallable);
		else
			oText += *theCPQ;
		}
	}

static bool spReadMLAttributeName(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, string& oName)
	{
	oName.resize(0);

	if (NotQ<UTF32> theCPQ = sQRead(iChanR))
		{ return false; }
	else if (*theCPQ == '"')
		{ return spReadUntil(iChanR, iChanU, '"', oName); }
	else if (*theCPQ == '\'')
		{ return spReadUntil(iChanR, iChanU, '\'', oName); }
	else
		{
		if (not Unicode::sIsAlpha(*theCPQ) && *theCPQ != '_' && *theCPQ != '?' && *theCPQ != '!')
			{
			sUnread(iChanU, *theCPQ);
			return false;
			}

		oName += *theCPQ;
		for (;;)
			{
			if (NotQ<UTF32> theCPQ = sQRead(iChanR))
				{ return true; }
			else if (not Unicode::sIsAlphaDigit(*theCPQ)
				&& *theCPQ != '_' && *theCPQ != '-' && *theCPQ != ':')
				{
				sUnread(iChanU, *theCPQ);
				return true;
				}
			else
				{ oName += *theCPQ; }
			}
		}
	}

static bool spReadMLAttributeValue(
	const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	bool iRecognizeEntities, ZRef<Callable_Entity> iCallable,
	string& oValue)
	{
	oValue.resize(0);

	if (NotQ<UTF32> theCPQ = sQRead(iChanR))
		{ return false; }
	else if (*theCPQ == '"')
		{ return spReadUntil(iChanR, iChanU, '"', oValue); }
	else if (*theCPQ == '\'')
		{ return spReadUntil(iChanR, iChanU, '\'', oValue); }
	else
		{
		sUnread(iChanU, *theCPQ);

		sSkip_WS(iChanR, iChanU);

		for (;;)
			{
			if (NotQ<UTF32> theCPQ = sQRead(iChanR))
				{ break; }
			else if (*theCPQ == '>')
				{
				sUnread(iChanU, *theCPQ);
				break;
				}
			else if (Unicode::sIsWhitespace(*theCPQ))
				{ break; }
			else if (*theCPQ == '&' && iRecognizeEntities)
				{ oValue += spReadReference(iChanR, iChanU, iCallable); }
			else
				{ oValue += *theCPQ; }
			}
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanRU_UTF

ChanRU_UTF::ChanRU_UTF(const ZooLib::ChanRU<UTF32>& iChanRU)
:	fChanR(iChanRU)
,	fChanU(iChanRU)
,	fRecognizeEntitiesInAttributeValues(false)
,	fBufferStart(0)
,	fToken(eToken_Fresh)
	{}

ChanRU_UTF::ChanRU_UTF(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
:	fChanR(iChanR)
,	fChanU(iChanU)
,	fRecognizeEntitiesInAttributeValues(false)
,	fBufferStart(0)
,	fToken(eToken_Fresh)
	{}

ChanRU_UTF::ChanRU_UTF(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	bool iRecognizeEntitiesInAttributeValues, ZRef<Callable_Entity> iCallable)
:	fChanR(iChanR)
,	fChanU(iChanU)
,	fRecognizeEntitiesInAttributeValues(iRecognizeEntitiesInAttributeValues)
,	fCallable(iCallable)
,	fBufferStart(0)
,	fToken(eToken_Fresh)
	{}

ChanRU_UTF::~ChanRU_UTF()
	{}

size_t ChanRU_UTF::QRead(UTF32* oDest, size_t iCount)
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	UTF32* localDest = oDest;
	if (fToken == eToken_Text)
		{
		while (iCount)
			{
			if (fBuffer.size())
				{
				size_t countToCopy = min(iCount, fBuffer.size() - fBufferStart);
				fBuffer.copy(localDest, countToCopy, fBufferStart);
				localDest += countToCopy;
				iCount -= countToCopy;
				fBufferStart += countToCopy;
				if (fBufferStart >= fBuffer.size())
					{
					// We've read everything in the buffer, so we can toss it.
					fBufferStart = 0;
					fBuffer.resize(0);
					}
				}
			else if (NotQ<UTF32> theCPQ = sQRead(fChanR))
				{
				fToken = eToken_Exhausted;
				break;
				}
			else if (*theCPQ == '<')
				{
				sUnread(fChanU, *theCPQ);
				break;
				}
			else if (*theCPQ == '&')
				{
				fBufferStart = 0;
				fBuffer = Unicode::sAsUTF32(spReadReference(fChanR, fChanU, fCallable));
				}
			else
				{
				*localDest++ = *theCPQ;
				--iCount;
				}
			}
		}

	return localDest - oDest;
	}

size_t ChanRU_UTF::Unread(const UTF32* iSource, size_t iCount)
	{
	ZAssert(fToken == eToken_Text);

	if (fBufferStart > iCount)
		{
		fBufferStart -= iCount;
		}
	else
		{
		// iCount >= fBufferStart
		const string32 toInsert(iSource + fBufferStart, iSource + iCount);
		fBuffer.insert(fBuffer.begin(), toInsert.rbegin(), toInsert.rend());
		fBufferStart = 0;
		}
	return iCount;
	}

size_t ChanRU_UTF::UnreadableLimit()
	{ return size_t(-1); }

ChanRU_UTF::operator operator_bool() const
	{ return operator_bool_gen::translate(fToken != eToken_Exhausted); }

EToken ChanRU_UTF::Current() const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	return fToken;
	}

ChanRU_UTF& ChanRU_UTF::Advance()
	{
	if (fToken == eToken_Fresh)
		{
		this->pAdvance();
		}
	else
		{
		// If we're on a text token, this will skip it. Otherwise it's a no-op.
		sSkipAll(*this);

		fToken = eToken_Fresh;
		}
	return *this;
	}

const string& ChanRU_UTF::Name() const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	if (fToken == eToken_TagBegin || fToken == eToken_TagEnd || fToken == eToken_TagEmpty)
		return fTagName;

	return sDefault<string>();
	}

Attrs_t ChanRU_UTF::Attrs() const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	if (fToken == eToken_TagBegin || fToken == eToken_TagEmpty)
		return fTagAttributes;

	return Attrs_t();
	}

ZQ<string> ChanRU_UTF::QAttr(const string& iAttrName) const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	if (fToken == eToken_TagBegin || fToken == eToken_TagEmpty)
		{
		for (Attrs_t::const_iterator ii = fTagAttributes.begin(); ii != fTagAttributes.end(); ++ii)
			{
			if (ii->first == iAttrName)
				return ii->second;
			}
		}
	return null;
	}

string ChanRU_UTF::Attr(const string& iAttrName) const
	{
	if (ZQ<string> theAttr = this->QAttr(iAttrName))
		return *theAttr;
	return string();
	}

// The semantics of this do not precisely match those of other sTryRead_XXX methods.
// We will consume code points from \a s up to and including the failing code point.
static bool spTryRead_String(const string8& iPattern, const ChanR_UTF& iChanR)
	{
	for (string8::const_iterator iter = iPattern.begin(), iterEnd = iPattern.end();
		/*no test*/; /*no inc*/)
		{
		UTF32 patternCP;
		if (not Unicode::sReadInc(iter, iterEnd, patternCP))
			{
			// Exhausted the pattern, we've matched everything.
			return true;
			}

		if (NotQ<UTF32> targetCPQ = sQRead(iChanR))
			{
			// Exhausted the target before seeing the pattern.
			return false;
			}
		else if (*targetCPQ != patternCP)
			{
			// Mismatch.
			return false;
			}
		}
	}

void ChanRU_UTF::pAdvance() const
	{ const_cast<ChanRU_UTF*>(this)->pAdvance(); }

void ChanRU_UTF::pAdvance()
	{
//	using namespace ZUtil_Strim;

	fTagAttributes.clear();

	for	(;;)
		{
		if (NotQ<UTF32> theCPQ = sQRead(fChanR))
			{
			fToken = eToken_Exhausted;
			return;
			}
		else if (*theCPQ != '<')
			{
			sUnread(fChanU, *theCPQ);
			fToken = eToken_Text;
			return;
			}

		sSkip_WS(fChanR, fChanU);

		if (NotQ<UTF32> theCPQ = sQRead(fChanR))
			{
			fToken = eToken_Exhausted;
			return;
			}
		else switch (*theCPQ)
			{
			case'/':
				{
				sSkip_WS(fChanR, fChanU);

				if (not spReadMLIdentifier(fChanR, fChanU, fTagName))
					{
					fToken = eToken_Exhausted;
					return;
					}

				sSkip_WS(fChanR, fChanU);

				if (not sTryRead_CP('>', fChanR, fChanU))
					{
					fToken = eToken_Exhausted;
					return;
					}

				fToken = eToken_TagEnd;
				return;
				}
			case '?':
				{
				// PI
				sSkip_Until(fChanR, "?>");
				break;
				}
			case '!':
				{
				if (sTryRead_CP('-', fChanR, fChanU))
					{
					if (sTryRead_CP('-', fChanR, fChanU))
						{
						// A comment.
						sSkip_Until(fChanR, "-->");
						}
					else
						{
						// Not a comment, but not an entity definition. Just skip
						// till we hit a '>'
						sSkip_Until(fChanR, ">");
						}
					}
				else if (spTryRead_String("[CDATA[", fChanR))
					{
					// CDATA
					sSkip_Until(fChanR, "]]>");
					}
				else
					{
					// An entity definition
					sSkip_Until(fChanR, ">");
					}
				break;
				}
			default:
				{
				sUnread(fChanU, *theCPQ);

				if (not spReadMLIdentifier(fChanR, fChanU, fTagName))
					{
					fToken = eToken_Exhausted;
					return;
					}

				for (;;)
					{
					sSkip_WS(fChanR, fChanU);

					string attributeName;
					attributeName.reserve(8);
					if (not spReadMLAttributeName(fChanR, fChanU, attributeName))
						break;

					sSkip_WS(fChanR, fChanU);

					if (sTryRead_CP('=', fChanR, fChanU))
						{
						sSkip_WS(fChanR, fChanU);
						string attributeValue;
						attributeValue.reserve(8);
						if (not spReadMLAttributeValue(fChanR, fChanU,
							fRecognizeEntitiesInAttributeValues, fCallable,
							attributeValue))
							{
							fToken = eToken_Exhausted;
							return;
							}
						fTagAttributes.push_back(Attr_t(attributeName, attributeValue));
						}
					else
						{
						fTagAttributes.push_back(Attr_t(attributeName, string()));
						}
					}

				sSkip_WS(fChanR, fChanU);

				if (sTryRead_CP('/', fChanR, fChanU))
					fToken = eToken_TagEmpty;
				else
					fToken = eToken_TagBegin;

				if (not sTryRead_CP('>', fChanR, fChanU))
					fToken = eToken_Exhausted;

				return;
				}
			}
		}
	}

ZRef<ChannerRU_UTF> sChanner(const ZRef<ZooLib::ChannerRU<UTF32>>& iChanner)
	{ return sChanner_Channer_T<ChanRU_UTF>(iChanner); }

// =================================================================================================
#pragma mark -
#pragma mark ZML parsing support

void sSkipText(ChanRU_UTF& r)
	{
	while (r.Current() == eToken_Text)
		r.Advance();
	}

bool sSkip(ChanRU_UTF& r, const string& iTagName)
	{
	vector<string> theTags(1, iTagName);
	return sSkip(r, theTags);
	}

bool sSkip(ChanRU_UTF& r, vector<string>& ioTags)
	{
	while (!ioTags.empty())
		{
		switch (r.Current())
			{
			case eToken_TagBegin:
				{
				ioTags.push_back(r.Name());
				break;
				}
			case eToken_TagEnd:
				{
				if (r.Name() != ioTags.back())
					return false;
				ioTags.pop_back();
				break;
				}
			case eToken_Exhausted:
				{
				return false;
				}
			default:
				break;
			}
		r.Advance();
		}
	return true;
	}

bool sTryRead_Begin(ChanRU_UTF& r, const string& iTagName)
	{
	if (r.Current() != eToken_TagBegin || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}

bool sTryRead_Empty(ChanRU_UTF& r, const string& iTagName)
	{
	if (r.Current() != eToken_TagEmpty || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}

bool sTryRead_End(ChanRU_UTF& r, const string& iTagName)
	{
	if (r.Current() != eToken_TagEnd || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark ZML::StrimW

#if 0

/** \class ZML::StrimW
ZML::StrimW extends the ZStrimW protocol with methods to open and close
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
ZML::StrimW can ensure that the tag you think you're closing is indeed
the last tag opened. Note that tag names, attribute names and attribute
values are stored, compared and emitted as-is -- no case conversion is performed,
so closing an 'HTML' begin tag with an 'html' end tag will trip an assertion.

ZML::StrimW::Tag exists to allow for the generation of tags that do not have their
balance checked, in particular common HTML tags such as BR, P, HR, LI, IMG etc.

To generate XML empty tags e.g. <sometag param="something"/> use ZML::StrimW::Empty.

After a call to ZML::StrimW::Begin, ZML::StrimW::Empty or ZML::StrimW::Tag
the strim is in a mode whereby attributes may be added to the tag. A call to
ZML::StrimW::End or any call to the normal write methods will cause
the pending tag plus its attributes to be emitted to the sink strim
followed by the actual write. ZML::StrimW::Attr trips an assertion if the strim
is not in the correct mode.

Note that ZML::StrimW descends from NonCopyable, in order to enforce
that it can't be assigned or copy constructed from another ZML::StrimW. Nevertheless
we declare a copy constructor and then don't define it. Why? In the words of
<a href="http://groups.google.com/group/comp.lang.c++/tree/browse_frm/thread/
13b9515221185fc0/4ae1743b3e3abeac?rnum=1&_done=
%2Fgroup%2Fcomp.lang.c%2B%2B%2Fbrowse_frm%2Fthread%2F13b9515221185fc0
%2F4ae1743b3e3abeac%3Ftvc%3D1%26#doc_59d07f9519b39ad1">
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

We often want to be able to pass a temporary ZML::StrimW initialized with a
regular strim to a function taking a const ZML::StrimW&. In gcc pre-4.0 this
worked just as we want, and gcc post-4.0 still does the same thing (a),
but now enforces the requirement that a copy constructor be visible.

The upshot is that if you get an error at link time, stating that no
implementation of ZML::StrimW(const ZML::StrimW&) can be found, you've
got yourself a compiler that's doing (b) and we'll need to rethink things.
*/

StrimW::StrimW(const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fTagType(eTagTypeNone),
	fWrittenSinceLastTag(true),
	fLastWasBegin(false),
	fLastWasEOL(false),
	fIndentEnabled(true),
	fString_EOL("\n"),
	fString_Indent("  ")
	{}

StrimW::StrimW(bool iIndent, const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fTagType(eTagTypeNone),
	fWrittenSinceLastTag(true),
	fLastWasBegin(false),
	fLastWasEOL(false),
	fIndentEnabled(iIndent),
	fString_EOL("\n"),
	fString_Indent("  ")
	{}

StrimW::StrimW(const string8& iEOL, const string8& iIndent, const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fTagType(eTagTypeNone),
	fWrittenSinceLastTag(true),
	fLastWasBegin(false),
	fLastWasEOL(false),
	fIndentEnabled(true),
	fString_EOL(iEOL),
	fString_Indent(iIndent)
	{}

StrimW::~StrimW()
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

static void spWriteIndent(const ZStrimW& iStrim, const string8& iString, size_t iCount)
	{
	while (iCount--)
		iStrim.Write(iString);
	}

void StrimW::Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU)
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
				size_t countWritten;
				fStrimSink.Write(localSource, countToWrite, &countWritten);
				if (not countWritten)
					break;

				localSource += countWritten;
				if (countWritten < countToWrite)
					{
					// We weren't able to write the whole chunk, so just
					// return to the top of the for loop and try again.
					continue;
					}
				}

			// Write the substitution text.
			switch (*theEntityCPQ)
				{
				case '"': fStrimSink.Write("&quot;"); break;
				case '&': fStrimSink.Write("&amp;"); break;
				case '<': fStrimSink.Write("&lt;"); break;
				case '>': fStrimSink.Write("&gt;"); break;
				case 0x00A0: fStrimSink.Write("&nbsp;"); break;
				default: fStrimSink.Writef("&#%u;", (unsigned int)*theEntityCPQ);
				}

			localSource = current;
			}
		else
			{
			size_t countToWrite = current - localSource;
			if (not countToWrite)
				break;
			size_t countWritten;
			fStrimSink.Write(localSource, countToWrite, &countWritten);
			if (not countWritten)
				break;
			localSource += countWritten;
			}
		}

	if (oCountCU)
		*oCountCU = localSource - iSource;

	UTF32 theCP;
	if (Unicode::sDecRead(iSource, localSource, localSourceEnd, theCP))
		fLastWasEOL = Unicode::sIsEOL(theCP);
	}

void StrimW::Imp_Flush()
	{
	sNonConst(this)->pWritePending();
	fStrimSink.Flush();
	}

const ZStrimW& StrimW::Raw() const
	{
	sNonConst(this)->pWritePending();
	return fStrimSink;
	}

// An easy way to directly include an NBSP in a string
// is with the code unit sequence "\xC2\xA0".
const StrimW& StrimW::WriteNBSP() const
	{
	sNonConst(this)->pPreText();

	fStrimSink.Write("&nbsp;");

	return *this;
	}

// The validity of iEntity is not checked -- be sure you
// use only legal characters.
const StrimW& StrimW::WriteEntity(const string8& iEntity) const
	{
	sNonConst(this)->pPreText();

	fStrimSink.Write("&");
	fStrimSink.Write(iEntity);
	fStrimSink.Write(";");

	return *this;
	}

const StrimW& StrimW::WriteEntity(const UTF8* iEntity) const
	{
	sNonConst(this)->pPreText();

	fStrimSink.Write("&");
	fStrimSink.Write(iEntity);
	fStrimSink.Write(";");

	return *this;
	}

const StrimW& StrimW::Begin(const string8& iTag) const
	{
	sNonConst(this)->pBegin(iTag, eTagTypeNormal);

	return *this;
	}

const StrimW& StrimW::End(const string8& iTag) const
	{
	sNonConst(this)->pWritePending();

	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());
	ZAssertStopf(kDebug_StrimW_ML, fTags.back() == iTag,
		"Expected \"%s\", got \"%s\"", fTags.back().c_str(), iTag.c_str());

	sNonConst(this)->pEnd();

	return *this;
	}

const StrimW& StrimW::End() const
	{
	sNonConst(this)->pWritePending();

	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());
	sNonConst(this)->pEnd();

	return *this;
	}

const StrimW& StrimW::Empty(const string8& iTag) const
	{
	sNonConst(this)->pBegin(iTag, eTagTypeEmpty);

	return *this;
	}

const StrimW& StrimW::PI(const string8& iTag) const
	{
	sNonConst(this)->pBegin(iTag, eTagTypePI);

	return *this;
	}

const StrimW& StrimW::Tag(const string8& iTag) const
	{
	sNonConst(this)->pBegin(iTag, eTagTypeNoEnd);

	return *this;
	}

const StrimW& StrimW::Attr(const string8& iName) const
	{
	sNonConst(this)->pAttr(iName, nullptr);

	return *this;
	}

const StrimW& StrimW::Attr(const string8& iName, const string8& iValue) const
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

const StrimW& StrimW::Attr(const string8& iName, const UTF8* iValue) const
	{ return this->Attr(iName, string8(iValue)); }

const StrimW& StrimW::Attr(const string8& iName, int iValue) const
	{
	string8 theValue = sStringf("%d", iValue);
	sNonConst(this)->pAttr(iName, new string8(theValue));

	return *this;
	}

const StrimW& StrimW::Attrf(const string8& iName, const UTF8* iValue, ...) const
	{
	va_list args;
	va_start(args, iValue);
	string8 theValue = sStringf(iValue, args);
	va_end(args);

	sNonConst(this)->pAttr(iName, new string8(theValue));

	return *this;
	}

const StrimW& StrimW::Attrs(const Attrs_t& iMap) const
	{
	for (Attrs_t::const_iterator ii = iMap.begin(); ii != iMap.end(); ++ii)
		this->Attr(ii->first, ii->second);

	return *this;
	}

bool StrimW::Indent(bool iIndent) const
	{
	sNonConst(this)->pWritePending();

	bool oldIndent = fIndentEnabled;
	sNonConst(this)->fIndentEnabled = iIndent;
	return oldIndent;
	}

void StrimW::Abandon()
	{
	fTagType = eTagTypeNone;
	fTags.clear();
	fAttributeNames.clear();

	sDeleteAll(fAttributeValues.begin(), fAttributeValues.end());
	fAttributeValues.clear();
	}

const StrimW& StrimW::EndAll()
	{
	this->pWritePending();

	while (!fTags.empty())
		this->pEnd();

	return *this;
	}

const StrimW& StrimW::EndAllIfPresent(const string8& iTag)
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

const StrimW& StrimW::EndAll(const string8& iTag)
	{
	this->pWritePending();

	for (bool gotIt = false; !gotIt && !fTags.empty(); /*no inc*/)
		{
		gotIt = (fTags.back() == iTag);
		this->pEnd();
		}

	return *this;
	}

const StrimW& StrimW::WritePending() const
	{
	sNonConst(this)->pWritePending();
	return *this;
	}

void StrimW::pBegin(const string8& iTag, ETagType iTagType)
	{
	ZAssertStop(1, !iTag.empty());
	this->pWritePending();
	fTags.push_back(iTag);
	fTagType = iTagType;
	fLastWasBegin = true;
	}

void StrimW::pPreText()
	{
	this->pWritePending();

	fWrittenSinceLastTag = true;
	fLastWasEOL = false;
	}

void StrimW::pPreTag()
	{
	if (fIndentEnabled)
		{
		if (not fWrittenSinceLastTag)
			{
			if (not fLastWasEOL)
				fStrimSink.Write(fString_EOL);
			spWriteIndent(fStrimSink, fString_Indent, fTags.size() - 1);
			}
		}
	fWrittenSinceLastTag = false;
	fLastWasEOL = false;
	}

void StrimW::pWritePending()
	{
	if (fTagType == eTagTypeNone)
		return;

	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());

	if (fIndentEnabled)
		{
		if (not fWrittenSinceLastTag)
			{
			if (not fLastWasEOL)
				fStrimSink.Write(fString_EOL);
			spWriteIndent(fStrimSink, fString_Indent, fTags.size() - 1);
			}
		}
	fWrittenSinceLastTag = false;
	fLastWasEOL = false;

	if (fTagType == eTagTypePI)
		fStrimSink.Write("<?");
	else
		fStrimSink.Write("<");

	fStrimSink.Write(fTags.back());

	if (not fAttributeNames.empty())
		{
		for (size_t xx = 0; xx < fAttributeNames.size(); ++xx)
			{
			fStrimSink.Write(" ");

			fStrimSink.Write(fAttributeNames[xx]);

			if (fAttributeValues[xx])
				{
				fStrimSink.Write("=\"");
				fStrimSink.Write(*fAttributeValues[xx]);
				fStrimSink.Write("\"");
				delete fAttributeValues[xx];
				}
			}
		fAttributeNames.clear();
		fAttributeValues.clear();
		}

	switch (fTagType)
		{
		case eTagTypeEmpty:
			fStrimSink.Write("/>");
			break;
		case eTagTypePI:
			fStrimSink.Write("?>");
			break;
		default:
			fStrimSink.Write(">");
			break;
		}

	if (fTagType != eTagTypeNormal)
		fTags.pop_back();

	fWrittenSinceLastTag = false;

	fTagType = eTagTypeNone;
	}

void StrimW::pAttr(const string8& iName, string8* iValue)
	{
	// We can only add an attribute if we've called Begin and there's
	// been no intervening call to Imp_WriteUTF8.
	ZAssertStop(kDebug_StrimW_ML, fTagType != eTagTypeNone);
	fAttributeNames.push_back(iName);
	fAttributeValues.push_back(iValue);
	}

void StrimW::pEnd()
	{
	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());

	if (fIndentEnabled)
		{
		if (not fWrittenSinceLastTag && not fLastWasBegin)
			{
			if (not fLastWasEOL)
				fStrimSink.Write(fString_EOL);
			spWriteIndent(fStrimSink, fString_Indent, fTags.size() - 1);
			}
		}
	fWrittenSinceLastTag = false;
	fLastWasEOL = false;
	fLastWasBegin = false;

	fStrimSink.Write("</");
	fStrimSink.Write(fTags.back());
	fStrimSink.Write(">");
	fTags.pop_back();

	fTagType = eTagTypeNone;
	}

#endif

// =================================================================================================
#pragma mark -
#pragma mark ZML::StrimW::Indenter

#if 0

StrimW::Indenter::Indenter(StrimW& iStrimW, bool iIndent)
:	fStrimW(iStrimW),
	fPriorIndent(iStrimW.Indent(iIndent))
	{}

StrimW::Indenter::~Indenter()
	{ fStrimW.Indent(fPriorIndent); }

#endif

// =================================================================================================
#pragma mark -
#pragma mark ZML::StrimmerW

#if 0

StrimmerW::StrimmerW(ZRef<ZStrimmerW> iStrimmerW)
:	fStrimmerW(iStrimmerW),
	fStrimW(iStrimmerW->GetStrimW())
	{}

StrimmerW::StrimmerW(bool iIndent, ZRef<ZStrimmerW> iStrimmerW)
:	fStrimmerW(iStrimmerW),
	fStrimW(iIndent, iStrimmerW->GetStrimW())
	{}

StrimmerW::StrimmerW(
	const string8& iEOL, const string8& iIndent, ZRef<ZStrimmerW> iStrimmerW)
:	fStrimmerW(iStrimmerW),
	fStrimW(iEOL, iIndent, iStrimmerW->GetStrimW())
	{}

StrimmerW::~StrimmerW()
	{}

const ZStrimW& StrimmerW::GetStrimW()
	{ return fStrimW; }

ZML::StrimW& StrimmerW::GetStrim()
	{ return fStrimW; }
#endif

} // namespace ZML
} // namespace ZooLib
