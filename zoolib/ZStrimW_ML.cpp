/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZStrimW_ML.h"

#include "zoolib/ZCompat_cmath.h" // For fmod
#include "zoolib/ZDebug.h"
#include "zoolib/ZString.h"
#include "zoolib/ZTuple.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZUtil_Strim_Tuple.h"

#include <stdio.h>

NAMESPACE_ZOOLIB_BEGIN

#define kDebug_StrimW_ML 1

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_ML

/** \class ZStrimW_ML
ZStrimW_ML extends the ZStrimW protocol with methods to open and close
tags, attach attributes to those tags incrementally and pretty-print the
output. For example, assuming we're passed \c iStrimW:
\code
ZStrimW_ML s(iStrimW);
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
ZStrimW_ML can ensure that the tag you think you're closing is indeed
the last tag opened. Note that tag names, attribute names and attribute
values are stored, compared and emitted as-is -- no case conversion is performed,
so closing an 'HTML' begin tag with an 'html' end tag will trip an assertion.

ZStrimW_ML::Tag exists to allow for the generation of tags that do not have their
balance checked, in particular common HTML tags such as BR, P, HR, LI, IMG etc.

To generate XML empty tags e.g. <sometag param="something"/> use ZStrimW_ML::Empty.

After a call to ZStrimW_ML::Begin, ZStrimW_ML::Empty or ZStrimW_ML::Tag
the strim is in a mode whereby attributes may be added to the tag. A call to
ZStrimW_ML::End or any call to the normal write methods will cause
the pending tag plus its attributes to be emitted to the sink strim
followed by the actual write. ZStrimW_ML::Attr trips an assertion if the strim
is not in the correct mode.


Note that ZStrimW_ML descends from NonCopyable, in order to enforce
that it can't be assigned or copy constructed from another ZStrimW_ML. Nevertheless
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

We often want to be able to pass a temporary ZStrimW_ML initialized with a
regular strim to a function taking a const ZStrimW_ML&. In gcc pre-4.0 this
worked just as we want, and gcc post-4.0 still does the same thing (a),
but now enforces the requirement that a copy constructor be visible.

The upshot is that if you get an error at link time, stating that no
implementation of ZStrimW_ML(const ZStrimW_ML&) can be found, you've
got yourself a compiler that's doing (b) and we'll need to rethink things.
*/

ZStrimW_ML::ZStrimW_ML(const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fTagType(eTagTypeNone),
	fWrittenSinceLastTag(true),
	fLastWasBegin(false),
	fLastWasEOL(false),
	fIndentEnabled(true),
	fString_EOL("\n"),
	fString_Indent("  ")
	{}

ZStrimW_ML::ZStrimW_ML(bool iIndent, const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fTagType(eTagTypeNone),
	fWrittenSinceLastTag(true),
	fLastWasBegin(false),
	fLastWasEOL(false),
	fIndentEnabled(iIndent),
	fString_EOL("\n"),
	fString_Indent("  ")
	{}

ZStrimW_ML::ZStrimW_ML(const string8& iEOL, const string8& iIndent, const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fTagType(eTagTypeNone),
	fWrittenSinceLastTag(true),
	fLastWasBegin(false),
	fLastWasEOL(false),
	fIndentEnabled(true),
	fString_EOL(iEOL),
	fString_Indent(iIndent)
	{}

ZStrimW_ML::~ZStrimW_ML()
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

static void sWriteIndent(const ZStrimW& iStrim, const string8& iString, size_t iCount)
	{
	while (iCount--)
		iStrim.Write(iString);
	}

namespace {
struct Entity_t
	{
	const UTF8* fText;
	size_t fLength;
	};
} // anonymous namespace

static const Entity_t sEntities[] =
	{
	{ "&quot;", 6 },
	{ "&amp;", 5 },
	{ "&lt;", 4 },
	{ "&gt;", 4 },
	{ "&nbsp;", 6 },
	};

void ZStrimW_ML::Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU)
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
				case '"': entity = &sEntities[0]; break;
				case '&': entity = &sEntities[1]; break;
				case '<': entity = &sEntities[2]; break;
				case '>': entity = &sEntities[3]; break;
				case 0x00A0: entity = &sEntities[4]; break;
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

void ZStrimW_ML::Imp_Flush()
	{ fStrimSink.Flush(); }

const ZStrimW& ZStrimW_ML::Raw() const
	{
	const_cast<ZStrimW_ML*>(this)->pWritePending();
	return fStrimSink;
	}

// An easy way to directly include an NBSP in a string
// is with the code unit sequence "\xC2\xA0".
const ZStrimW_ML& ZStrimW_ML::WriteNBSP() const
	{
	const_cast<ZStrimW_ML*>(this)->pPreText();

	fStrimSink.Write("&nbsp;");

	return *this;
	}

// The validity of iEntity is not checked -- be sure you
// use only legal characters.
const ZStrimW_ML& ZStrimW_ML::WriteEntity(const string8& iEntity) const
	{
	const_cast<ZStrimW_ML*>(this)->pPreText();

	fStrimSink.Write("&");
	fStrimSink.Write(iEntity);
	fStrimSink.Write(";");

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::WriteEntity(const UTF8* iEntity) const
	{
	const_cast<ZStrimW_ML*>(this)->pPreText();

	fStrimSink.Write("&");
	fStrimSink.Write(iEntity);
	fStrimSink.Write(";");

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::Begin(const string8& iTag) const
	{
	const_cast<ZStrimW_ML*>(this)->pBegin(iTag, eTagTypeNormal);

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::End(const string8& iTag) const
	{
	const_cast<ZStrimW_ML*>(this)->pWritePending();

	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());
	ZAssertStopf(kDebug_StrimW_ML, fTags.back() == iTag,
		("Expected \"%s\", got \"%s\"", fTags.back().c_str(), iTag.c_str()));

	const_cast<ZStrimW_ML*>(this)->pEnd();

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::End() const
	{
	const_cast<ZStrimW_ML*>(this)->pWritePending();

	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());
	const_cast<ZStrimW_ML*>(this)->pEnd();

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::Empty(const string8& iTag) const
	{
	const_cast<ZStrimW_ML*>(this)->pBegin(iTag, eTagTypeEmpty);

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::PI(const string8& iTag) const
	{
	const_cast<ZStrimW_ML*>(this)->pBegin(iTag, eTagTypePI);

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::Tag(const string8& iTag) const
	{
	const_cast<ZStrimW_ML*>(this)->pBegin(iTag, eTagTypeNoEnd);

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::Attr(const string8& iName) const
	{
	const_cast<ZStrimW_ML*>(this)->pAttr(iName, nullptr);

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::Attr(const string8& iName, const string8& iValue) const
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
			// no did we accumulate values into it. So we can do so now and will
			// share the underlying rep held by iValue.
			newValue = new string8(iValue);
			}
		}

	const_cast<ZStrimW_ML*>(this)->pAttr(iName, newValue);

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::Attr(const string8& iName, const UTF8* iValue) const
	{ return this->Attr(iName, string8(iValue)); }

const ZStrimW_ML& ZStrimW_ML::Attr(const string8& iName, int iValue) const
	{
	string8 theValue = ZString::sFromInt(iValue);
	const_cast<ZStrimW_ML*>(this)->pAttr(iName, new string8(theValue));

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::Attrf(const string8& iName, const UTF8* iValue, ...) const
	{
	va_list args;
	va_start(args, iValue);
	string8 theValue = ZString::sFormat(iValue, args);
	va_end(args);

	const_cast<ZStrimW_ML*>(this)->pAttr(iName, new string8(theValue));

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::Attr(const string8& iName, const ZTValue& iValue) const
	{
	// The first few types, up to time, will not have any code points
	// that need entity encoding, and so we call pAttr directly.
	// String and the default case could contain anything, so we
	// call Attr(const string8&, const string8&) and return from the switch.
	string8* theString = nullptr;
	switch (iValue.TypeOf())
		{
		case eZType_Null:
			break;
		case eZType_ID:
			theString = new string8(ZString::sFormat("%lld", iValue.GetID()));
			break;
		case eZType_Int8:
			theString = new string8(ZString::sFormat("%d", iValue.GetInt8()));
			break;
		case eZType_Int16:
			theString = new string8(ZString::sFormat("%d", iValue.GetInt16()));
			break;
		case eZType_Int32:
			theString = new string8(ZString::sFormat("%d", iValue.GetInt32()));
			break;
		case eZType_Int64:
			theString = new string8(ZString::sFormat("%lld", iValue.GetInt64()));
			break;
		case eZType_Bool:
			{
			if (iValue.GetBool())
				theString = new string8("true");
			else
				theString = new string8("false");
			break;
			}
		case eZType_Float:
			theString = new string8(ZString::sFormat("%.9g", iValue.GetFloat()));
			break;
		case eZType_Double:
			theString = new string8(ZString::sFormat("%.17g", iValue.GetDouble()));
			break;
		case eZType_Time:
			{
			if (ZTime theTime = iValue.GetTime())
				theString = new string8(ZUtil_Time::sAsString_ISO8601(theTime, false));
			else
				theString = new string8;
			break;
			}
		case eZType_String:
			this->Attr(iName, iValue.GetString());
			return *this;
		default:
			this->Attr(iName, ZUtil_Strim_Tuple::sAsString(iValue));
			return *this;
		}
	const_cast<ZStrimW_ML*>(this)->pAttr(iName, theString);	

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::Attrs(const ZTuple& iTuple) const
	{
	for (ZTuple::const_iterator i = iTuple.begin(); i != iTuple.end(); ++i)
		this->Attr(iTuple.NameOf(i).AsString(), iTuple.GetValue(i));

	return *this;
	}

bool ZStrimW_ML::Indent(bool iIndent) const
	{
	const_cast<ZStrimW_ML*>(this)->pWritePending();

	bool oldIndent = fIndentEnabled;
	const_cast<ZStrimW_ML*>(this)->fIndentEnabled = iIndent;
	return oldIndent;
	}

void ZStrimW_ML::Abandon()
	{
	fTagType = eTagTypeNone;
	fTags.clear();
	fAttributeNames.clear();

	ZUtil_STL::sDeleteAll(fAttributeValues.begin(), fAttributeValues.end());
	fAttributeValues.clear();
	}

const ZStrimW_ML& ZStrimW_ML::EndAll()
	{
	this->pWritePending();

	while (!fTags.empty())
		this->pEnd();

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::EndAllIfPresent(const string8& iTag)
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

const ZStrimW_ML& ZStrimW_ML::EndAll(const string8& iTag)
	{
	this->pWritePending();

	for (bool gotIt = false; !gotIt && !fTags.empty(); /*no inc*/)
		{
		gotIt = (fTags.back() == iTag);
		this->pEnd();
		}

	return *this;
	}

const ZStrimW_ML& ZStrimW_ML::WritePending()
	{
	this->pWritePending();

	return *this;
	}

void ZStrimW_ML::pBegin(const string8& iTag, ETagType iTagType)
	{
	ZAssertStop(1, !iTag.empty());
	this->pWritePending();
	fTags.push_back(iTag);
	fTagType = iTagType;
	fLastWasBegin = true;
	}

void ZStrimW_ML::pPreText()
	{
	this->pWritePending();

	fWrittenSinceLastTag = true;
	fLastWasEOL = false;
	}

void ZStrimW_ML::pPreTag()
	{
	if (fIndentEnabled)
		{
		if (!fWrittenSinceLastTag)
			{
			if (!fLastWasEOL)
				fStrimSink.Write(fString_EOL);
			sWriteIndent(fStrimSink, fString_Indent, fTags.size() - 1);
			}
		}
	fWrittenSinceLastTag = false;
	fLastWasEOL = false;
	}

void ZStrimW_ML::pWritePending()
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
			sWriteIndent(fStrimSink, fString_Indent, fTags.size() - 1);
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

void ZStrimW_ML::pAttr(const string8& iName, string8* iValue)
	{
	// We can only add an attribute if we've called Begin and there's
	// been no intervening call to Imp_WriteUTF8.
	ZAssertStop(kDebug_StrimW_ML, fTagType != eTagTypeNone);
	fAttributeNames.push_back(iName);
	fAttributeValues.push_back(iValue);
	}

void ZStrimW_ML::pEnd()
	{
	ZAssertStop(kDebug_StrimW_ML, !fTags.empty());

	if (fIndentEnabled)
		{
		if (!fWrittenSinceLastTag && !fLastWasBegin)
			{
			if (!fLastWasEOL)
				fStrimSink.Write(fString_EOL);
			sWriteIndent(fStrimSink, fString_Indent, fTags.size() - 1);
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
#pragma mark * ZStrimW_ML::Indenter

ZStrimW_ML::Indenter::Indenter(ZStrimW_ML& iStrimW_ML, bool iIndent)
:	fStrimW_ML(iStrimW_ML),
	fPriorIndent(iStrimW_ML.Indent(iIndent))
	{}

ZStrimW_ML::Indenter::~Indenter()
	{ fStrimW_ML.Indent(fPriorIndent); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimmerW_ML

ZStrimmerW_ML::ZStrimmerW_ML(ZRef<ZStrimmerW> iStrimmer)
:	fStrimmer(iStrimmer),
	fStrim(iStrimmer->GetStrimW())
	{}

ZStrimmerW_ML::ZStrimmerW_ML(bool iIndent, ZRef<ZStrimmerW> iStrimmer)
:	fStrimmer(iStrimmer),
	fStrim(iIndent, iStrimmer->GetStrimW())
	{}

ZStrimmerW_ML::ZStrimmerW_ML(
	const string8& iEOL, const string8& iIndent, ZRef<ZStrimmerW> iStrimmer)
:	fStrimmer(iStrimmer),
	fStrim(iEOL, iIndent, iStrimmer->GetStrimW())
	{}

ZStrimmerW_ML::~ZStrimmerW_ML()
	{}

const ZStrimW& ZStrimmerW_ML::GetStrimW()
	{ return fStrim; }

NAMESPACE_ZOOLIB_END
