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

#include "zoolib/ZML.h"
#include "zoolib/ZStrimR_Boundary.h"
#include "zoolib/ZString.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_STL.h"

#include <stdio.h> // For sprintf

namespace ZooLib {

namespace ZML {

#define kDebug_StrimW_ML 1

using std::min;
using std::pair;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static helper functions

static string spReadReference(
	const ZStrimU& iStrim, EntityCallback iCallback, void* iRefcon)
	{
	using namespace ZUtil_Strim;

	string result;

	if (sTryRead_CP(iStrim, '#'))
		{
		// It's a character reference.
		int64 theInt;
		bool gotIt = false;
		if (sTryRead_CP(iStrim, 'x') || sTryRead_CP(iStrim, 'X'))
			gotIt = sTryRead_HexInteger(iStrim, theInt);
		else
			gotIt = sTryRead_DecimalInteger(iStrim, theInt);

		if (gotIt && sTryRead_CP(iStrim, ';'))
			result += UTF32(theInt);
		}
	else
		{
		string theEntity;
		theEntity.reserve(8);
		for (;;)
			{
			UTF32 theCP;
			if (!iStrim.ReadCP(theCP))
				{
				theEntity.clear();
				break;
				}

			if (theCP == ';')
				break;

			if (ZUnicode::sIsWhitespace(theCP))
				{
				theEntity.clear();
				break;
				}

			theEntity += theCP;
			}

		if (!theEntity.empty())
			{
			if (iCallback)
				result = iCallback(iRefcon, theEntity);
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

static bool spReadMLIdentifier(const ZStrimU& s, string& oText)
	{
	oText.resize(0);

	UTF32 curCP;
	if (!s.ReadCP(curCP))
		return false;

	if (!ZUnicode::sIsAlpha(curCP) && curCP != '_' && curCP != '?' && curCP != '!')
		{
		s.Unread(curCP);
		return false;
		}

	oText += curCP;

	for (;;)
		{
		if (!s.ReadCP(curCP))
			{
			break;
			}
		else if (!ZUnicode::sIsAlphaDigit(curCP) && curCP != '_' && curCP != '-' && curCP != ':')
			{
			s.Unread(curCP);
			break;
			}
		else
			{
			oText += curCP;
			}
		}

	return true;
	}

static bool spReadUntil(const ZStrimU& s, UTF32 iTerminator, string& oText)
	{
	oText.resize(0);

	for (;;)
		{
		UTF32 theCP;
		if (!s.ReadCP(theCP))
			return false;

		if (theCP == iTerminator)
			return true;

		oText += theCP;
		}
	}

static bool spReadUntil(
	const ZStrimU& s, bool iRecognizeEntities,
	EntityCallback iCallback, void* iRefcon,
	UTF32 iTerminator, string& oText)
	{
	oText.resize(0);

	for (;;)
		{
		UTF32 theCP;
		if (!s.ReadCP(theCP))
			{
			return false;
			}
		else if (theCP == iTerminator)
			{
			return true;
			}
		else if (theCP == '&' && iRecognizeEntities)
			{
			oText += spReadReference(s, iCallback, iRefcon);
			}
		else
			{
			oText += theCP;
			}
		}
	}

static bool spReadMLAttributeName(const ZStrimU& s, string& oName)
	{
	oName.resize(0);

	UTF32 curCP;
	if (!s.ReadCP(curCP))
		return false;

	if (curCP == '"')
		{
		return spReadUntil(s, '"', oName);
		}
	else if (curCP == '\'')
		{
		return spReadUntil(s, '\'', oName);
		}
	else
		{
		if (!ZUnicode::sIsAlpha(curCP) && curCP != '_' && curCP != '?' && curCP != '!')
			{
			s.Unread(curCP);
			return false;
			}

		oName += curCP;
		for (;;)
			{
			if (!s.ReadCP(curCP))
				break;

			if (!ZUnicode::sIsAlphaDigit(curCP) && curCP != '_' && curCP != '-' && curCP != ':')
				{
				s.Unread(curCP);
				break;
				}

			oName += curCP;
			}
		}

	return true;
	}

static bool spReadMLAttributeValue(
	const ZStrimU& s, bool iRecognizeEntities,
	EntityCallback iCallback, void* iRefcon,
	string& oValue)
	{
	oValue.resize(0);

	UTF32 curCP;
	if (!s.ReadCP(curCP))
		return false;

	if (curCP == '"')
		{
		return spReadUntil(s, iRecognizeEntities,
			iCallback, iRefcon, '"', oValue);
		}
	else if (curCP == '\'')
		{
		return spReadUntil(s, iRecognizeEntities,
			iCallback, iRefcon, '\'', oValue);
		}
	else
		{
		s.Unread(curCP);

		ZUtil_Strim::sSkip_WS(s);

		for (;;)
			{
			if (!s.ReadCP(curCP))
				{
				break;
				}
			else if (curCP == '>')
				{
				s.Unread(curCP);
				break;
				}
			else if (ZUnicode::sIsWhitespace(curCP))
				{
				break;
				}
			else if (curCP == '&' && iRecognizeEntities)
				{
				oValue += spReadReference(s, iCallback, iRefcon);
				}
			else
				{
				oValue += curCP;
				}
			}
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZML::StrimU

StrimU::StrimU(const ZStrimU& iStrim)
:	fStrim(iStrim),
	fCallback(nullptr),
	fBufferStart(0),
	fToken(eToken_Fresh),
	fRecognizeEntitiesInAttributeValues(false)
	{}

StrimU::StrimU(const ZStrimU& iStrim, bool iRecognizeEntitiesInAttributeValues)
:	fStrim(iStrim),
	fCallback(nullptr),
	fBufferStart(0),
	fToken(eToken_Fresh),
	fRecognizeEntitiesInAttributeValues(iRecognizeEntitiesInAttributeValues)
	{}

StrimU::StrimU(const ZStrimU& iStrim, EntityCallback iCallback, void* iRefcon)
:	fStrim(iStrim),
	fCallback(iCallback),
	fRefcon(iRefcon),
	fBufferStart(0),
	fToken(eToken_Fresh),
	fRecognizeEntitiesInAttributeValues(false)
	{}

StrimU::~StrimU()
	{}

void StrimU::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
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
			else
				{
				UTF32 theCP;
				if (!fStrim.ReadCP(theCP))
					{
					fToken = eToken_Exhausted;
					break;
					}
				else if (theCP == '<')
					{
					fStrim.Unread(theCP);
					break;
					}
				else if (theCP == '&')
					{
					fBufferStart = 0;
					fBuffer = ZUnicode::sAsUTF32(spReadReference(fStrim, fCallback, fRefcon));
					}
				else
					{
					*localDest++ = theCP;
					--iCount;
					}
				}
			}
		}

	if (oCount)
		*oCount = localDest - oDest;
	}

void StrimU::Imp_Unread(UTF32 iCP)
	{
	ZAssert(fToken == eToken_Text);

	if (fBufferStart == 0)
		fBuffer.insert(fBuffer.begin(), iCP);
	else
		--fBufferStart;

	ZAssert(fBuffer[fBufferStart] == iCP);
	}

size_t StrimU::Imp_UnreadableLimit()
	{ return size_t(-1); }

StrimU::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fToken != eToken_Exhausted); }

EToken StrimU::Current() const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	return fToken;
	}

StrimU& StrimU::Advance()
	{
	if (fToken == eToken_Fresh)
		{
		this->pAdvance();
		}
	else
		{
		// If we're on a text token, this will skip it. Otherwise it's a no-op.
		this->SkipAll();

		fToken = eToken_Fresh;
		}
	return *this;
	}

static string spEmptyString;

const string& StrimU::Name() const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	if (fToken == eToken_TagBegin || fToken == eToken_TagEnd || fToken == eToken_TagEmpty)
		return fTagName;

	return spEmptyString;
	}

Attrs_t StrimU::Attrs() const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	if (fToken == eToken_TagBegin || fToken == eToken_TagEmpty)
		return fTagAttributes;

	return Attrs_t();
	}

// The semantics of this do not precisely match those of other sTryRead_XXX methods.
// We will consume code points from \a s up to and including the failing code point.
static bool spTryRead_String(const ZStrimR& iStrimR, const string8& iPattern)
	{
	for (string8::const_iterator iter = iPattern.begin(), iterEnd = iPattern.end();
		/*no test*/;/*no inc*/)
		{
		UTF32 patternCP;
		if (!ZUnicode::sReadInc(iter, iterEnd, patternCP))
			{
			// Exhausted the pattern, we've matched everything.
			return true;
			}

		UTF32 targetCP;
		if (!iStrimR.ReadCP(targetCP))
			{
			// Exhausted the target before seeing the pattern.
			return false;
			}

		if (targetCP != patternCP)
			{
			// Mismatch.
			return false;
			}
		}
	}

void StrimU::pAdvance() const
	{ const_cast<StrimU*>(this)->pAdvance(); }

void StrimU::pAdvance()
	{
	using namespace ZUtil_Strim;

	fTagAttributes.clear();

	for	(;;)
		{
		UTF32 theCP;
		if (!fStrim.ReadCP(theCP))
			{
			fToken = eToken_Exhausted;
			return;
			}

		if (theCP != '<')
			{
			fStrim.Unread(theCP);
			fToken = eToken_Text;
			return;
			}

		sSkip_WS(fStrim);

		if (!fStrim.ReadCP(theCP))
			{
			fToken = eToken_Exhausted;
			return;
			}

		switch (theCP)
			{
			case'/':
				{
				sSkip_WS(fStrim);

				if (!spReadMLIdentifier(fStrim, fTagName))
					{
					fToken = eToken_Exhausted;
					return;
					}

				sSkip_WS(fStrim);

				if (!sTryRead_CP(fStrim, '>'))
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
				ZStrimR_Boundary("?>", fStrim).SkipAll();
				break;
				}
			case '!':
				{
				if (sTryRead_CP(fStrim, '-'))
					{
					if (sTryRead_CP(fStrim, '-'))
						{
						// A comment.
						ZStrimR_Boundary("-->", fStrim).SkipAll();
						}
					else
						{
						// Not a comment, but not an entity definition. Just skip
						// till we hit a '>'
						ZStrimR_Boundary(">", fStrim).SkipAll();
						}
					}
				else if (spTryRead_String(fStrim, "[CDATA["))
					{
					// CDATA
					ZStrimR_Boundary("]]>", fStrim).SkipAll();
					}
				else
					{
					// An entity definition
					ZStrimR_Boundary(">", fStrim).SkipAll();
					}
				break;
				}
			default:
				{
				fStrim.Unread(theCP);

				if (!spReadMLIdentifier(fStrim, fTagName))
					{
					fToken = eToken_Exhausted;
					return;
					}

				for (;;)
					{
					sSkip_WS(fStrim);

					string attributeName;
					attributeName.reserve(8);
					if (!spReadMLAttributeName(fStrim, attributeName))
						break;

					sSkip_WS(fStrim);

					if (sTryRead_CP(fStrim, '='))
						{
						sSkip_WS(fStrim);
						string attributeValue;
						attributeValue.reserve(8);
						if (!spReadMLAttributeValue(
							fStrim, fRecognizeEntitiesInAttributeValues,
							fCallback, fRefcon, attributeValue))
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

				sSkip_WS(fStrim);

				if (sTryRead_CP(fStrim, '/'))
					fToken = eToken_TagEmpty;
				else
					fToken = eToken_TagBegin;

				if (!sTryRead_CP(fStrim, '>'))
					fToken = eToken_Exhausted;

				return;
				}
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZML::StrimmerU

StrimmerU::StrimmerU(ZRef<ZStrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
,	fStrim(iStrimmerU->GetStrimU())
	{}

StrimmerU::StrimmerU(ZRef<ZStrimmerU> iStrimmerU, bool iRecognizeEntitiesInAttributeValues)
:	fStrimmerU(iStrimmerU)
,	fStrim(iStrimmerU->GetStrimU(), iRecognizeEntitiesInAttributeValues)
	{}

StrimmerU::StrimmerU(ZRef<ZStrimmerU> iStrimmerU, EntityCallback iCallback, void* iRefcon)
:	fStrimmerU(iStrimmerU)
,	fStrim(iStrimmerU->GetStrimU(), iCallback, iRefcon)
	{}

StrimmerU::~StrimmerU()
	{}

const ZStrimU& StrimmerU::GetStrimU()
	{ return fStrim; }

StrimU& StrimmerU::GetStrim()
	{ return fStrim; }

// =================================================================================================
#pragma mark -
#pragma mark * ZML parsing support

void sSkipText(StrimU& r)
	{
	while (r.Current() == eToken_Text)
		r.Advance();
	}

bool sSkip(StrimU& r, const string& iTagName)
	{
	vector<string> theTags(1, iTagName);
	return sSkip(r, theTags);
	}

bool sSkip(StrimU& r, vector<string>& ioTags)
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
			}
		r.Advance();
		}
	return true;
	}

bool sTryRead_Begin(StrimU& r, const string& iTagName)
	{
	if (r.Current() != eToken_TagBegin || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}

bool sTryRead_End(StrimU& r, const string& iTagName)
	{
	if (r.Current() != eToken_TagEnd || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZML::StrimR_TextOnly

#if 0

StrimR_TextOnly::StrimR_TextOnly(StrimU& iStrimU)
:	fStrimU(iStrimU)
	{}

void StrimR_TextOnly::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	UTF32* localDest = oDest;
	while (iCount)
		{
		if (fStrimU.Current() == eToken_Text)
			{
			size_t countRead;
			fStrimU.Read(localDest, iCount, &countRead);
			if (countRead == 0)
				{
				fStrimU.Advance();
				}
			else
				{
				localDest += countRead;
				iCount -= countRead;
				}
			}
		else
			{
			switch (fStrimU.Current())
				{
				case eToken_TagBegin:
					{
					fTags.push_back(pair<string, Attrs_t>(fStrimU.Name(), fStrimU.Attrs()));
					break;
					}
				case eToken_TagEnd:
					{
					if (!fTags.empty())
						fTags.pop_back();
					break;
					}
				case eToken_Exhausted:
					{
					iCount = 0;
					break;
					}
				default:
					break;
				}
			fStrimU.Advance();
			}
		}

	if (oCount)
		*oCount = localDest - oDest;
	}

string StrimR_TextOnly::BackName() const
	{
	ZAssert(!fTags.empty());
	return fTags.back().first;
	}

Attrs_t StrimR_TextOnly::BackAttr() const
	{
	ZAssert(!fTags.empty());
	return fTags.back().second;
	}

void StrimR_TextOnly::AllNames(vector<string>& oNames) const
	{
	for (vector<pair<string, Attrs_t> >::const_iterator i = fTags.begin();
		i != fTags.end(); ++i)
		{
		oNames.push_back((*i).first);
		}
	}

void StrimR_TextOnly::AllAttrs(vector<Attrs_t>& oAttrs) const
	{
	for (vector<pair<string, Attrs_t> >::const_iterator i = fTags.begin();
		i != fTags.end(); ++i)
		{
		oAttrs.push_back((*i).second);
		}
	}

const vector<pair<string, Attrs_t> >& StrimR_TextOnly::All() const
	{
	return fTags;
	}

#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZML::StrimW

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

namespace { // anonymous

struct Entity_t
	{
	const UTF8* fText;
	size_t fLength;
	};

const Entity_t spEntities[] =
	{
	{ "&quot;", 6 },
	{ "&amp;", 5 },
	{ "&lt;", 4 },
	{ "&gt;", 4 },
	{ "&nbsp;", 6 },
	};

} // anonymous namespace

void StrimW::Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU)
	{
	this->pPreText();

	const UTF8* localSource = iSource;
	const UTF8* localSourceEnd = iSource + iCountCU;
	for (;;)
		{
		// Walk forward through the source till we find a entity CP.
		const Entity_t* entity = nullptr;
		const UTF8* current = localSource;
		const UTF8* priorToEntity;
		while (!entity)
			{
			priorToEntity = current;
			UTF32 theCP;
			if (!ZUnicode::sReadInc(current, localSourceEnd, theCP))
				break;
			switch (theCP)
				{
				case '"': entity = &spEntities[0]; break;
				case '&': entity = &spEntities[1]; break;
				case '<': entity = &spEntities[2]; break;
				case '>': entity = &spEntities[3]; break;
				case 0x00A0: entity = &spEntities[4]; break;
				}
			}

		size_t countWritten;
		if (entity)
			{
			// Write everything prior to the entity.
			if (size_t countToWrite = priorToEntity - localSource)
				{
				fStrimSink.Write(localSource, countToWrite, &countWritten);
				if (!countWritten)
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
			fStrimSink.Write(entity->fText, entity->fLength, &countWritten);
			if (!countWritten)
				break;

			// fStrimSink consumed the subsitution text, so advance localSource
			// past the entity code point.
			localSource = current;
			}
		else
			{
			size_t countToWrite = current - localSource;
			if (!countToWrite)
				break;
			fStrimSink.Write(localSource, countToWrite, &countWritten);
			if (!countWritten)
				break;
			localSource += countWritten;
			}
		}

	if (oCountCU)
		*oCountCU = localSource - iSource;

	UTF32 theCP;
	if (ZUnicode::sDecRead(iSource, localSource, localSourceEnd, theCP))
		fLastWasEOL = ZUnicode::sIsEOL(theCP);
	}

void StrimW::Imp_Flush()
	{
	const_cast<StrimW*>(this)->pWritePending();
	fStrimSink.Flush();
	}

const ZStrimW& StrimW::Raw() const
	{
	const_cast<StrimW*>(this)->pWritePending();
	return fStrimSink;
	}

// An easy way to directly include an NBSP in a string
// is with the code unit sequence "\xC2\xA0".
const StrimW& StrimW::WriteNBSP() const
	{
	const_cast<StrimW*>(this)->pPreText();

	fStrimSink.Write("&nbsp;");

	return *this;
	}

// The validity of iEntity is not checked -- be sure you
// use only legal characters.
const StrimW& StrimW::WriteEntity(const string8& iEntity) const
	{
	const_cast<StrimW*>(this)->pPreText();

	fStrimSink.Write("&");
	fStrimSink.Write(iEntity);
	fStrimSink.Write(";");

	return *this;
	}

const StrimW& StrimW::WriteEntity(const UTF8* iEntity) const
	{
	const_cast<StrimW*>(this)->pPreText();

	fStrimSink.Write("&");
	fStrimSink.Write(iEntity);
	fStrimSink.Write(";");

	return *this;
	}

const StrimW& StrimW::Begin(const string8& iTag) const
	{
	const_cast<StrimW*>(this)->pBegin(iTag, eTagTypeNormal);

	return *this;
	}

const StrimW& StrimW::End(const string8& iTag) const
	{
	const_cast<StrimW*>(this)->pWritePending();

	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());
	ZAssertStopf(kDebug_StrimW_ML, fTags.back() == iTag,
		("Expected \"%s\", got \"%s\"", fTags.back().c_str(), iTag.c_str()));

	const_cast<StrimW*>(this)->pEnd();

	return *this;
	}

const StrimW& StrimW::End() const
	{
	const_cast<StrimW*>(this)->pWritePending();

	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());
	const_cast<StrimW*>(this)->pEnd();

	return *this;
	}

const StrimW& StrimW::Empty(const string8& iTag) const
	{
	const_cast<StrimW*>(this)->pBegin(iTag, eTagTypeEmpty);

	return *this;
	}

const StrimW& StrimW::PI(const string8& iTag) const
	{
	const_cast<StrimW*>(this)->pBegin(iTag, eTagTypePI);

	return *this;
	}

const StrimW& StrimW::Tag(const string8& iTag) const
	{
	const_cast<StrimW*>(this)->pBegin(iTag, eTagTypeNoEnd);

	return *this;
	}

const StrimW& StrimW::Attr(const string8& iName) const
	{
	const_cast<StrimW*>(this)->pAttr(iName, nullptr);

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
			if (!ZUnicode::sReadInc(current, end, theCP))
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
							sprintf(smallEntity, "&#x%02X;", theCP);
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
				ZUnicode::sDec(start, current, end);
				newValue = new string8(start, current);
				newValue->reserve(iValue.size());
				}
			}
		if (!newValue)
			{
			// We saw no CPs that need entity encoding, so never allocated newValue
			// nor did we accumulate values into it. So we can do so now and will
			// share the underlying rep held by iValue.
			newValue = new string8(iValue);
			}
		}

	const_cast<StrimW*>(this)->pAttr(iName, newValue);

	return *this;
	}

const StrimW& StrimW::Attr(const string8& iName, const UTF8* iValue) const
	{ return this->Attr(iName, string8(iValue)); }

const StrimW& StrimW::Attr(const string8& iName, int iValue) const
	{
	string8 theValue = ZString::sFromInt(iValue);
	const_cast<StrimW*>(this)->pAttr(iName, new string8(theValue));

	return *this;
	}

const StrimW& StrimW::Attrf(const string8& iName, const UTF8* iValue, ...) const
	{
	va_list args;
	va_start(args, iValue);
	string8 theValue = ZStringf(iValue, args);
	va_end(args);

	const_cast<StrimW*>(this)->pAttr(iName, new string8(theValue));

	return *this;
	}

const StrimW& StrimW::Attrs(const Attrs_t& iMap) const
	{
	for (Attrs_t::const_iterator i = iMap.begin(); i != iMap.end(); ++i)
		this->Attr((*i).first, (*i).second);

	return *this;
	}

bool StrimW::Indent(bool iIndent) const
	{
	const_cast<StrimW*>(this)->pWritePending();

	bool oldIndent = fIndentEnabled;
	const_cast<StrimW*>(this)->fIndentEnabled = iIndent;
	return oldIndent;
	}

void StrimW::Abandon()
	{
	fTagType = eTagTypeNone;
	fTags.clear();
	fAttributeNames.clear();

	ZUtil_STL::sDeleteAll(fAttributeValues.begin(), fAttributeValues.end());
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

	if (ZUtil_STL::sContains(fTags, iTag))
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
	const_cast<StrimW*>(this)->pWritePending();
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
		if (!fWrittenSinceLastTag)
			{
			if (!fLastWasEOL)
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
		if (!fWrittenSinceLastTag)
			{
			if (!fLastWasEOL)
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

	if (!fAttributeNames.empty())
		{
		for (size_t x = 0; x < fAttributeNames.size(); ++x)
			{
			fStrimSink.Write(" ");

			fStrimSink.Write(fAttributeNames[x]);

			if (fAttributeValues[x])
				{
				fStrimSink.Write("=\"");
				fStrimSink.Write(*fAttributeValues[x]);
				fStrimSink.Write("\"");
				delete fAttributeValues[x];
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
		if (!fWrittenSinceLastTag && !fLastWasBegin)
			{
			if (!fLastWasEOL)
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

// =================================================================================================
#pragma mark -
#pragma mark * ZML::StrimW::Indenter

StrimW::Indenter::Indenter(StrimW& iStrimW, bool iIndent)
:	fStrimW(iStrimW),
	fPriorIndent(iStrimW.Indent(iIndent))
	{}

StrimW::Indenter::~Indenter()
	{ fStrimW.Indent(fPriorIndent); }

// =================================================================================================
#pragma mark -
#pragma mark * ZML::StrimmerW

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

} // namespace ZML

} // namespace ZooLib
