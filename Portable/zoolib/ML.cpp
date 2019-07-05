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

#include "zoolib/ML.h"

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
namespace ML {

#define kDebug_StrimW_ML 1

using std::min;
using std::pair;
using std::string;
using std::vector;

using namespace Util_Chan;
using namespace Util_STL;

// =================================================================================================
#pragma mark - Static helper functions

static string spReadReference(const ZooLib::ChanRU_UTF& iChanRU, ZP<Callable_Entity> iCallable)
	{
	string result;

	if (sTryRead_CP('#', iChanRU))
		{
		// It's a character reference.
		int64 theInt;
		bool gotIt = false;
		if (sTryRead_CP('x', iChanRU) || sTryRead_CP('X', iChanRU))
			gotIt = sTryRead_HexInteger(iChanRU, theInt);
		else
			gotIt = sTryRead_DecimalInteger(iChanRU, theInt);

		if (gotIt && sTryRead_CP(';', iChanRU))
			result += UTF32(theInt);
		}
	else
		{
		string theEntity;
		theEntity.reserve(8);
		for (;;)
			{
			if (NotQ<UTF32> theCPQ = sQRead(iChanRU))
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

static bool spReadMLIdentifier(const ZooLib::ChanRU_UTF& iChanRU, string& oText)
	{
	oText.resize(0);

	if (NotQ<UTF32> theCPQ = sQRead(iChanRU))
		{ return false; }
	else if (not Unicode::sIsAlpha(*theCPQ) && *theCPQ != '_' && *theCPQ != '?' && *theCPQ != '!')
		{
		sUnread(iChanRU, *theCPQ);
		return false;
		}
	else
		{
		oText += *theCPQ;

		for (;;)
			{
			if (NotQ<UTF32> theCPQ = sQRead(iChanRU))
				{
				break;
				}
			else if (not Unicode::sIsAlphaDigit(*theCPQ)
				&& *theCPQ != '_' && *theCPQ != '-' && *theCPQ != ':')
				{
				sUnread(iChanRU, *theCPQ);
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

static bool spReadUntil(const ZooLib::ChanRU_UTF& iChanRU, UTF32 iTerminator, string& oText)
	{
	oText.resize(0);

	for (;;)
		{
		if (NotQ<UTF32> theCPQ = sQRead(iChanRU))
			return false;
		else if (*theCPQ == iTerminator)
			return true;
		else
			oText += *theCPQ;
		}
	}

static bool spReadMLAttributeName(const ZooLib::ChanRU_UTF& iChanRU, string& oName)
	{
	oName.resize(0);

	if (NotQ<UTF32> theCPQ = sQRead(iChanRU))
		{ return false; }
	else if (*theCPQ == '"')
		{ return spReadUntil(iChanRU, '"', oName); }
	else if (*theCPQ == '\'')
		{ return spReadUntil(iChanRU, '\'', oName); }
	else
		{
		if (not Unicode::sIsAlpha(*theCPQ) && *theCPQ != '_' && *theCPQ != '?' && *theCPQ != '!')
			{
			sUnread(iChanRU, *theCPQ);
			return false;
			}

		oName += *theCPQ;
		for (;;)
			{
			if (NotQ<UTF32> theCPQ = sQRead(iChanRU))
				{ return true; }
			else if (not Unicode::sIsAlphaDigit(*theCPQ)
				&& *theCPQ != '_' && *theCPQ != '-' && *theCPQ != ':')
				{
				sUnread(iChanRU, *theCPQ);
				return true;
				}
			else
				{ oName += *theCPQ; }
			}
		}
	}

static bool spReadMLAttributeValue(
	const ZooLib::ChanRU_UTF& iChanRU,
	bool iRecognizeEntities, ZP<Callable_Entity> iCallable,
	string& oValue)
	{
	oValue.resize(0);

	if (NotQ<UTF32> theCPQ = sQRead(iChanRU))
		{ return false; }
	else if (*theCPQ == '"')
		{ return spReadUntil(iChanRU, '"', oValue); }
	else if (*theCPQ == '\'')
		{ return spReadUntil(iChanRU, '\'', oValue); }
	else
		{
		sUnread(iChanRU, *theCPQ);

		sSkip_WS(iChanRU);

		for (;;)
			{
			if (NotQ<UTF32> theCPQ = sQRead(iChanRU))
				{ break; }
			else if (*theCPQ == '>')
				{
				sUnread(iChanRU, *theCPQ);
				break;
				}
			else if (Unicode::sIsWhitespace(*theCPQ))
				{ break; }
			else if (*theCPQ == '&' && iRecognizeEntities)
				{ oValue += spReadReference(iChanRU, iCallable); }
			else
				{ oValue += *theCPQ; }
			}
		}

	return true;
	}

// =================================================================================================
#pragma mark - ML::ChanRU

ChanRU::ChanRU(const ZooLib::ChanRU_UTF& iChanRU)
:	fChanRU(iChanRU)
,	fRecognizeEntitiesInAttributeValues(false)
,	fBufferStart(0)
,	fToken(eToken_Fresh)
	{}

ChanRU::ChanRU(const ZooLib::ChanRU_UTF& iChanRU,
	bool iRecognizeEntitiesInAttributeValues, ZP<Callable_Entity> iCallable)
:	fChanRU(iChanRU)
,	fRecognizeEntitiesInAttributeValues(iRecognizeEntitiesInAttributeValues)
,	fCallable(iCallable)
,	fBufferStart(0)
,	fToken(eToken_Fresh)
	{}

ChanRU::~ChanRU()
	{}

size_t ChanRU::Read(UTF32* oDest, size_t iCount)
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
			else if (NotQ<UTF32> theCPQ = sQRead(fChanRU))
				{
				fToken = eToken_Exhausted;
				break;
				}
			else if (*theCPQ == '<')
				{
				sUnread(fChanRU, *theCPQ);
				break;
				}
			else if (*theCPQ == '&')
				{
				fBufferStart = 0;
				fBuffer = Unicode::sAsUTF32(spReadReference(fChanRU, fCallable));
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

size_t ChanRU::Unread(const UTF32* iSource, size_t iCount)
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

EToken ChanRU::Current() const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	return fToken;
	}

ChanRU& ChanRU::Advance()
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

const string& ChanRU::Name() const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	if (fToken == eToken_TagBegin || fToken == eToken_TagEnd || fToken == eToken_TagEmpty)
		return fTagName;

	return sDefault<string>();
	}

Attrs_t ChanRU::Attrs() const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	if (fToken == eToken_TagBegin || fToken == eToken_TagEmpty)
		return fTagAttributes;

	return Attrs_t();
	}

ZQ<string> ChanRU::QAttr(const string& iAttrName) const
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

string ChanRU::Attr(const string& iAttrName) const
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

void ChanRU::pAdvance() const
	{ const_cast<ChanRU*>(this)->pAdvance(); }

void ChanRU::pAdvance()
	{
	fTagAttributes.clear();

	for	(;;)
		{
		if (NotQ<UTF32> theCPQ = sQRead(fChanRU))
			{
			fToken = eToken_Exhausted;
			return;
			}
		else if (*theCPQ != '<')
			{
			sUnread(fChanRU, *theCPQ);
			fToken = eToken_Text;
			return;
			}

		sSkip_WS(fChanRU);

		if (NotQ<UTF32> theCPQ = sQRead(fChanRU))
			{
			fToken = eToken_Exhausted;
			return;
			}
		else switch (*theCPQ)
			{
			case'/':
				{
				sSkip_WS(fChanRU);

				if (not spReadMLIdentifier(fChanRU, fTagName))
					{
					fToken = eToken_Exhausted;
					return;
					}

				sSkip_WS(fChanRU);

				if (not sTryRead_CP('>', fChanRU))
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
				sSkip_Until(fChanRU, "?>");
				break;
				}
			case '!':
				{
				if (sTryRead_CP('-', fChanRU))
					{
					if (sTryRead_CP('-', fChanRU))
						{
						// A comment.
						sSkip_Until(fChanRU, "-->");
						}
					else
						{
						// Not a comment, but not an entity definition. Just skip
						// till we hit a '>'
						sSkip_Until(fChanRU, ">");
						}
					}
				else if (spTryRead_String("[CDATA[", fChanRU))
					{
					// CDATA
					sSkip_Until(fChanRU, "]]>");
					}
				else
					{
					// An entity definition
					sSkip_Until(fChanRU, ">");
					}
				break;
				}
			default:
				{
				sUnread(fChanRU, *theCPQ);

				if (not spReadMLIdentifier(fChanRU, fTagName))
					{
					fToken = eToken_Exhausted;
					return;
					}

				for (;;)
					{
					sSkip_WS(fChanRU);

					string attributeName;
					attributeName.reserve(8);
					if (not spReadMLAttributeName(fChanRU, attributeName))
						break;

					sSkip_WS(fChanRU);

					if (sTryRead_CP('=', fChanRU))
						{
						sSkip_WS(fChanRU);
						string attributeValue;
						attributeValue.reserve(8);
						if (not spReadMLAttributeValue(fChanRU,
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

				sSkip_WS(fChanRU);

				if (sTryRead_CP('/', fChanRU))
					fToken = eToken_TagEmpty;
				else
					fToken = eToken_TagBegin;

				if (not sTryRead_CP('>', fChanRU))
					fToken = eToken_Exhausted;

				return;
				}
			}
		}
	}

ZP<ChannerRU> sChanner(const ZP<ZooLib::ChannerRU<UTF32>>& iChanner)
	{ return sChanner_Channer_T<ChanRU>(iChanner); }

// =================================================================================================
#pragma mark - ML parsing support

void sSkipText(ChanRU& r)
	{
	while (r.Current() == eToken_Text)
		r.Advance();
	}

bool sSkip(ChanRU& r, const string& iTagName)
	{
	vector<string> theTags(1, iTagName);
	return sSkip(r, theTags);
	}

bool sSkip(ChanRU& r, vector<string>& ioTags)
	{
	while (not ioTags.empty())
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

bool sTryRead_Begin(ChanRU& r, const string& iTagName)
	{
	if (r.Current() != eToken_TagBegin || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}

bool sTryRead_Empty(ChanRU& r, const string& iTagName)
	{
	if (r.Current() != eToken_TagEmpty || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}

bool sTryRead_End(ChanRU& r, const string& iTagName)
	{
	if (r.Current() != eToken_TagEnd || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}


} // namespace ML
} // namespace ZooLib
