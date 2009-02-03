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
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZMemory.h"

NAMESPACE_ZOOLIB_BEGIN

using std::min;
using std::pair;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static helper functions

static string sReadReference(
	const ZStrimU& iStrim, ZML::Reader::EntityCallback iCallback, void* iRefcon)
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

static bool sReadMLIdentifier(const ZStrimU& s, string& oText)
	{
	oText.resize(0);

	UTF32 curCP;
	if (!s.ReadCP(curCP))
		return false;

	if (!ZUnicode::sIsAlpha(curCP) && curCP != '_' && curCP != '?' && curCP != '!')
		{
		s.Unread();
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
			s.Unread();
			break;
			}
		else
			{
			oText += curCP;
			}
		}

	return true;
	}

static bool sReadUntil(const ZStrimU& s, UTF32 iTerminator, string& oText)
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

static bool sReadUntil(
	const ZStrimU& s, bool iRecognizeEntities,
	ZML::Reader::EntityCallback iCallback, void* iRefcon,
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
			oText += sReadReference(s, iCallback, iRefcon);
			}
		else
			{
			oText += theCP;
			}
		}
	}

static bool sReadMLAttributeName(const ZStrimU& s, string& oName)
	{
	oName.resize(0);

	UTF32 curCP;
	if (!s.ReadCP(curCP))
		return false;

	if (curCP == '"')
		{
		return sReadUntil(s, '"', oName);
		}
	else if (curCP == '\'')
		{
		return sReadUntil(s, '\'', oName);
		}
	else
		{
		if (!ZUnicode::sIsAlpha(curCP) && curCP != '_' && curCP != '?' && curCP != '!')
			{
			s.Unread();
			return false;
			}

		oName += curCP;
		for (;;)
			{
			if (!s.ReadCP(curCP))
				break;

			if (!ZUnicode::sIsAlphaDigit(curCP) && curCP != '_' && curCP != '-' && curCP != ':')
				{
				s.Unread();
				break;
				}
				
			oName += curCP;
			}
		}

	return true;
	}

static bool sReadMLAttributeValue(
	const ZStrimU& s, bool iRecognizeEntities,
	ZML::Reader::EntityCallback iCallback, void* iRefcon,
	string& oValue)
	{
	oValue.resize(0);

	UTF32 curCP;
	if (!s.ReadCP(curCP))
		return false;

	if (curCP == '"')
		{
		return sReadUntil(s, iRecognizeEntities,
			iCallback, iRefcon, '"', oValue);
		}
	else if (curCP == '\'')
		{
		return sReadUntil(s, iRecognizeEntities,
			iCallback, iRefcon, '\'', oValue);
		}
	else
		{
		s.Unread();

		ZUtil_Strim::sSkip_WS(s);

		for (;;)
			{
			if (!s.ReadCP(curCP))
				{
				break;
				}
			else if (curCP == '>')
				{
				s.Unread();
				break;
				}
			else if (ZUnicode::sIsWhitespace(curCP))
				{
				break;
				}
			else if (curCP == '&' && iRecognizeEntities)
				{
				oValue += sReadReference(s, iCallback, iRefcon);
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
#pragma mark * ZML::Reader

ZML::Reader::Reader(const ZStrimU& iStrim)
:	fStrim(iStrim),
	fCallback(nil),
	fBufferStart(0),
	fToken(eToken_Exhausted),
	fRecognizeEntitiesInAttributeValues(false)
	{
	this->pAdvance();
	}

ZML::Reader::Reader(const ZStrimU& iStrim, bool iRecognizeEntitiesInAttributeValues)
:	fStrim(iStrim),
	fCallback(nil),
	fBufferStart(0),
	fToken(eToken_Exhausted),
	fRecognizeEntitiesInAttributeValues(iRecognizeEntitiesInAttributeValues)
	{
	this->pAdvance();
	}

ZML::Reader::Reader(const ZStrimU& iStrim, EntityCallback iCallback, void* iRefcon)
:	fStrim(iStrim),
	fCallback(iCallback),
	fRefcon(iRefcon),
	fBufferStart(0),
	fToken(eToken_Exhausted),
	fRecognizeEntitiesInAttributeValues(false)
	{
	this->pAdvance();
	}

ZML::Reader::~Reader()
	{}

ZML::Reader::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fToken != eToken_Exhausted); }

ZML::EToken ZML::Reader::Current() const
	{ return fToken; }

ZML::Reader& ZML::Reader::Advance()
	{
	if (fToken == eToken_Text)
		{
		// Suck up all the text till we hit the start of a tag.
		for (;;)
			{
			UTF32 theCP;
			if (!fStrim.ReadCP(theCP))
				{
				fToken = eToken_Exhausted;
				return *this;
				}
			else if (theCP == '<')
				{
				fStrim.Unread();
				break;
				}
			}
		}
	this->pAdvance();
	return *this;
	}

static string sEmptyString;

const string& ZML::Reader::Name() const
	{
	if (fToken == eToken_TagBegin || fToken == eToken_TagEnd || fToken == eToken_TagEmpty)
		return fTagName;
	return sEmptyString;
	}

ZTuple ZML::Reader::Attrs() const
	{
	if (fToken == eToken_TagBegin || fToken == eToken_TagEmpty)
		return fTagAttributes;
	return ZTuple();
	}

const ZStrimR& ZML::Reader::Text()
	{ return *this; }

void ZML::Reader::Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount)
	{
	UTF32* localDest = iDest;
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
					fStrim.Unread();
					break;
					}
				else if (theCP == '&')
					{
					fBufferStart = 0;
					fBuffer = ZUnicode::sAsUTF32(sReadReference(fStrim, fCallback, fRefcon));
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
		*oCount = localDest - iDest;
	}

// The semantics of this do not precisely match those of other sTryRead_XXX methods.
// We will consume code points from \a s up to and including the failing code point.
static bool sTryRead_String(const ZStrimR& iStrimR, const string8& iPattern)
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

void ZML::Reader::pAdvance()
	{
	using namespace ZUtil_Strim;

	fTagAttributes.Clear();

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
			fStrim.Unread();
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

				if (!sReadMLIdentifier(fStrim, fTagName))
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
				else if (sTryRead_String(fStrim, "[CDATA["))
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
				fStrim.Unread();

				if (!sReadMLIdentifier(fStrim, fTagName))
					{
					fToken = eToken_Exhausted;
					return;
					}

				for (;;)
					{
					sSkip_WS(fStrim);

					string attributeName;
					attributeName.reserve(8);
					if (!sReadMLAttributeName(fStrim, attributeName))
						break;

					sSkip_WS(fStrim);

					if (sTryRead_CP(fStrim, '='))
						{
						sSkip_WS(fStrim);
						string attributeValue;
						attributeValue.reserve(8);
						if (!sReadMLAttributeValue(
							fStrim, fRecognizeEntitiesInAttributeValues,
							fCallback, fRefcon, attributeValue))
							{
							fToken = eToken_Exhausted;
							return;
							}
						fTagAttributes.SetString(ZTName(attributeName), attributeValue);
						}
					else
						{
						fTagAttributes.SetNull(ZTName(attributeName));
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
#pragma mark * ZML parsing support

void ZML::sSkipText(ZML::Reader& r)
	{
	while (r.Current() == ZML::eToken_Text)
		r.Advance();
	}

bool ZML::sSkip(Reader& r, const string& iTagName)
	{
	vector<string> theTags(1, iTagName);
	return sSkip(r, theTags);
	}

bool ZML::sSkip(ZML::Reader& r, vector<string>& ioTags)
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

bool ZML::sTryRead_Begin(Reader& r, const string& iTagName)
	{
	if (r.Current() != ZML::eToken_TagBegin || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}

bool ZML::sTryRead_End(Reader& r, const string& iTagName)
	{
	if (r.Current() != ZML::eToken_TagEnd || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZML::StrimR

ZML::StrimR::StrimR(Reader& iReader)
:	fReader(iReader)
	{}

void ZML::StrimR::Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount)
	{
	UTF32* localDest = iDest;
	while (iCount)
		{
		if (fReader.Current() == ZML::eToken_Text)
			{
			size_t countRead;
			fReader.Text().Read(localDest, iCount, &countRead);
			if (countRead == 0)
				{
				fReader.Advance();
				}
			else
				{
				localDest += countRead;
				iCount -= countRead;
				}
			}
		else
			{
			switch (fReader.Current())
				{
				case ZML::eToken_TagBegin:
					{
					fTags.push_back(pair<string, ZTuple>(fReader.Name(), fReader.Attrs()));
					break;
					}
				case ZML::eToken_TagEnd:
					{
					if (!fTags.empty())
						fTags.pop_back();
					break;
					}
				case ZML::eToken_Exhausted:
					{
					iCount = 0;
					break;
					}
				default:
					break;
				}
			fReader.Advance();
			}
		}

	if (oCount)
		*oCount = localDest - iDest;
	}

string ZML::StrimR::BackName() const
	{
	ZAssert(!fTags.empty());
	return fTags.back().first;
	}

ZTuple ZML::StrimR::BackAttr() const
	{
	ZAssert(!fTags.empty());
	return fTags.back().second;
	}

void ZML::StrimR::AllNames(vector<string>& oNames) const
	{
	for (vector<pair<string, ZTuple> >::const_iterator i = fTags.begin(); i != fTags.end(); ++i)
		oNames.push_back((*i).first);
	}

void ZML::StrimR::AllAttrs(vector<ZTuple>& oAttrs) const
	{
	for (vector<pair<string, ZTuple> >::const_iterator i = fTags.begin(); i != fTags.end(); ++i)
		oAttrs.push_back((*i).second);
	}

const vector<pair<string, ZTuple> >& ZML::StrimR::All() const
	{
	return fTags;
	}

NAMESPACE_ZOOLIB_END
