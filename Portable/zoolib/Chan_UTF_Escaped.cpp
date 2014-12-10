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
// MARK: - ChanR_UTF_Escaped

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
		ZQ<UTF32> theCPQ = sQRead(fChanR);
		if (not theCPQ)
			throw ParseException("Unexpected end of strim whilst parsing a string");
		UTF32 theCP = *theCPQ;

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
			ZQ<UTF32> theCPQ = sQRead(fChanR);
			if (not theCPQ)
				throw ParseException("Unexpected end of strim whilst parsing a string");

			UTF32 theCP = *theCPQ;
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
					if (ZQ<int,false> theQ = sQRead_HexDigit(fChanR, fChanU))
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
						if (ZQ<int,false> theQ = sQRead_HexDigit(fChanR, fChanU))
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
// MARK: - ChanW_UTF_Escaped::Options

ChanW_UTF_Escaped::Options::Options()
:	fQuoteQuotes(true)
,	fEscapeHighUnicode(true)
	{}

// =================================================================================================
// MARK: - ChanW_UTF_Escaped

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
			sWriteMust("\\r", fStrimSink);
			sWriteMust(fEOL, fStrimSink);
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
					sWriteMust("\\t", fStrimSink);
					break;
					}
				case '\n':
					{
					if (lastWasCR)
						sWriteMust("\\r", fStrimSink);
					sWriteMust("\\n", fStrimSink);
					sWriteMust(fEOL, fStrimSink);
					break;
					}
				case '\r':
					{
					if (lastWasCR)
						{
						sWriteMust("\\r", fStrimSink);
						sWriteMust(fEOL, fStrimSink);
						}
					fLastWasCR = true;
					break;
					}
				case '\b':
					{
					sWriteMust("\\b", fStrimSink);
					break;
					}
				case '\f':
					{
					sWriteMust("\\f", fStrimSink);
					break;
					}
				default:
					{
					sWriteMust("\\x", fStrimSink);
					sWriteMust(spAsHexCP(theCP >> 4), fStrimSink);
					sWriteMust(spAsHexCP(theCP & 0xF), fStrimSink);
					break;
					}
				}
			}
		else if (theCP < 0x80 || !fEscapeHighUnicode)
			{
			if (fQuoteQuotes && theCP == '\"')
				sWriteMust("\\\"", fStrimSink);
			else if (not fQuoteQuotes && theCP == '\'')
				sWriteMust("\\\'", fStrimSink);
			else if (theCP == '\\')
				sWriteMust("\\\\", fStrimSink);
			else
				sWriteMust(theCP, fStrimSink);
			}
		else if (theCP < 0x10000)
			{
			sWriteMust("\\u", fStrimSink);
			sWriteMust(spAsHexCP((theCP >> 12) & 0xF), fStrimSink);
			sWriteMust(spAsHexCP((theCP >> 8) & 0xF), fStrimSink);
			sWriteMust(spAsHexCP((theCP >> 4) & 0xF), fStrimSink);
			sWriteMust(spAsHexCP(theCP & 0xF), fStrimSink);
			}
		else
			{
			sWriteMust("\\U", fStrimSink);
			sWriteMust(spAsHexCP((theCP >> 28) & 0xF), fStrimSink);
			sWriteMust(spAsHexCP((theCP >> 24) & 0xF), fStrimSink);
			sWriteMust(spAsHexCP((theCP >> 20) & 0xF), fStrimSink);
			sWriteMust(spAsHexCP((theCP >> 16) & 0xF), fStrimSink);
			sWriteMust(spAsHexCP((theCP >> 12) & 0xF), fStrimSink);
			sWriteMust(spAsHexCP((theCP >> 8) & 0xF), fStrimSink);
			sWriteMust(spAsHexCP((theCP >> 4) & 0xF), fStrimSink);
			sWriteMust(spAsHexCP(theCP & 0xF), fStrimSink);
			}
		}
	return iCountCU - localCount;
	}

} // namespace ZooLib
