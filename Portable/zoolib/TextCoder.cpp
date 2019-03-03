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

#include "zoolib/TextCoder.h"

#include "zoolib/Memory.h"
#include "zoolib/Unicode.h"

using std::min;

namespace ZooLib {

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
#pragma mark - TextCoder

// =================================================================================================
#pragma mark - TextDecoder

/**
\class TextDecoder
\sa Unicode
\sa TextEncoder

\brief Takes binary data and generates Unicode text according to some encoding scheme.

The standard TextDecoder derivatives follow ZooLib's standard practice of treating malformed
data as being something to simply skip. Malformed data generally means data that either does
not conform to the specification for the particular text encoding (illegal byte values or
sequences) or occasionally data that represents code points that have no equivalent in Unicode,
although this is extremely rare.

Decoders are allowed and expected to maintain state, so that large sequences of source material
can processed by multiple calls to TextDecoder::Decode. A decoder is assumed to be in a fresh
state when first created, and can be returned to that state by calling TextDecoder::Reset.
*/

/**
\fn bool sDecode( \
const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped, \
UTF32* oDest, size_t iDestCU, size_t* oDestCU)

\brief Decode UTF-32 text, reading the encoded form from a buffer in memory.

For single byte encodings implementing TextDecoder::Decode is straightforward, generally just
a table lookup for each byte in the source. For multi-byte encodings some subtlety is required,
particularly when handling source buffers that contain valid but incomplete data.

\param iSource Points to the start of the encoded data to be decoded.
\param iSourceBytes The number of bytes of encoded data that are available to be decoded.
\param oSourceBytes (optional output) The number of bytes of encoded data that were consumed.
\param oSourceBytesSkipped (optional output) The number of bytes of encoded data that were
				illegal or malformed in some fashion, were ignored and so did not contribute
				to any generated UTF-32 code units.
\param oDest Points to the start of the buffer into which UTF-32 is to be placed.
\param iDestCU The maximum number of UTF-32 code units to be generated. It's the size
				of the buffer referenced by \a oDest.
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
TextDecoder_Unicode_AutoDetect does this, in order to more cleanly handle the skipping of a BOM.
*/

bool sDecode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	const ZRef<TextDecoder>& iTextDecoder,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	return sCall(iTextDecoder,
		iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped,
		oDest, iDestCU, oDestCU);
	}

/** \brief Decode UTF-32 text, reading the encoded form from a ChanR_Bin.

The ChanR protocol is such that data cannot be returned to the chanr once read. In order
to allow TextDecoder derivatives to take advantage of their knowledge of an encoding scheme
TextDecoder::Decode is an overrideable method. The platform-specific decoders work only in terms
of buffers, and do not provide enough information to communicate how much data they need to read
in order to generate a certain quantity of output. The default implementation of
TextDecoder::Decode has to nibble away at the chan in order not to over-read it.

\param iChanR The chan from which encoded data is to be read.
\param oDest Points to the start of the buffer into which UTF-32 is to be placed.
\param iDestCU The maximum number of UTF-32 code units to be generated. It's the size
				of the buffer referenced by \a oDest.
\param oDestCU (optional output) The number of UTF-32 code units that were generated. This
				will also be the number of Unicode code points, provided that your decoder does
				not generate illegal UTF-32 code units.
*/
void sDecode(
	const ChanR_Bin& iChanR,
	const ZRef<TextDecoder>& iTextDecoder,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	uint8 buffer[kBufSize];

	UTF32* localDest = oDest;
	size_t bufferUsed = 0;
	while (iDestCU)
		{
		size_t countRead = sRead(iChanR, buffer + bufferUsed, min(kBufSize - bufferUsed, iDestCU));
		if (countRead == 0)
			{
			// We may still have data in the buffer, in which case bufferUsed will be non-zero.
			// However it must be incomplete data, code units that in themselves do not
			// form a valid code point, otherwise the call to Decode on the last iteration
			// would have consumed them, and as the chan has gone empty we will not be
			// getting any more code units which could, in conjunction with the data in the
			// buffer, represent valid code points. If we were decoding from a buffer, rather
			// than a chan, then we could return a value indicating that this situation
			// has occurred. But the data has already been irrevocably pulled from the chan
			// and in this general implementation there is no way for a caller to do anything
			// with the knowledge. So we simply abandon the buffered data.
			break;
			}
		bufferUsed += countRead;

		size_t countConsumed;
		size_t utf32Generated;
		sDecode(
			buffer, bufferUsed, &countConsumed, nullptr,
			iTextDecoder,
			localDest, iDestCU, &utf32Generated);

		// Remove the consumed code units from the buffer by reducing bufferUsed
		// and moving any remaining data to the front of the buffer.
		bufferUsed -= countConsumed;
		sMemMove(buffer, buffer + countConsumed, bufferUsed);

		localDest += utf32Generated;
		iDestCU -= utf32Generated;
		}

	if (oDestCU)
		*oDestCU = localDest - oDest;
	}

/** \brief Decode a single UTF-32 codepoint from a ChanR.

\param iChanR The chan from which encoded data is to be read.
\param iTextDecoder The text decoder to use.
\return A ZQ containing the CP. It will be empty if no CP could be read from iChanR, because
				the stream was empty, or contained only malformed data.
*/
ZQ<UTF32> sQDecode(const ChanR_Bin& iChanR, const ZRef<TextDecoder>& iTextDecoder)
	{
	UTF32 theCP;
	size_t destCU;
	sDecode(iChanR, iTextDecoder, &theCP, 1, &destCU);
	if (destCU)
		return theCP;
	return null;
	}

// =================================================================================================
#pragma mark - TextEncoder

/**
\class TextEncoder
\sa Unicode
\sa TextDecoder

\brief Takes Unicode text and generates binary data according to some encoding scheme.

Encoders are allowed and expected to maintain state, so that large sequences of source
text can be processed by multiple calls to TextEncoder::Encode. This state can be
discarded by calling TextEncoder::Reset, returning the encoder to the same condition
it was in when it was first instantiated.

Encoding is less tricky than decoding because source material is always a sequence of
UTF-32 code units and thus there is no need to deal with truncated source.
*/

/**
\fn void TextEncoder::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU, \
void* oDest, size_t iDestBytes, size_t* oDestBytes)

\brief Encode UTF-32 text, writing the encoded form to a buffer in memory.

\param iSource Points to the start of UTF-32 text to be encoded.
\param iSourceCU The number UTF-32 code units that are available to be encoded.
\param oSourceCU (optional output) The number of UTF-32 code units that were consumed.
\param oDest Points to the start of the buffer into which the encoded data is to be placed.
\param iDestBytes The maximum number bytes of encoded data to be generated. It's the size
				of the buffer referenced by \a oDest.
\param oDestBytes (optional output) The number of bytes of encoded data that were generated.

It's entirely legal for an encoder to consume UTF-32 without generating any encoded data. If
the source has illegal code units they will be skipped over. It's also feasible that an encoder
might suck up and buffer UTF-32 in one set of calls without generating output, and later
generate output without consuming any source. The standard TextEncoder derivatives do not
do this, but others might need to.
*/
void sEncode(
	const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	const ZRef<TextEncoder>& iTextEncoder,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	sCall(iTextEncoder,
		iSource, iSourceCU, oSourceCU,
		oDest, iDestBytes, oDestBytes);
	}

/** \brief Encode UTF-32 text, writing the encoded form to a ZStreamW.

\param iStreamW The stream to which encoded data is to be written.
\param iSource Points to the start of UTF-32 text to be encoded.
\param iSourceCU The number UTF-32 code units that are available to be encoded.
\param oSourceCU (optional output) The number of UTF-32 code units that were consumed.
*/
void sEncode(
	const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	const ZRef<TextEncoder>& iTextEncoder,
	const ChanW_Bin& iChanW)
	{
	uint8 buffer[kBufSize];
	const UTF32* localSource = iSource;
	while (iSourceCU)
		{
		size_t utf32Consumed;
		size_t countGenerated;
		sEncode(localSource, iSourceCU, &utf32Consumed,
			iTextEncoder,
			buffer, kBufSize, &countGenerated);
		size_t countWritten = sWrite(iChanW, buffer, countGenerated);
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
bool sEncode(UTF32 iCP, const ZRef<TextEncoder>& iTextEncoder, const ChanW_Bin& iChanW)
	{
	size_t countConsumed;
	sEncode(&iCP, 1, &countConsumed, iTextEncoder, iChanW);
	return countConsumed;
	}

} // namespace ZooLib
