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

#include "zoolib/Chan_UTF_Escaped.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan.h"
#include "zoolib/Util_Chan_UTF.h"

#include "zoolib/ZDebug.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_UTF_Escaped

ChanR_UTF_Escaped::ChanR_UTF_Escaped(UTF32 iDelimiter,
	const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
:	fDelimiter(iDelimiter)
,	fChanR(iChanR)
,	fChanU(iChanU)
	{}

ChanR_UTF_Escaped::~ChanR_UTF_Escaped()
	{}

size_t ChanR_UTF_Escaped::QRead(UTF32* oDest, size_t iCountCU)
	{
	using namespace Util_Chan;

	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iCountCU;
	while (localDestEnd > localDest)
		{
		UTF32 theCP;
		if (not sQRead(theCP, fChanR))
			throw ParseException("Unexpected end of strim whilst parsing a string");

		if (theCP == fDelimiter)
			{
			sUnread(theCP, fChanU);
			break;
			}

		if (Unicode::sIsEOL(theCP))
			{
			sUnread(theCP, fChanU);
			throw ParseException("Illegal end of line whilst parsing a string");
			}

		if (theCP == '\\')
			{
			if (not sQRead(theCP, fChanR))
				throw ParseException("Unexpected end of strim whilst parsing a string");

			switch (theCP)
				{
				case '\\':
					theCP = '\\';
					break;
				case 't':
					theCP = '\t';
					break;
				case 'n':
					theCP = '\n';
					break;
				case 'r':
					theCP = '\r';
					break;
				case 'b':
					theCP = '\b';
					break;
				case 'f':
					theCP = '\f';
					break;
				case '"':
					theCP = '\"';
					break;
				case '\'':
					theCP = '\'';
					break;
				case '/':
					theCP = '/';
					break;
				case 'x':
					{
					if (NotQ<int> theQ = sQRead_HexDigit(fChanR, fChanU))
						{
						throw ParseException("Illegal non-hex digit following \"\\x\"");
						}
					else
						{
						theCP = *theQ;
						while (ZQ<int> theQ = sQRead_HexDigit(fChanR, fChanU))
							theCP = (theCP << 4) + *theQ;
						}
					break;
					}
				case 'u':
				case 'U':
					{
					int32 requiredChars = 4;
					if (theCP == 'U')
						requiredChars = 8;

					UTF32 resultCP = 0;
					while (requiredChars--)
						{
						if (NotQ<int> theQ = sQRead_HexDigit(fChanR, fChanU))
							{
							throw ParseException(string8("Illegal non-hex digit in \"\\")
								+ char(theCP) + "\" escape sequence");
							}
						else
							{
							resultCP = (resultCP << 4) + *theQ;
							}
						}
					theCP = resultCP;
					break;
					}
				default:
					{
					// Gotta love escape sequences. This message
					// has "\" (quote, backslash, quote) at the end.
					throw ParseException("Illegal character following \"\\\"");
					}
				}
			}

		*localDest++ = theCP;
		}

	return localDest - oDest;
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanW_UTF_Escaped::Options

ChanW_UTF_Escaped::Options::Options()
:	fQuoteQuotes(true)
,	fEscapeHighUnicode(true)
	{}

// =================================================================================================
#pragma mark -
#pragma mark ChanW_UTF_Escaped

ChanW_UTF_Escaped::ChanW_UTF_Escaped(const Options& iOptions, const ChanW_UTF& iStrimSink)
:	fStrimSink(iStrimSink),
	fEOL(iOptions.fEOL),
	fQuoteQuotes(iOptions.fQuoteQuotes),
	fEscapeHighUnicode(iOptions.fEscapeHighUnicode),
	fLastWasCR(false)
	{}

ChanW_UTF_Escaped::ChanW_UTF_Escaped(const ChanW_UTF& iStrimSink)
:	fStrimSink(iStrimSink),
	fQuoteQuotes(true),
	fEscapeHighUnicode(true),
	fLastWasCR(false)
	{}

ChanW_UTF_Escaped::~ChanW_UTF_Escaped()
	{
	try
		{
		if (fLastWasCR)
			{
			sEWrite(fStrimSink, "\\r");
			sEWrite(fStrimSink, fEOL);
			}
		}
	catch (...)
		{}
	}

static UTF32 spAsHexCP(int inInt)
	{
	if (inInt < 10)
		return inInt + '0';
	return inInt - 10 + 'A';
	}

size_t ChanW_UTF_Escaped::QWrite(const UTF32* iSource, size_t iCountCU)
	{
	size_t localCount = iCountCU + 1;
	while (--localCount)
		{
		UTF32 theCP = *iSource++;
		bool lastWasCR = fLastWasCR;
		fLastWasCR = false;
		if (theCP < 0x20 || theCP == 0x7F)
			{
			switch (theCP)
				{
				case '\t':
					{
					sEWrite(fStrimSink, "\\t");
					break;
					}
				case '\n':
					{
					if (lastWasCR)
						sEWrite(fStrimSink, "\\r");
					sEWrite(fStrimSink, "\\n");
					sEWrite(fStrimSink, fEOL);
					break;
					}
				case '\r':
					{
					if (lastWasCR)
						{
						sEWrite(fStrimSink, "\\r");
						sEWrite(fStrimSink, fEOL);
						}
					fLastWasCR = true;
					break;
					}
				case '\b':
					{
					sEWrite(fStrimSink, "\\b");
					break;
					}
				case '\f':
					{
					sEWrite(fStrimSink, "\\f");
					break;
					}
				default:
					{
					sEWrite(fStrimSink, "\\x");
					sEWrite(fStrimSink, spAsHexCP(theCP >> 4));
					sEWrite(fStrimSink, spAsHexCP(theCP & 0xF));
					break;
					}
				}
			}
		else if (theCP < 0x80 || !fEscapeHighUnicode)
			{
			if (fQuoteQuotes && theCP == '\"')
				sEWrite(fStrimSink, "\\\"");
			else if (not fQuoteQuotes && theCP == '\'')
				sEWrite(fStrimSink, "\\\'");
			else if (theCP == '\\')
				sEWrite(fStrimSink, "\\\\");
			else
				sEWrite(fStrimSink, theCP);
			}
		else if (theCP < 0x10000)
			{
			sEWrite(fStrimSink, "\\u");
			sEWrite(fStrimSink, spAsHexCP((theCP >> 12) & 0xF));
			sEWrite(fStrimSink, spAsHexCP((theCP >> 8) & 0xF));
			sEWrite(fStrimSink, spAsHexCP((theCP >> 4) & 0xF));
			sEWrite(fStrimSink, spAsHexCP(theCP & 0xF));
			}
		else
			{
			sEWrite(fStrimSink, "\\U");
			sEWrite(fStrimSink, spAsHexCP((theCP >> 28) & 0xF));
			sEWrite(fStrimSink, spAsHexCP((theCP >> 24) & 0xF));
			sEWrite(fStrimSink, spAsHexCP((theCP >> 20) & 0xF));
			sEWrite(fStrimSink, spAsHexCP((theCP >> 16) & 0xF));
			sEWrite(fStrimSink, spAsHexCP((theCP >> 12) & 0xF));
			sEWrite(fStrimSink, spAsHexCP((theCP >> 8) & 0xF));
			sEWrite(fStrimSink, spAsHexCP((theCP >> 4) & 0xF));
			sEWrite(fStrimSink, spAsHexCP(theCP & 0xF));
			}
		}
	return iCountCU - localCount;
	}

} // namespace ZooLib
