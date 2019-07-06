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

#include "zoolib/ChanRU_UTF_ML.h"

#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_STL.h"
#include "zoolib/Util_STL_vector.h"
//#include "zoolib/Util_string.h"

namespace ZooLib {

#define kDebug_StrimW_ML 1

using std::min;
//using std::pair;
using std::string;
using std::vector;

using namespace ML;
using namespace Util_Chan;
using namespace Util_STL;

// =================================================================================================
#pragma mark - ChanRU_UTF_ML

ChanRU_UTF_ML::ChanRU_UTF_ML(const ZooLib::ChanRU_UTF& iChanRU)
:	fChanRU(iChanRU)
,	fRecognizeEntitiesInAttributeValues(false)
,	fBufferStart(0)
,	fToken(eToken_Fresh)
	{}

ChanRU_UTF_ML::ChanRU_UTF_ML(const ZooLib::ChanRU_UTF& iChanRU,
	bool iRecognizeEntitiesInAttributeValues, ZP<Callable_Entity> iCallable)
:	fChanRU(iChanRU)
,	fRecognizeEntitiesInAttributeValues(iRecognizeEntitiesInAttributeValues)
,	fCallable(iCallable)
,	fBufferStart(0)
,	fToken(eToken_Fresh)
	{}

ChanRU_UTF_ML::~ChanRU_UTF_ML()
	{}

size_t ChanRU_UTF_ML::Read(UTF32* oDest, size_t iCount)
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
				fBuffer = Unicode::sAsUTF32(ML::sReadReference(fChanRU, fCallable));
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

size_t ChanRU_UTF_ML::Unread(const UTF32* iSource, size_t iCount)
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

EToken ChanRU_UTF_ML::Current() const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	return fToken;
	}

ChanRU_UTF_ML& ChanRU_UTF_ML::Advance()
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

const string& ChanRU_UTF_ML::Name() const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	if (fToken == eToken_TagBegin || fToken == eToken_TagEnd || fToken == eToken_TagEmpty)
		return fTagName;

	return sDefault<string>();
	}

Attrs_t ChanRU_UTF_ML::Attrs() const
	{
	if (fToken == eToken_Fresh)
		this->pAdvance();

	if (fToken == eToken_TagBegin || fToken == eToken_TagEmpty)
		return fTagAttributes;

	return Attrs_t();
	}

ZQ<string> ChanRU_UTF_ML::QAttr(const string& iAttrName) const
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

string ChanRU_UTF_ML::Attr(const string& iAttrName) const
	{
	if (ZQ<string> theAttr = this->QAttr(iAttrName))
		return *theAttr;
	return string();
	}

void ChanRU_UTF_ML::pAdvance() const
	{ sNonConst(this)->pAdvance(); }

void ChanRU_UTF_ML::pAdvance()
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

				if (not sReadIdentifier(fChanRU, fTagName))
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
				else if (sRead_String("[CDATA[", fChanRU))
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

				if (not sReadIdentifier(fChanRU, fTagName))
					{
					fToken = eToken_Exhausted;
					return;
					}

				for (;;)
					{
					sSkip_WS(fChanRU);

					string attributeName;
					attributeName.reserve(8);
					if (not sReadAttributeName(fChanRU, attributeName))
						break;

					sSkip_WS(fChanRU);

					if (sTryRead_CP('=', fChanRU))
						{
						sSkip_WS(fChanRU);
						string attributeValue;
						attributeValue.reserve(8);
						if (not sReadAttributeValue(fChanRU,
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

ZP<ChannerRU_UTF_ML> sChannerRU_UTF_ML(const ZP<ChannerRU<UTF32>>& iChanner)
	{ return sChanner_Channer_T<ChanRU_UTF_ML>(iChanner); }

// =================================================================================================
#pragma mark - ML parsing support

void sSkipText(ChanRU_UTF_ML& r)
	{
	while (r.Current() == eToken_Text)
		r.Advance();
	}

bool sSkip(ChanRU_UTF_ML& r, const string& iTagName)
	{
	vector<string> theTags(1, iTagName);
	return sSkip(r, theTags);
	}

bool sSkip(ChanRU_UTF_ML& r, vector<string>& ioTags)
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

bool sTryRead_Begin(ChanRU_UTF_ML& r, const string& iTagName)
	{
	if (r.Current() != eToken_TagBegin || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}

bool sTryRead_Empty(ChanRU_UTF_ML& r, const string& iTagName)
	{
	if (r.Current() != eToken_TagEmpty || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}

bool sTryRead_End(ChanRU_UTF_ML& r, const string& iTagName)
	{
	if (r.Current() != eToken_TagEnd || r.Name() != iTagName)
		return false;

	r.Advance();
	return true;
	}

} // namespace ZooLib
