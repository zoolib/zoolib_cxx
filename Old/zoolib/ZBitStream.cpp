/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#include "zoolib/Memory.h"

#include "zoolib/ZBitStream.h"
#include "zoolib/ZDebug.h"

using std::min;

namespace ZooLib {

#define kDebug_BitStream 2

// =================================================================================================
#pragma mark -
#pragma mark ZBitReaderBE

/** \class ZBitReaderBE
\sa ZBitWriterBE
*/

ZBitReaderBE::ZBitReaderBE()
:	fAvailBits(0)
	{}

ZBitReaderBE::~ZBitReaderBE()
	{}

/// Return \a iCountBits in the low bit positions of \a oResult
/**
If there are insufficient bits buffered from previous calls then read additional data
from \a iStream. The bits in the stream are considered to be ordered from low to high
within consecutive bytes. So the first bit in the stream is returned as bit zero of
the low byte of \a oResult. Bit zero is that bit representing 1 rather than that
representing 128.
*/
bool ZBitReaderBE::ReadBits(const ZStreamR& iStream, size_t iCountBits, uint32& oResult)
	{
	ZAssertStop(kDebug_BitStream, iCountBits <= 32);
	oResult = 0;
	size_t shift = 0;
	while (iCountBits)
		{
		if (fAvailBits == 0)
			{
			if (not iStream.ReadByte(fBuffer))
				return false;
			fAvailBits = 8;
			}
		size_t bitsToUse = min(iCountBits, fAvailBits);
		size_t mask = (1 << bitsToUse) - 1;
		oResult |= (fBuffer & mask) << shift;
		fBuffer = fBuffer >> bitsToUse;
		fAvailBits -= bitsToUse;
		iCountBits -= bitsToUse;
		shift += bitsToUse;
		}
	return true;
	}

/// Return \a iCountBits in the low bit positions of \a oResult
/**
If there are insufficient bits buffered from previous calls then read additional data
from \a iSource, the number of integral bytes read being returned in \a oCountBytesConsumed.
The bits in the stream are considered to be ordered from low to high within consecutive
bytes. So the first byte in memory is returned in the low byte of \a oResult (this
is the same as little endian byte order.)
*/
void ZBitReaderBE::ReadBits(
	const void* iSource, size_t iCountBits, uint32& oResult, size_t* oCountBytesConsumed)
	{
	ZAssertStop(kDebug_BitStream, iCountBits <= 32);
	if (oCountBytesConsumed)
		*oCountBytesConsumed = 0;
	const uint8* localSource = static_cast<const uint8*>(iSource);
	oResult = 0;
	size_t shift = 0;
	while (iCountBits)
		{
		if (fAvailBits == 0)
			{
			fBuffer = *localSource++;
			if (oCountBytesConsumed)
				*oCountBytesConsumed += 1;
			fAvailBits = 8;
			}
		size_t bitsToUse = min(iCountBits, fAvailBits);
		size_t mask = (1 << bitsToUse) - 1;
		oResult |= (fBuffer & mask) << shift;
		fBuffer = fBuffer >> bitsToUse;
		fAvailBits -= bitsToUse;
		iCountBits -= bitsToUse;
		shift += bitsToUse;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark ZBitReaderLE

/** \class ZBitReaderLE
\sa ZBitReaderLE
*/

ZBitReaderLE::ZBitReaderLE()
:	fAvailBits(0)
	{}

ZBitReaderLE::~ZBitReaderLE()
	{}

/// Return \a iCountBits in the low bit positions of \a oResult
/**
If there are insufficient bits buffered from previous calls then read additional data
from \a iStream. The bits in the stream are considered to be ordered from high to low
within consecutive bytes. When returned they are ordered from low to high.
So the first bit in the stream is the high bit of the first byte, but is returned as
bit zero of the low byte of \a oResult.
*/
bool ZBitReaderLE::ReadBits(const ZStreamR& iStream, size_t iCountBits, uint32& oResult)
	{
	ZAssertStop(kDebug_BitStream, iCountBits <= 32);
	oResult = 0;
	size_t shift = 0;
	while (iCountBits--)
		{
		if (fAvailBits-- == 0)
			{
			if (not iStream.ReadByte(fBuffer))
				return false;
			fAvailBits = 7;
			}
		oResult |= uint32(((fBuffer >> fAvailBits) & 1)) << shift;
		++shift;
		}
	return true;
	}

/// Return \a iCountBits in the low bit positions of \a oResult
/**
If there are insufficient bits buffered from previous calls then read additional data
from \a iSource, the number of integral bytes read being returned in \a oCountBytesConsumed.
The bits in the stream are considered to be ordered from low to high within consecutive
bytes. So the first byte in memory is returned in the low byte of \a oResult (this
is the same as little endian byte order.)
*/
void ZBitReaderLE::ReadBits(
	const void* iSource, size_t iCountBits, uint32& oResult, size_t* oCountBytesConsumed)
	{
	ZUnimplemented();
	ZAssertStop(kDebug_BitStream, iCountBits <= 32);
	const uint8* localSource = static_cast<const uint8*>(iSource);
	oResult = 0;
	while (iCountBits--)
		{
		if (fAvailBits-- == 0)
			{
			fBuffer = *localSource++;
			fAvailBits = 7;
			oResult <<= 8;
			}
		oResult |= (fBuffer >> fAvailBits) & 1;
		}

	if (oCountBytesConsumed)
		*oCountBytesConsumed = localSource - static_cast<const uint8*>(iSource);
	}

// =================================================================================================
#pragma mark -
#pragma mark ZBitWriterBE

/** \class ZBitWriterBE
\sa ZBitReaderBE
*/

ZBitWriterBE::ZBitWriterBE()
:	fBuffer(0),
	fAvailBits(8)
	{}

ZBitWriterBE::~ZBitWriterBE()
	{}

/// Write the low \a iCountBits from \a iBits to \a iStream.
/**
Bits are written one byte at a time, so are buffered by ZBitWriterBE until 8
are available. The bits in the stream are considered to be ordered from low to
high within consecutive bytes. Passing 16 or 32 bits at a time in \a iBits will
thus put the data onto the stream in little endian order.
*/
bool ZBitWriterBE::WriteBits(const ZStreamW& iStream, size_t iCountBits, uint32 iBits)
	{
	ZAssertStop(kDebug_BitStream, iCountBits <= 32);
	ZAssertStop(kDebug_BitStream, iBits < (1U << iCountBits));
	while (iCountBits)
		{
		if (fAvailBits == 0)
			{
			size_t countWritten;
			iStream.Write(&fBuffer, 1, &countWritten);
			if (countWritten == 0)
				return false;
			fAvailBits = 8;
			fBuffer = 0;
			}
		size_t bitsToUse = min(iCountBits, fAvailBits);
		size_t mask = (1 << bitsToUse) - 1;
		fBuffer |= (iBits & mask) << (8 - fAvailBits);
		iBits = iBits >> bitsToUse;
		fAvailBits -= bitsToUse;
		iCountBits -= bitsToUse;
		}
	return true;
	}

/// Write the low \a iCountBits from \a iBits to \a oDest.
/**
Bits are written one byte at a time, so are buffered by ZBitWriterBE until 8
are available. The number of bytes actually written is returned in \a oCountBytesWritten,
and can of course be zero. The bits in memory are considered to be ordered from low to
high within consecutive bytes. Passing 16 or 32 bits at a time in \a iBits will
thus put the data into memory in little endian order.
*/
void ZBitWriterBE::WriteBits(
	void* oDest, size_t iCountBits, uint32 iBits, size_t* oCountBytesWritten)
	{
	ZAssertStop(kDebug_BitStream, iCountBits <= 32);
	ZAssertStop(kDebug_BitStream, iBits < (1U << iCountBits));

	uint8* localDest = static_cast<uint8*>(oDest);
	while (iCountBits)
		{
		if (fAvailBits == 0)
			{
			*localDest++ = fBuffer;
			fAvailBits = 8;
			fBuffer = 0;
			}
		size_t bitsToUse = min(iCountBits, fAvailBits);
		size_t mask = (1 << bitsToUse) - 1;
		fBuffer |= (iBits & mask) << (8 - fAvailBits);
		iBits = iBits >> bitsToUse;
		fAvailBits -= bitsToUse;
		iCountBits -= bitsToUse;
		}

	if (oCountBytesWritten)
		*oCountBytesWritten = localDest - static_cast<uint8*>(oDest);
	}

/// Flush the buffer by writing pending bits to \a iStream.
void ZBitWriterBE::Finish(const ZStreamW& iStream)
	{
	if (fAvailBits != 8)
		iStream.Write(&fBuffer, 1, nullptr);
	}

/// Flush the buffer by writing pending bits to \a oDest.
/**
The number of bytes written is returned in \a oCountBytesWritten.
*/
void ZBitWriterBE::Finish(void* oDest, size_t* oCountBytesWritten)
	{
	if (fAvailBits != 8)
		{
		*static_cast<uint8*>(oDest) = fBuffer;
		if (oCountBytesWritten)
			*oCountBytesWritten = 1;
		fAvailBits = 8;
		}
	else
		{
		if (oCountBytesWritten)
			*oCountBytesWritten = 0;
		}
	}
// =================================================================================================
#pragma mark -
#pragma mark ZBitWriterLE

/** \class ZBitWriterLE
\sa ZBitReaderLE
*/

ZBitWriterLE::ZBitWriterLE()
:	fBuffer(0),
	fAvailBits(8)
	{}

ZBitWriterLE::~ZBitWriterLE()
	{}

/// Write the low \a iCountBits from \a iBits to \a iStream.
/**
Bits are written one byte at a time, so are buffered by ZBitWriterLE until 8
are available. The bits in the stream are considered to be ordered from high to
low within consecutive bytes, but are passed in from low to high.
*/
bool ZBitWriterLE::WriteBits(const ZStreamW& iStream, size_t iCountBits, uint32 iBits)
	{
	ZAssertStop(kDebug_BitStream, iCountBits <= 32);
	ZAssertStop(kDebug_BitStream, iBits < (1U << iCountBits));
	while (iCountBits--)
		{
		if (fAvailBits-- == 0)
			{
			size_t countWritten;
			iStream.Write(&fBuffer, 1, &countWritten);
			if (countWritten == 0)
				return false;
			fAvailBits = 7;
			fBuffer = 0;
			}
		fBuffer |= (iBits & 1) << fAvailBits;
		iBits >>= 1;
		}
	return true;
	}

/// Write the low \a iCountBits from \a iBits to \a oDest.
/**
Bits are written one byte at a time, so are buffered by ZBitWriterLE until 8
are available. The number of bytes actually written is returned in \a oCountBytesWritten,
and can of course be zero. The bits in memory are considered to be ordered from high to
low within consecutive bytes, but are passed in from low to high.
*/
void ZBitWriterLE::WriteBits(
	void* oDest, size_t iCountBits, uint32 iBits, size_t* oCountBytesWritten)
	{
	ZAssertStop(kDebug_BitStream, iCountBits <= 32);
	ZAssertStop(kDebug_BitStream, iBits < (1U << iCountBits));

	uint8* localDest = static_cast<uint8*>(oDest);
	while (iCountBits--)
		{
		if (fAvailBits-- == 0)
			{
			*localDest++ = fBuffer;
			fAvailBits = 7;
			fBuffer = 0;
			}
		fBuffer |= (iBits & 1) << fAvailBits;
		iBits >>= 1;
		}

	if (oCountBytesWritten)
		*oCountBytesWritten = localDest - static_cast<uint8*>(oDest);
	}

/// Flush the buffer by writing pending bits to \a iStream.
void ZBitWriterLE::Finish(const ZStreamW& iStream)
	{
	if (fAvailBits != 8)
		iStream.Write(&fBuffer, 1, nullptr);
	}

/// Flush the buffer by writing pending bits to \a oDest.
/**
The number of bytes written is returned in \a oCountBytesWritten.
*/
void ZBitWriterLE::Finish(void* oDest, size_t* oCountBytesWritten)
	{
	if (fAvailBits != 8)
		{
		*static_cast<uint8*>(oDest) = fBuffer;
		if (oCountBytesWritten)
			*oCountBytesWritten = 1;
		fAvailBits = 8;
		}
	else
		{
		if (oCountBytesWritten)
			*oCountBytesWritten = 0;
		}
	}

} // namespace ZooLib
