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

#include "zoolib/ZTextCoder_Win.h"

#if ZCONFIG_API_Enabled(TextCoder_Win)

#include "zoolib/ZFactoryChain.h"
#include "zoolib/ZThread.h"

#include <map>
#include <stdexcept>

using std::map;
using std::min;
using std::runtime_error;
using std::string;
using std::vector;

NAMESPACE_ZOOLIB_BEGIN

ZAssertCompile(sizeof(WCHAR) == sizeof(UTF16));

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
#pragma mark -
#pragma mark * Factory functions

namespace ZANONYMOUS {

bool sMake_Decoder(ZTextDecoder*& oResult, const string& iParam)
	{
	try
		{
		oResult = new ZTextDecoder_Win(iParam);
		return true;		
		}
	catch (...)
		{}
	return false;
	}

ZFactoryChain_Maker_T<ZTextDecoder*, const string&>
	sMaker_Decoder(sMake_Decoder);

bool sMake_Encoder(ZTextEncoder*& oResult, const string& iParam)
	{
	try
		{
		oResult = new ZTextEncoder_Win(iParam);
		return true;
		}
	catch (...)
		{}
	return false;
	}

ZFactoryChain_Maker_T<ZTextEncoder*, const string&>
	sMaker_Encoder(sMake_Encoder);

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

namespace ZANONYMOUS {
struct NameToCodePage_t
	{
	const char* fName;
	UINT fCodePage;
	int fMIBEnum; // The MIB is not used, it's just for informational purposes.
	};
} // anonymous namespace

static NameToCodePage_t sNameToCodePage[] =
	{
		{ "iso_8859-1", 28591, 4 },
		{ "iso_8859-2", 28592, 5 },
		{ "iso_8859-3", 28593, 6 },
		{ "iso_8859-4", 28594, 7 },
		{ "iso_8859-5", 28595, 8 },
		{ "iso_8859-6", 28596, 9 },
		{ "iso_8859-7", 28597, 10 },
		{ "iso_8859-8", 28598, 11 },
		{ "iso_8859-9", 28599, 12 },
		{ "iso_8859-15", 28605, 111 },

		{ "windows-1250", 1250, 2250 }, // ANSI - Central European
		{ "windows-1251", 1251, 2251 }, // ANSI - Cyrillic
		{ "windows-1252", 1252, 2252 }, // ANSI - Latin I
		{ "windows-1253", 1253, 2253 }, // ANSI - Greek
		{ "windows-1254", 1254, 2254 }, // ANSI - Turkish
		{ "windows-1255", 1255, 2255 }, // ANSI - Hebrew
		{ "windows-1256", 1256, 2256 }, // ANSI - Arabic
		{ "windows-1257", 1257, 2257 }, // ANSI - Baltic
		{ "windows-1258", 1258, 2258 }, // ANSI/OEM - Vietnamese

		{ "gbk", 936, 113 },			// Simplified Chinese
		{ "gb2312", 20936, 2025 },		// Simplified Chinese
		{ "hz-gb-2312", 52936, 2085 },	// Simplified Chinese
		{ "gb18030", 54936, 114 },		// 4 Byte Simplified Chinese

		{ "big5", 10002, 2026 },		// Traditional Chinese

		{ "shift_jis", 932, 17 },	// ANSI/OEM - Japanese, Shift-JIS

		{ "euc-cn", 51936, -1 }, // EUC - Simplified Chinese
		{ "euc-tw", 51950, -1 }, // EUC - Traditional Chinese
		{ "euc-jp", 51932, 18 }, // EUC - Japanese
		{ "euc-kr", 51949, 38 }, // EUC - Korean

		{ "johab", 1361, -1 },				// Korean
		{ "ks_c_5601-1987", 10003, 2026 },	// Korean

		{ "koi8-r", 20866, 2084 },	// Russian
		{ "koi8-u", 21866, 2088 },	// Ukrainian

		{ "tscii", 57004, -1 },		// Tamil

		{ "ibm037", 37, 2028 },		// EBCDIC
		{ "ibm437", 437, 2011 },	// OEM - United States
		{ "ibm500", 500, 2044 },	// IBM EBCDIC - International
		{ "ibm737", 737, -1 },		// OEM - Greek (formerly 437G)
		{ "ibm775", 775, 2087 },	// OEM - Baltic
		{ "ibm850", 8850, 2009 },	// OEM - Multilingual Latin I
		{ "ibm852", 852, 2010 },	// OEM - Latin II
		{ "ibm855", 855, 2046 },	// OEM - Cyrillic (primarily Russian)
		{ "ibm857", 857, 2047 },	// OEM - Turkish
		{ "ibm860", 860, 2048 },	// OEM - Portuguese
		{ "ibm861", 861, 2049 },	// OEM - Icelandic
		{ "ibm862", 862, 2013 },	// OEM - Hebrew 
		{ "ibm863", 863, 2050 },	// OEM - Canadian-French
		{ "ibm864", 864, 2051 },	// OEM - Arabic
		{ "ibm865", 865, 2052 },	// OEM - Nordic
		{ "ibm866", 866, 2086 },	// OEM - Russian
		{ "ibm869", 869, 2054 },	// OEM - Modern Greek
		{ "ibm870", 870, 2055 },	// IBM EBCDIC - Multilingual/ROECE (Latin-2) 
//		{ "ibm874", 874, -1 },		// ANSI/OEM - Thai (same as CP28605, ISO 8859-15) <--- Not true?
		{ "ibm949", 949, -1 },		// ANSI/OEM - Korean (Unified Hangeul Code)
		{ "ibm950", 950, -1 },		// ANSI/OEM -  Traditional Chinese (Taiwan; Hong Kong SAR, PRC)

		{ "macintosh", 10000, 2027 },		// MAC - Roman
		{ "x-mac-japanese", 10001, -1 },
		{ "x-mac-arabic", 10004, -1 },
		{ "x-mac-hebrew", 10005, -1 },
		{ "x-mac-greek", 10006, -1 },
		{ "x-mac-cyrillic", 10007, -1 },
		{ "x-mac-romanian", 10010, -1 },
		{ "x-mac-ukrainian", 10017, -1 },
		{ "x-mac-thai", 10021, -1 },
		{ "x-mac-icelandic", 10079, -1 },
	};


static string sCanonicalName(const string& iName)
	{
	string result;
	bool seenNonZeroDigit = false;
	for (string::const_iterator i = iName.begin(); i != iName.end(); ++i)
		{
		char theChar = tolower(*i);
		switch (theChar)
			{
			case ' ':
			case '_':
			case '-':
				{
				// Strip spaces, underscores and hyphens.
				break;
				}
			default:
				{
				if (isdigit(theChar))
					{
					// Strip zeroes not preceded by a non-zero digit.
					if (theChar == '0')
						{
						if (seenNonZeroDigit)
							result += theChar;
						}
					else
						{
						result += theChar;
						seenNonZeroDigit = true;
						}
					}
				else
					{
					seenNonZeroDigit = false;
					result += theChar;
					}
				break;
				}
			}
		}
	return result;
	}

static bool sBuilt_NameToCodePage = false;
ZMutexNR sMutex_NameToCodePage;
static map<string, UINT> sMap_NameToCodePage;

static UINT sLookupName(const string& iName)
	{
	if (!sBuilt_NameToCodePage)
		{
		sMutex_NameToCodePage.Acquire();
		if (!sBuilt_NameToCodePage)
			{
			for (size_t x = 0; x < countof(sNameToCodePage); ++x)
				{
				sMap_NameToCodePage[sCanonicalName(sNameToCodePage[x].fName)]
					= sNameToCodePage[x].fCodePage;
				}
			sBuilt_NameToCodePage = true;
			}
		sMutex_NameToCodePage.Release();
		}

	string canonicalName = sCanonicalName(iName);
	map<string, UINT>::iterator theIter = sMap_NameToCodePage.find(canonicalName);
	if (theIter != sMap_NameToCodePage.end())
		return (*theIter).second;

	// Work our way through any aliases ZTextCoder may know about
	vector<string> aliases;
	ZTextCoder::sGetAliases(iName, aliases);
	for (vector<string>::iterator i = aliases.begin(); i != aliases.end(); ++i)
		{
		theIter = sMap_NameToCodePage.find(*i);
		if (theIter != sMap_NameToCodePage.end())
			return (*theIter).second;
		}
	return 0;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTextDecoder_Win

ZTextDecoder_Win::ZTextDecoder_Win(const char* iName)
	{
	this->Init(sLookupName(iName));
	}

ZTextDecoder_Win::ZTextDecoder_Win(const string& iName)
	{
	this->Init(sLookupName(iName));
	}

ZTextDecoder_Win::ZTextDecoder_Win(UINT iSourceCodePage)
	{
	this->Init(iSourceCodePage);
	}

ZTextDecoder_Win::~ZTextDecoder_Win()
	{}

static void sMBToWC(UINT iSourceCodePage, const CHAR* iSource, size_t iSourceCU, size_t& oSourceCU,
			WCHAR* iDest, size_t iDestCU, size_t& oDestCU)
	{
	ZAssertStop(1, iSourceCU && iDestCU);

	if (iSourceCU >= 2 * iDestCU)
		{
		// We've got at least twice as many source bytes as space for destination code units.
		// The odds are higher that we'd overflow the destination buffer. So discover how
		// much space we would need.
		for (;;)
			{
			int result = ::MultiByteToWideChar(iSourceCodePage, 0, iSource, iSourceCU, iDest, 0);
			if (result <= iDestCU)
				break;
			// There is insufficient space. Scale the source count to match. This is of course
			// only approximate, but will get us in the right ballpark immediately.
			iSourceCU = size_t(iSourceCU * (size_t(double(iDestCU) / double(result))));
			}
		}

	for (;;)
		{
		if (int result = ::MultiByteToWideChar(
			iSourceCodePage, 0, iSource, iSourceCU, iDest, iDestCU))
			{
			oSourceCU = iSourceCU;
			oDestCU = result;
			break;
			}
		else
			{
			DWORD err = ::GetLastError();
			if (err == ERROR_INSUFFICIENT_BUFFER)
				{
				// We don't have enough space, and don't know how much we need.
				// Just halve the source amount.
				iSourceCU /= 2;
				ZAssertStopf(0, iSourceCU, ("iSourceCU went to zero."));
				}
			else
				{
				ZDebugStopf(0, ("MultiByteToWideChar returned an unexpected error"));
				}
			}
		}
	}

static void sMBToWC_CanFail(
	UINT iSourceCodePage, const CHAR* iSource, size_t iSourceCU, size_t& oSourceCU,
	WCHAR* iDest, size_t iDestCU, size_t& oDestCU)
	{
	ZAssertStop(1, iSourceCU && iDestCU);

	DWORD theFlags = MB_ERR_INVALID_CHARS;
	for (;;)
		{
		if (int result = ::MultiByteToWideChar(
			iSourceCodePage, theFlags, iSource, iSourceCU, iDest, iDestCU))
			{
			oSourceCU = iSourceCU;
			oDestCU = result;
			break;
			}
		else
			{
			DWORD err = ::GetLastError();
			if (err == ERROR_INSUFFICIENT_BUFFER || err == ERROR_NO_UNICODE_TRANSLATION)
				{
				// Either we don't have enough destination space or there's bad data in the source.
				// Halve the source amount we're going to try to decode.
				iSourceCU /= 2;
				if (!iSourceCU)
					{
					// iSourceCU went to zero. 
					oSourceCU = 0;
					oDestCU = 0;
					break;
					}
				}
			else if (err == ERROR_INVALID_FLAGS)
				{
				theFlags = 0;
				}
			else
				{
				ZDebugStopf(0, ("MultiByteToWideChar returned an unexpected error"));
				}
			}
		}
	}


bool ZTextDecoder_Win::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* iDest, size_t iDestCU, size_t* oDestCU)
	{
	WCHAR utf16Buffer[kBufSize];

	const CHAR* localSource = static_cast<const CHAR*>(iSource);
	UTF32* localDest = iDest;

	if (oSourceBytesSkipped)
		*oSourceBytesSkipped = 0;

	while (iSourceBytes && iDestCU)
		{
		size_t sourceConsumed;
		size_t utf16Generated;
		if (oSourceBytesSkipped)
			{
			sMBToWC_CanFail(fSourceCodePage, localSource, iSourceBytes, sourceConsumed,
					utf16Buffer, min(iDestCU, kBufSize), utf16Generated);
			if (sourceConsumed == 0)
				{
				// We failed to consume any of the source, so we must have moved localSource
				// until it's pointing at a bad byte. Skip over it.
				++localSource;
				--iSourceBytes;
				++*oSourceBytesSkipped;
				}
			}
		else
			{
			sMBToWC(fSourceCodePage, localSource, iSourceBytes, sourceConsumed,
					utf16Buffer, min(iDestCU, kBufSize), utf16Generated);
			}

		size_t utf16Consumed;
		size_t utf32Generated;
		ZUnicode::sUTF16ToUTF32(
			reinterpret_cast<const UTF16*>(utf16Buffer), utf16Generated,
			&utf16Consumed, nullptr, 
			localDest, iDestCU,
			&utf32Generated);

		if (utf16Generated > utf16Consumed)
			{
			// We were not able to consume all the utf16 data generated by sMBToWC. So
			// the number of source code units consumed is *not* sourceConsumed, but some
			// lesser number. This would likely only occur if the UTF-16 had decomposed
			// CP sequences, which we ask that MultiByteToWideChar not generate.
			ZUnimplemented();
			}
		localSource += sourceConsumed;
		iSourceBytes -= sourceConsumed;
		localDest += utf32Generated;
		iDestCU -= utf32Generated;
		}

	if (oSourceBytes)
		*oSourceBytes = localSource - static_cast<const CHAR*>(iSource);
	if (oDestCU)
		*oDestCU = localDest - iDest;
	return true;
	}

void ZTextDecoder_Win::Init(UINT iSourceCodePage)
	{	
	if (!::IsValidCodePage(iSourceCodePage))
		throw runtime_error("ZTextDecoder_Win, invalid code page");
	fSourceCodePage = iSourceCodePage;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTextEncoder_Win

ZTextEncoder_Win::ZTextEncoder_Win(const char* iName)
	{
	this->Init(sLookupName(iName));
	}

ZTextEncoder_Win::ZTextEncoder_Win(const string& iName)
	{
	this->Init(sLookupName(iName));
	}

ZTextEncoder_Win::ZTextEncoder_Win(UINT iDestCodePage)
	{
	this->Init(iDestCodePage);
	}

ZTextEncoder_Win::~ZTextEncoder_Win()
	{}

static void sWCToMB(UINT iDestCodePage, const WCHAR* iSource, size_t iSourceCU, size_t& oSourceCU,
			CHAR* iDest, size_t iDestCU, size_t& oDestCU)
	{
	ZAssertStop(1, iSourceCU && iDestCU);
	if (iSourceCU > iDestCU)
		{
		// We've got more UTF-16 than destination bytes, so discover how much space we would need.
		for (;;)
			{
			int result = ::WideCharToMultiByte(
				iDestCodePage, 0, iSource, iSourceCU, iDest, 0, nullptr, nullptr);

			if (result <= iDestCU)
				break;
			// There is insufficient space. Scale the source count to match. This is of course
			// only approximate, but will get us in the right ballpark immediately.
			iSourceCU = size_t(iSourceCU * (double(iDestCU) / double(result)));
			}
		}

	for (;;)
		{
		if (int result = ::WideCharToMultiByte(
			iDestCodePage, 0, iSource, iSourceCU, iDest, iDestCU, nullptr, nullptr))
			{
			oSourceCU = iSourceCU;
			oDestCU = result;
			break;
			}
		else
			{
			DWORD err = ::GetLastError();
			if (err == ERROR_INSUFFICIENT_BUFFER)
				{
				// We don't have enough space, and don't know how much we need.
				// Just halve the source amount.
				iSourceCU /= 2;
				ZAssertStopf(0, iSourceCU, ("iSourceCU went to zero."));
				if (!iSourceCU)
					break;
				}
			else
				{
				ZDebugStopf(0, ("MultiByteToWideChar returned an unexpected error"));
				}
			}
		}
	}

void ZTextEncoder_Win::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
					void* iDest, size_t iDestBytes, size_t* oDestBytes)
	{
	WCHAR utf16Buffer[kBufSize];

	const UTF32* localSource = iSource;
	CHAR* localDest = static_cast<CHAR*>(iDest);

	while (iSourceCU && iDestBytes)
		{
		size_t utf32Consumed;
		size_t utf16Generated;
		ZUnicode::sUTF32ToUTF16(
			localSource, iSourceCU,
			&utf32Consumed, nullptr,
			reinterpret_cast<UTF16*>(utf16Buffer), kBufSize,
			&utf16Generated, 
			iSourceCU, nullptr);

		size_t utf16Consumed;
		size_t destGenerated;
		sWCToMB(fDestCodePage, utf16Buffer, utf16Generated, utf16Consumed,
			localDest, iDestBytes, destGenerated);

		if (utf16Generated > utf16Consumed)
			{
			// We were not able to consume all the intermediary UTF-16 that was generated.
			// Turn the number of complete code points consumed back into the number of
			// corresponding UTF-32 code units.
			size_t codePoints = ZUnicode::sCUToCP(
				reinterpret_cast<const UTF16*>(utf16Buffer), utf16Consumed);

			utf32Consumed = ZUnicode::sCPToCU(localSource, codePoints);
			}
		localSource += utf32Consumed;
		iSourceCU -= utf32Consumed;
		localDest += destGenerated;
		iDestBytes -= destGenerated;
		}

	if (oSourceCU)
		*oSourceCU = localSource - iSource;
	if (oDestBytes)
		*oDestBytes = localDest - static_cast<CHAR*>(iDest);
	}

void ZTextEncoder_Win::Init(UINT iDestCodePage)
	{
	if (!::IsValidCodePage(iDestCodePage))
		throw runtime_error("ZTextDecoder_Win, invalid code page");
	fDestCodePage = iDestCodePage;
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(TextCoder_Win)
