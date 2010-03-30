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

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_Escaped

ZStrimR_Escaped::ZStrimR_Escaped(const ZStrimU& iStrimSource, UTF32 iDelimiter)
:	fStrimSource(iStrimSource),
	fDelimiter(iDelimiter)
	{}

ZStrimR_Escaped::~ZStrimR_Escaped()
	{}

void ZStrimR_Escaped::Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount)
	{
	using namespace ZUtil_Strim;

	UTF32* localDest = iDest;
	UTF32* localDestEnd = iDest + iCount;
	while (localDestEnd > localDest)
		{
		UTF32 theCP;
		if (!fStrimSource.ReadCP(theCP))
			throw ParseException("Unexpected end of strim whilst parsing a string");

		if (theCP == fDelimiter)
			{
			fStrimSource.Unread(theCP);
			break;
			}

		if (ZUnicode::sIsEOL(theCP))
			throw ParseException("Illegal end of line whilst parsing a string");

		if (theCP == '\\')
			{
			if (!fStrimSource.ReadCP(theCP))
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
					if (!sTryRead_HexDigit(fStrimSource, curDigit))
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
						if (!sTryRead_HexDigit(fStrimSource, curDigit))
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
		*oCount = localDest - iDest;	
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_Escaped::Options

ZStrimW_Escaped::Options::Options()
:	fQuoteQuotes(true),
	fEscapeHighUnicode(true)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_Escaped

ZStrimW_Escaped::ZStrimW_Escaped(const Options& iOptions, const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fEOL(iOptions.fEOL),
	fQuoteQuotes(iOptions.fQuoteQuotes),
	fEscapeHighUnicode(iOptions.fEscapeHighUnicode),
	fLastWasCR(false)
	{}

ZStrimW_Escaped::ZStrimW_Escaped(const ZStrimW& iStrimSink)
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
			fStrimSink.Write("\\r");
			fStrimSink.Write(fEOL);
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

void ZStrimW_Escaped::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
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
					fStrimSink.Write("\\t");
					break;
					}
				case '\n':
					{
					if (lastWasCR)
						fStrimSink.Write("\\r");
					fStrimSink.Write("\\n");
					fStrimSink.Write(fEOL);
					break;
					}
				case '\r':
					{
					if (lastWasCR)
						{
						fStrimSink.Write("\\r");
						fStrimSink.Write(fEOL);
						}
					fLastWasCR = true;
					break;
					}
				case '\b':
					{
					fStrimSink.Write("\\b");
					break;
					}
				case '\f':
					{
					fStrimSink.Write("\\f");
					break;
					}
				default:
					{
					fStrimSink.Write("\\x");
					fStrimSink.WriteCP(spAsHexCP(theCP >> 4));
					fStrimSink.WriteCP(spAsHexCP(theCP & 0xF));
					break;
					}
				}
			}
		else if (theCP < 0x80 || !fEscapeHighUnicode)
			{
			if (fQuoteQuotes && theCP == '\"')
				fStrimSink.Write("\\\"");
			else if (!fQuoteQuotes && theCP == '\'')
				fStrimSink.Write("\\\'");
			else if (theCP == '\\')
				fStrimSink.Write("\\\\");
			else
				fStrimSink.WriteCP(theCP);
			}
		else if (theCP < 0x10000)
			{
			fStrimSink.Write("\\u");
			fStrimSink.WriteCP(spAsHexCP((theCP >> 12) & 0xF));
			fStrimSink.WriteCP(spAsHexCP((theCP >> 8) & 0xF));
			fStrimSink.WriteCP(spAsHexCP((theCP >> 4) & 0xF));
			fStrimSink.WriteCP(spAsHexCP(theCP & 0xF));
			}
		else
			{
			fStrimSink.Write("\\U");
			fStrimSink.WriteCP(spAsHexCP((theCP >> 28) & 0xF));
			fStrimSink.WriteCP(spAsHexCP((theCP >> 24) & 0xF));
			fStrimSink.WriteCP(spAsHexCP((theCP >> 20) & 0xF));
			fStrimSink.WriteCP(spAsHexCP((theCP >> 16) & 0xF));
			fStrimSink.WriteCP(spAsHexCP((theCP >> 12) & 0xF));
			fStrimSink.WriteCP(spAsHexCP((theCP >> 8) & 0xF));
			fStrimSink.WriteCP(spAsHexCP((theCP >> 4) & 0xF));
			fStrimSink.WriteCP(spAsHexCP(theCP & 0xF));
			}
		}
	if (oCountCU)
		*oCountCU = iCountCU - localCount;
	}

NAMESPACE_ZOOLIB_END
