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

#include "zoolib/ZDebug.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_Strim.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStrimR_Escaped

ZStrimR_Escaped::ZStrimR_Escaped(const ZStrimU& iStrimSource, UTF32 iDelimiter)
:	fStrimSource(iStrimSource),
	fDelimiter(iDelimiter)
	{}

ZStrimR_Escaped::~ZStrimR_Escaped()
	{}

void ZStrimR_Escaped::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	using namespace ZUtil_Strim;

	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iCount;
	while (localDestEnd > localDest)
		{
		UTF32 theCP;
		if (not fStrimSource.ReadCP(theCP))
			throw ParseException("Unexpected end of strim whilst parsing a string");

		if (theCP == fDelimiter)
			{
			fStrimSource.Unread(theCP);
			break;
			}

		if (ZUnicode::sIsEOL(theCP))
			{
			fStrimSource.Unread(theCP);
			throw ParseException("Illegal end of line whilst parsing a string");
			}

		if (theCP == '\\')
			{
			if (not fStrimSource.ReadCP(theCP))
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
					int curDigit;
					if (not sTryRead_HexDigit(fStrimSource, curDigit))
						throw ParseException("Illegal non-hex digit following \"\\x\"");

					theCP = curDigit;

					if (sTryRead_HexDigit(fStrimSource, curDigit))
						theCP = (curDigit << 4) + curDigit;
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
						int curDigit;
						if (not sTryRead_HexDigit(fStrimSource, curDigit))
							{
							throw ParseException(string8("Illegal non-hex digit in \"\\")
								+ char(theCP) + "\" escape sequence");
							}

						resultCP = (resultCP << 4) + curDigit;
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

	if (oCount)
		*oCount = localDest - oDest;
	}

// =================================================================================================
// MARK: - ZStrimW_Escaped::Options

ZStrimW_Escaped::Options::Options()
:	fQuoteQuotes(true),
	fEscapeHighUnicode(true)
	{}

// =================================================================================================
// MARK: - ZStrimW_Escaped

ZStrimW_Escaped::ZStrimW_Escaped(const Options& iOptions, const ChanW_UTF& iStrimSink)
:	fStrimSink(iStrimSink),
	fEOL(iOptions.fEOL),
	fQuoteQuotes(iOptions.fQuoteQuotes),
	fEscapeHighUnicode(iOptions.fEscapeHighUnicode),
	fLastWasCR(false)
	{}

ZStrimW_Escaped::ZStrimW_Escaped(const ChanW_UTF& iStrimSink)
:	fStrimSink(iStrimSink),
	fQuoteQuotes(true),
	fEscapeHighUnicode(true),
	fLastWasCR(false)
	{}

ZStrimW_Escaped::~ZStrimW_Escaped()
	{
	try
		{
		if (fLastWasCR)
			{
			sWrite("\\r", fStrimSink);
			sWrite(fEOL, fStrimSink);
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

size_t ZStrimW_Escaped::Write(const UTF32* iSource, size_t iCountCU)
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
					sWrite("\\t", fStrimSink);
					break;
					}
				case '\n':
					{
					if (lastWasCR)
						sWrite("\\r", fStrimSink);
					sWrite("\\n", fStrimSink);
					sWrite(fEOL, fStrimSink);
					break;
					}
				case '\r':
					{
					if (lastWasCR)
						{
						sWrite("\\r", fStrimSink);
						sWrite(fEOL, fStrimSink);
						}
					fLastWasCR = true;
					break;
					}
				case '\b':
					{
					sWrite("\\b", fStrimSink);
					break;
					}
				case '\f':
					{
					sWrite("\\f", fStrimSink);
					break;
					}
				default:
					{
					sWrite("\\x", fStrimSink);
					sWrite(spAsHexCP(theCP >> 4), fStrimSink);
					sWrite(spAsHexCP(theCP & 0xF), fStrimSink);
					break;
					}
				}
			}
		else if (theCP < 0x80 || !fEscapeHighUnicode)
			{
			if (fQuoteQuotes && theCP == '\"')
				sWrite("\\\"", fStrimSink);
			else if (not fQuoteQuotes && theCP == '\'')
				sWrite("\\\'", fStrimSink);
			else if (theCP == '\\')
				sWrite("\\\\", fStrimSink);
			else
				sWrite(theCP, fStrimSink);
			}
		else if (theCP < 0x10000)
			{
			sWrite("\\u", fStrimSink);
			sWrite(spAsHexCP((theCP >> 12) & 0xF), fStrimSink);
			sWrite(spAsHexCP((theCP >> 8) & 0xF), fStrimSink);
			sWrite(spAsHexCP((theCP >> 4) & 0xF), fStrimSink);
			sWrite(spAsHexCP(theCP & 0xF), fStrimSink);
			}
		else
			{
			sWrite("\\U", fStrimSink);
			sWrite(spAsHexCP((theCP >> 28) & 0xF), fStrimSink);
			sWrite(spAsHexCP((theCP >> 24) & 0xF), fStrimSink);
			sWrite(spAsHexCP((theCP >> 20) & 0xF), fStrimSink);
			sWrite(spAsHexCP((theCP >> 16) & 0xF), fStrimSink);
			sWrite(spAsHexCP((theCP >> 12) & 0xF), fStrimSink);
			sWrite(spAsHexCP((theCP >> 8) & 0xF), fStrimSink);
			sWrite(spAsHexCP((theCP >> 4) & 0xF), fStrimSink);
			sWrite(spAsHexCP(theCP & 0xF), fStrimSink);
			}
		}
	return iCountCU - localCount;
	}

} // namespace ZooLib
