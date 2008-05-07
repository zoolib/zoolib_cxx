/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "ZMemory.h"
#include "ZStream.h"
#include "ZTextCoder.h"

using std::min;

static const size_t kBufSize = ZooLib::sStackBufferSize;

// =================================================================================================
#pragma mark -
#pragma mark * ZTextCoder

namespace ZANONYMOUS {
struct CharsetEntry_t
	{
	const char* fName;
	int fMIB;
	};
} // anonymous namespace

static const CharsetEntry_t sCharsetTable[] =
	{
		{ "csmacintosh", 2027 },
		{ "mac", 2027 },
		{ "macintosh", 2027 },
		{ "x-mac-roman", 2027 },
		{ "mac-roman", 2027 },
		{ "macroman", 2027 },
	
		{ "cp1252", 2252 },
		{ "windows-1252", 2252 },
		{ "winlatin1", 2252 },
		{ "x-ansi", 2252 },
	
		{ "8859_1", 4 },
		{ "cp819", 4 },
		{ "csisolatin1", 4 },
		{ "ibm819", 4 },
		{ "iso-8859-1", 4 },
		{ "iso-ir-100", 4 },
		{ "iso8859-1", 4 },
		{ "iso_8859-1", 4 },
		{ "iso_8859-1:1987", 4 },
		{ "l1", 4 },
		{ "latin1", 4 },
	
		{ "x-nextstep", -1 },
	
		{ "ansi_x3.4-1968", 3 },
		{ "ansi_x3.4-1986", 3 },
		{ "ascii", 3 },
		{ "cp367", 3 },
		{ "csascii", 3 },
		{ "ibm367", 3 },
		{ "iso-ir-6", 3 },
		{ "iso-ir-6us", 3 },
		{ "iso646-us", 3 },
		{ "iso_646.irv:1991", 3 },
		{ "us", 3 },
		{ "us-ascii", 3 },
	
		{ "csunicode", 1000 },
		{ "iso-10646-ucs-2", 1000 },
	
		{ "unicode-1-1-utf-8", 106 },
		{ "unicode-2-0-utf-8", 106 },
		{ "utf-16", 106 },
		{ "utf-8", 106 },
		{ "x-unicode-2-0-utf-8", 106 },
	
		{ "x-mac-japanese", -1 },
	
		{ "x-mac-chinesetrad", -1 },
		{ "x-mac-trad-chinese", -1 },
	
		{ "csksc56011987", 36 },
		{ "iso-ir-149", 36 },
		{ "korean", 36 },
		{ "ks_c_5601", 36 },
		{ "ks_c_5601-1987", 36 },
		{ "ks_c_5601-1989", 36 },
		{ "ks_c_5601_1987", 36 },
		{ "ksc5601", 36 },
		{ "ksc_5601", 36 },
		{ "x-mac-korean", 36 },
	
		{ "x-mac-arabic", -1 },
		{ "x-mac-hebrew", -1 },
		{ "x-mac-greek", -1 },
		{ "x-mac-cyrillic", -1 },
		{ "x-mac-devanagari", -1 },
		{ "x-mac-gurmukhi", -1 },
		{ "x-mac-gujarati", -1 },
		{ "x-mac-thai", -1 },
		{ "x-mac-chinesesimp", -1 },
		{ "x-mac-simp-chinese", -1 },
		{ "x-mac-tibetan", -1 },
		{ "x-mac-ce", -1 },
		{ "x-mac-centraleurroman", -1 },
		{ "x-mac-symbol", -1 },
		{ "x-mac-dingbats", -1 },
		{ "x-mac-turkish", -1 },
		{ "x-mac-croatian", -1 },
		{ "x-mac-icelandic", -1 },
		{ "x-mac-romanian", -1 },
		{ "x-mac-farsi", -1 },
		{ "x-mac-ukrainian", -1 },
		{ "x-mac-vt100", -1 },
	
		{ "csisolatin2", 5 },
		{ "iso-8859-2", 5 },
		{ "iso-ir-101", 5 },
		{ "iso8859-2", 5 },
		{ "iso_8859-2", 5 },
		{ "iso_8859-2:1987", 5 },
		{ "l2", 5 },
		{ "latin2", 5 },
	
		{ "csisolatin3", 6 },
		{ "iso-8859-3", 6 },
		{ "iso-ir-109", 6 },
		{ "iso_8859-3", 6 },
		{ "iso_8859-3:1988", 6 },
		{ "l3", 6 },
		{ "latin3", 6 },
	
		{ "csisolatin4", 7 },
		{ "iso-8859-4", 7 },
		{ "iso-ir-110", 7 },
		{ "iso_8859-4", 7 },
		{ "iso_8859-4:1988", 7 },
		{ "l4", 7 },
		{ "latin4", 7 },
	
		{ "csisolatincyrillic", 8 },
		{ "cyrillic", 8 },
		{ "iso-8859-5", 8 },
		{ "iso-ir-144", 8 },
		{ "iso_8859-5", 8 },
		{ "iso_8859-5:1988", 8 },
	
		{ "arabic", 9 },
		{ "asmo-708", 9 },
		{ "csisolatinarabic", 9 },
		{ "ecma-114", 9 },
		{ "iso-8859-6", 9 },
		{ "iso-ir-127", 9 },
		{ "iso_8859-6", 9 },
		{ "iso_8859-6:1987", 9 },
	
		{ "csisolatingreek", 10 },
		{ "ecma-118", 10 },
		{ "elot_928", 10 },
		{ "greek", 10 },
		{ "greek8", 10 },
		{ "iso-8859-7", 10 },
		{ "iso-ir-126", 10 },
		{ "iso_8859-7", 10 },
		{ "iso_8859-7:1987", 10 },
	
		{ "csiso88598e", 11 },
		{ "csiso88598i", 11 },
		{ "csisolatinhebrew", 11 },
		{ "dos-862", 11 },
		{ "hebrew", 11 },
		{ "iso-8859-8", 11 },
		{ "iso-8859-8-e", 11 },
		{ "iso-8859-8-i", 11 },
		{ "iso-ir-138", 11 },
		{ "iso_8859-8", 11 },
		{ "iso_8859-8-e", 11 },
		{ "iso_8859-8-i", 11 },
		{ "iso_8859-8:1988", 11 },
		{ "logical", 11 },
		{ "visual", 11 },
	
		{ "csisolatin5", 12 },
		{ "iso-8859-9", 12 },
		{ "iso-ir-148", 12 },
		{ "iso_8859-9", 12 },
		{ "iso_8859-9:1989", 12 },
		{ "l5", 12 },
		{ "latin5", 12 },
	
		{ "csisolatin6", 13 },
		{ "iso-8859-10", 13 },
		{ "iso-ir-157", 13 },
		{ "iso_8859-10:1992", 13 },
		{ "l6", 13 },
		{ "latin6", 13 },
	
		{ "iso-8859-11", -1 },
	
		{ "iso-8859-13", 109 },
	
		{ "iso-8859-14", 110 },
		{ "iso-celtic", 110 },
		{ "iso-ir-199", 110 },
		{ "iso_8859-14", 110 },
		{ "iso_8859-14:1998", 110 },
		{ "l8", 110 },
		{ "latin8", 110 },
	
		{ "csisolatin9", 111 },
		{ "iso-8859-15", 111 },
		{ "iso_8859-15", 111 },
		{ "l9", 111 },
		{ "latin-9", 111 },
		{ "latin9", 111 },
	
		{ "437", 2011 },
		{ "cp437", 2011 },
		{ "cspc8codepage437", 2011 },
		{ "ibm437", 2011 },
	
		{ "cp737", -1 },
		{ "ibm737", -1 },
	
		{ "cp500", 2087 },
		{ "cp775", 2087 },
		{ "csibm500", 2087 },
		{ "cspc775baltic", 2087 },
		{ "ebcdic-cp-be", 2087 },
		{ "ebcdic-cp-ch", 2087 },
		{ "ibm500", 2087 },
		{ "ibm775", 2087 },
	
		{ "850", 2009 },
		{ "cp850", 2009 },
		{ "cspc850multilingual", 2009 },
		{ "ibm850", 2009 },
	
		{ "852", 2010 },
		{ "cp852", 2010 },
		{ "cspcp852", 2010 },
		{ "ibm852", 2010 },
	
		{ "857", 2047 },
		{ "cp857", 2047 },
		{ "csibm857", 2047 },
		{ "ibm857", 2047 },
	
		{ "861", 2049 },
		{ "cp-is", 2049 },
		{ "cp861", 2049 },
		{ "csibm861", 2049 },
		{ "ibm861", 2049 },
	
		{ "cp864", 2051 },
		{ "csibm864", 2051 },
		{ "dos-720", 2051 },
		{ "ibm864", 2051 },
	
		{ "866", 2086 },
		{ "cp866", 2086 },
		{ "csibm866", 2086 },
		{ "ibm866", 2086 },
	
		{ "869", 2054 },
		{ "cp-gr", 2054 },
		{ "cp869", 2054 },
		{ "csibm869", 2054 },
		{ "ibm869", 2054 },
	
		{ "cp874", 2259 },
		{ "dos-874", 2259 },
		{ "tis-620", 2259 },
		{ "windows-874", 2259 },
	
		{ "cp932", 2024 },
		{ "csshiftjis", 2024 },
		{ "cswindows31j", 2024 },
		{ "ms_kanji", 2024 },
		{ "shift-jis", 2024 },
		{ "shift_jis", 2024 },
		{ "windows-31j", 2024 },
		{ "x-ms-cp932", 2024 },
		{ "x-sjis", 2024 },
	
		{ "cp949", -1 },
	
		{ "cp950", -1 },
	
		{ "windows-1250", 2250 },
		{ "winlatin2", 2250 },
		{ "x-cp1250", 2250 },
	
		{ "wincyrillic", 2251 },
		{ "windows-1251", 2251 },
		{ "x-cp1251", 2251 },
	
		{ "windows-1253", 2253 },
		{ "wingreek", 2253 },
	
		{ "windows-1254", 2254 },
		{ "winturkish", 2254 },
	
		{ "windows-1255", 2255 },
		{ "winhebrew", 2255 },
	
		{ "cp1256", 2256 },
		{ "winarabic", 2256 },
		{ "windows-1256", 2256 },
	
		{ "winbaltic", 2257 },
		{ "windows-1257", 2257 },
	
		{ "johab", -1 },
	
		{ "windows-1258", 2258 },
		{ "winvietnamese", 2258 },
	
		{ "cshalfwidthkatakana", 15 },
		{ "jis_x0201", 15 },
		{ "x0201", 15 },
	
		{ "csiso87jisx0208", 63 },
		{ "iso-ir-87", 63 },
		{ "jis_c6226-1983", 63 },
		{ "jis_x0208-1983", 63 },
		{ "x0208", 63 },
	
		{ "jis_x0208-1990", -1 },
	
		{ "csiso159jisx02121990", 98 },
		{ "iso-ir-159", 98 },
		{ "jis_x0212-1990", 98 },
		{ "x0212", 98 },
	
		{ "csiso42jisc62261978", 49 },
		{ "iso-ir-42", 49 },
		{ "jis_c6226-1978", 49 },
	
		{ "chinese", 57 },
		{ "csgb231280", 57 },
		{ "csiso58gb231280", 57 },
		{ "gb2312-80", 57 },
		{ "gb231280", 57 },
		{ "gb_2312-80", 57 },
		{ "iso-ir-58", 57 },
	
		{ "x-gbk", -1 },
	
		{ "gb18030", 114 },
	
		{ "csiso2022jp", 39 },
		{ "iso-2022-jp", 39 },
	
		{ "csiso2022jp2", 40 },
		{ "iso-2022-jp-2", 40 },
	
		{ "iso-2022-jp-1", -1 },
	
		{ "iso-2022-jp-3", -1 },
	
		{ "iso-2022-cn", 104 },
	
		{ "iso-2022-cn-ext", 105 },
	
		{ "csiso2022kr", 37 },
		{ "iso-2022-kr", 37 },
	
		{ "cseucpkdfmtjapanese", 18 },
		{ "euc-jp", 18 },
		{ "extended_unix_code_packed_format_for_japanese", 18 },
		{ "x-euc", 18 },
		{ "x-euc-jp", 18 },
	
		{ "cn-gb", 113 },
		{ "cp936", 113 },
		{ "csgb2312", 113 },
		{ "euc-cn", 113 },
		{ "gb2312", 113 },
		{ "gbk", 113 },
		{ "ms936", 113 },
		{ "windows-936", 113 },
		{ "x-euc-cn", 113 },
	
		{ "euc-tw", -1 },
	
		{ "cseuckr", 38 },
		{ "euc-kr", 38 },
	
		{ "cskoi8r", 2084 },
		{ "koi", 2084 },
		{ "koi8", 2084 },
		{ "koi8-r", 2084 },
		{ "koi8r", 2084 },
	
		{ "big5", 2026 },
		{ "cn-big5", 2026 },
		{ "csbig5", 2026 },
		{ "x-x-big5", 2026 },
	
		{ "x-mac-roman-latin1", -1 },
	
		{ "hz-gb-2312", 2085 },
	
		{ "big5-hkscs", 2101 },
	
		{ "cp037", 2028 },
		{ "csibm037", 2028 },
		{ "ebcdic-cp-ca", 2028 },
		{ "ebcdic-cp-nl", 2028 },
		{ "ebcdic-cp-us", 2028 },
		{ "ebcdic-cp-wt", 2028 },
		{ "ibm037", 2028 },
		{ 0, 0 }
	};

void ZTextCoder::sGetAliases(const std::string& iName, std::vector<std::string>& oAliases)
	{	
	const CharsetEntry_t* found = nil;
	for (const CharsetEntry_t* current = sCharsetTable; current->fName && !found; ++current)
		{
		if (iName == current->fName)
			found = current;
		}

	if (found)
		{
		int theMIB = found->fMIB;
		if (theMIB != -1)
			{
			for (const CharsetEntry_t* current = sCharsetTable; current->fName; ++current)
				{
				if (theMIB == current->fMIB)
					oAliases.push_back(current->fName);
				}
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTextDecoder

/**
\class ZTextDecoder
\sa group_Unicode
\sa ZTextEncoder

\brief Takes binary data and generates Unicode text according to some encoding scheme.

The standard ZTextDecoder derivatives follow ZooLib's standard practice of treating malformed
data as being something to simply skip. Malformed data generally means data that either does
not conform to the specification for the particular text encoding (illegal byte values or
sequences) or occasionally data that represents code points that have no equivalent in Unicode,
although this is extremely rare.

Decoders are allowed and expected to maintain state, so that large sequences of source material
can processed by multiple calls to ZTextDecoder::Decode. A decoder is assumed to be in a fresh
state when first created, and can be returned to that state by calling ZTextDecoder::Reset.
*/

/**
\fn bool ZTextDecoder::Decode( \
const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped, \
UTF32* iDest, size_t iDestCU, size_t* oDestCU)

\brief Decode UTF-32 text, reading the encoded form from a buffer in memory.

For single byte encodings implementing ZTextDecoder::Decode is straightforward, generally just
a table lookup for each byte in the source. For multi-byte encodings some subtlety is required,
particularly when handling source buffers that contain valid but incomplete data.

\param iSource Points to the start of the encoded data to be decoded.
\param iSourceBytes The number of bytes of encoded data that are available to be decoded.
\param oSourceBytes (optional output) The number of bytes of encoded data that were consumed.
\param oSourceBytesSkipped (optional output) The number of bytes of encoded data that were
				illegal or malformed in some fashion, were ignored and so did not contribute
				to any generated UTF-32 code units.
\param iDest Points to the start of the buffer into which UTF-32 is to be placed.
\param iDestCU The maximum number of UTF-32 code units to be generated. It's the size
				of the buffer referenced by \a iDest.
\param oDestCU (optional output) The number of UTF-32 code units that were generated. This
				will also be the number of Unicode code points, provided that your decoder does
				not generate illegal UTF-32 code units.
\return \li \a true The normal return value.

\return \li \a false If \a iSourceBytes and \a iDestCU were non-zero but nothing could be consumed
				and nothing generated, generally because the source data was incomplete. This can
				only occur with source encodings that may require more than a single byte to
				represent a single Unicode code point.

It's entirely legal for a decoder to consume source data without generating any UTF-32, e.g. if
the source contains only illegal code units they will be skipped over without generating output,
and the total number of bytes making up such skipped data will be placed in \a oSourceBytesSkipped.
It's also feasible that a decoder might suck up and buffer source data in one set of calls, and
later generate UTF-32 output without consuming any source data. For example
ZTextDecoder_Unicode_AutoDetect does this, in order to more cleanly handle the skipping of a BOM.
*/

/** \brief Decode UTF-32 text, reading the encoded form from a ZStreamR.

The ZStreamR protocol is such that data cannot be returned to the stream once read. In order
to allow ZTextDecoder derivatives to take advantage of their knowledge of an encoding scheme
ZTextDecoder::Decode is an overrideable method. The platform-specific decoders work only in terms
of buffers, and do not provide enough information to communicate how much data they need to read
in order to generate a certain quantity of output. The default implementation of
ZTextDecoder::Decode has to nibble away at the stream in order not to over-read it.

\param iStreamR The stream from which encoded data is to be read.
\param iDest Points to the start of the buffer into which UTF-32 is to be placed.
\param iDestCU The maximum number of UTF-32 code units to be generated. It's the size
				of the buffer referenced by \a iDest.
\param oDestCU (optional output) The number of UTF-32 code units that were generated. This
				will also be the number of Unicode code points, provided that your decoder does
				not generate illegal UTF-32 code units.
*/
void ZTextDecoder::Decode(const ZStreamR& iStreamR, UTF32* iDest, size_t iDestCU, size_t* oDestCU)
	{
	uint8 buffer[kBufSize];

	UTF32* localDest = iDest;
	size_t bufferUsed = 0;
	while (iDestCU)
		{
		size_t countRead;
		iStreamR.Read(buffer + bufferUsed, min(kBufSize - bufferUsed, iDestCU), &countRead);
		if (countRead == 0)
			{
			// We may still have data in the buffer, in which case bufferUsed will be non-zero.
			// However it must be incomplete data, code units that in themselves do not
			// form a valid code point, otherwise the call to Decode on the last iteration
			// would have consumed them, and as the stream has gone empty we will not be
			// getting any more code units which could, in conjunction with the data in the
			// buffer, represent valid code points. If we were decoding from a buffer, rather
			// than a stream, then we could return a value indicating that this situation
			// has occurred. But the data has already been irrevocably pulled from the stream
			// and in this general implementation there is no way for a caller to do anything
			// with the knowledge. So we simply abandon the buffered data.
			break;
			}
		bufferUsed += countRead;

		size_t countConsumed;
		size_t utf32Generated;
		this->Decode(buffer, bufferUsed, &countConsumed, nil, localDest, iDestCU, &utf32Generated);

		// Remove the consumed code units from the buffer by reducing bufferUsed
		// and moving any remaining data to the front of the buffer.
		bufferUsed -= countConsumed;
		ZBlockMove(buffer + countConsumed, buffer, bufferUsed);

		localDest += utf32Generated;
		iDestCU -= utf32Generated;
		}

	if (oDestCU)
		*oDestCU = localDest - iDest;
	}

/** \brief Decode a single UTF-32 codepoint from a ZStreamR.

\param iStreamR The stream from which encoded data is to be read.
\param oCP The code point that was read.

\return \li \a true \a oCP contains a code point read from \a iStreamR.

\return \li \a false No code point could be read from iStreamR, because
				the stream was empty, or contained only malformed data.
*/
bool ZTextDecoder::Decode(const ZStreamR& iStreamR, UTF32& oCP)
	{
	size_t countCU;
	this->Decode(iStreamR, &oCP, 1, &countCU);
	return countCU;
	}

/** \brief Discard any state in preparation for a fresh sequence of source material.*/
void ZTextDecoder::Reset()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTextEncoder

/**
\class ZTextEncoder
\sa group_Unicode
\sa ZTextDecoder

\brief Takes Unicode text and generates binary data according to some encoding scheme.

Encoders are allowed and expected to maintain state, so that large sequences of source
text can be processed by multiple calls to ZTextEncoder::Encode. This state can be
discarded by calling ZTextEncoder::Reset, returning the encoder to the same condition
it was in when it was first instantiated.

Encoding is less tricky than decoding because source material is always a sequence of
UTF-32 code units and thus there is no need to deal with truncated source.
*/

/**
\fn void ZTextEncoder::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU, \
void* iDest, size_t iDestBytes, size_t* oDestBytes)

\brief Encode UTF-32 text, writing the encoded form to a buffer in memory.

\param iSource Points to the start of UTF-32 text to be encoded.
\param iSourceCU The number UTF-32 code units that are available to be encoded.
\param oSourceCU (optional output) The number of UTF-32 code units that were consumed.
\param iDest Points to the start of the buffer into which the encoded data is to be placed.
\param iDestBytes The maximum number bytes of encoded data to be generated. It's the size
				of the buffer referenced by \a iDest.
\param oDestBytes (optional output) The number of bytes of encoded data that were generated.

It's entirely legal for an encoder to consume UTF-32 without generating any encoded data. If
the source has illegal code units they will be skipped over. It's also feasible that an encoder
might suck up and buffer UTF-32 in one set of calls without generating output, and later
generate output without consuming any source. The standard ZTextEncoder derivatives do not
do this, but others might need to.
*/

/** \brief Encode UTF-32 text, writing the encoded form to a ZStreamW.

\param iStreamW The stream to which encoded data is to be written.
\param iSource Points to the start of UTF-32 text to be encoded.
\param iSourceCU The number UTF-32 code units that are available to be encoded.
\param oSourceCU (optional output) The number of UTF-32 code units that were consumed.
*/
void ZTextEncoder::Encode(
	const ZStreamW& iStreamW, const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU)
	{
	uint8 buffer[kBufSize];
	const UTF32* localSource = iSource;
	while (iSourceCU)
		{
		size_t utf32Consumed;
		size_t countGenerated;
		this->Encode(localSource, iSourceCU, &utf32Consumed, buffer, kBufSize, &countGenerated);
		size_t countWritten;
		iStreamW.Write(buffer, countGenerated, &countWritten);
		if (countWritten < countGenerated)
			{
			// There's no direct way for an encoder to tell us which bytes in a buffer correspond
			// to a code point. We could re-encode subsets of the source and binary chop our way
			// to the answer, however we'd need to have saved the encoder's state prior to the
			// original call, in case it is stateful. The proper solution would likely be to allow
			// a caller to pass in an array into which the offsets would be written.
			break;
			}

		iSourceCU -= utf32Consumed;
		localSource += utf32Consumed;
		}

	if (oSourceCU)
		*oSourceCU = localSource - iSource;
	}

/** \brief Encode a single UTF-32 to a ZStreamW.

\param iStreamW The stream to which encoded data is to be written.
\param iCP The code point to write.
*/
void ZTextEncoder::Encode(const ZStreamW& iStreamW, UTF32 iCP)
	{ this->Encode(iStreamW, &iCP, 1, nil); }

/** \brief Discard any state in preparation for a fresh sequence of source material.*/
void ZTextEncoder::Reset()
	{}
