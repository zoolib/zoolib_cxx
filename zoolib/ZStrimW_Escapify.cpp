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

#include "zoolib/ZStrimW_Escapify.h"

NAMESPACE_ZOOLIB_USING

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_Escapify::Options

ZStrimW_Escapify::Options::Options()
:	fQuoteQuotes(true),
	fEscapeHighUnicode(true)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_Escapify

ZStrimW_Escapify::ZStrimW_Escapify(const Options& iOptions, const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fEOL(iOptions.fEOL),
	fQuoteQuotes(iOptions.fQuoteQuotes),
	fEscapeHighUnicode(iOptions.fEscapeHighUnicode),
	fLastWasCR(false)
	{}

ZStrimW_Escapify::ZStrimW_Escapify(const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fQuoteQuotes(true),
	fEscapeHighUnicode(true),
	fLastWasCR(false)
	{}

ZStrimW_Escapify::~ZStrimW_Escapify()
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

static UTF32 sAsHexCP(int inInt)
	{
	if (inInt < 10)
		return inInt + '0';
	return inInt - 10 + 'A';
	}

void ZStrimW_Escapify::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
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
					fStrimSink.WriteCP(sAsHexCP(theCP >> 4));
					fStrimSink.WriteCP(sAsHexCP(theCP & 0xF));
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
			fStrimSink.WriteCP(sAsHexCP((theCP >> 12) & 0xF));
			fStrimSink.WriteCP(sAsHexCP((theCP >> 8) & 0xF));
			fStrimSink.WriteCP(sAsHexCP((theCP >> 4) & 0xF));
			fStrimSink.WriteCP(sAsHexCP(theCP & 0xF));
			}
		else
			{
			fStrimSink.Write("\\U");
			fStrimSink.WriteCP(sAsHexCP((theCP >> 28) & 0xF));
			fStrimSink.WriteCP(sAsHexCP((theCP >> 24) & 0xF));
			fStrimSink.WriteCP(sAsHexCP((theCP >> 20) & 0xF));
			fStrimSink.WriteCP(sAsHexCP((theCP >> 16) & 0xF));
			fStrimSink.WriteCP(sAsHexCP((theCP >> 12) & 0xF));
			fStrimSink.WriteCP(sAsHexCP((theCP >> 8) & 0xF));
			fStrimSink.WriteCP(sAsHexCP((theCP >> 4) & 0xF));
			fStrimSink.WriteCP(sAsHexCP(theCP & 0xF));
			}
		}
	if (oCountCU)
		*oCountCU = iCountCU - localCount;
	}

