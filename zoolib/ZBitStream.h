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

#ifndef __ZBitStream__
#define __ZBitStream__ 1
#include "zconfig.h"

#include "ZStream.h"

/*
Bits may be packed from high-to-low (128 down to 1) or from low-to-high (1, 2, 4 ... 128) in
a byte. The BE variant assumes that bit 0 (1) is the first bit in the stream, bit 1 (2) is
the second, bit 3 (8) is the third and so forth. The LE variant assume that bit 7 (128) is the
first, bit 6 (64) is the send and so forth. The current code always returns or takes data
using the BE notation, so reading 8 bits from a LE source will return a byte with the bits
in reverse order. There should probably be BE and LE variants of the Read and Write calls
for each reader and writer. And there should be efficient single bit methods.
*/

// =================================================================================================
#pragma mark -
#pragma mark * ZBitReaderBE

/// ZBitReaderBE reads a bitstream from a ZStreamR or from memory in increments of 1 to 32 bits .

class ZBitReaderBE
	{
public:
	ZBitReaderBE();
	~ZBitReaderBE();

	bool ReadBits(const ZStreamR& iStream, size_t iCountBits, uint32& oResult);
	void ReadBits(const void* iSource, size_t iCountBits,
		uint32& oResult, size_t* oCountBytesConsumed);

private:
	uint8 fBuffer;
	size_t fAvailBits;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZBitReaderLE

/// ZBitReaderLE reads a bitstream from a ZStreamR or from memory in increments of 1 to 32 bits .

class ZBitReaderLE
	{
public:
	ZBitReaderLE();
	~ZBitReaderLE();

	bool ReadBits(const ZStreamR& iStream, size_t iCountBits, uint32& oResult);
	void ReadBits(const void* iSource, size_t iCountBits,
		uint32& oResult, size_t* oCountBytesConsumed);

private:
	uint8 fBuffer;
	size_t fAvailBits;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZBitWriterBE

/// ZBitWriterBE writes a bitstream to a ZStreamW or to memory in increments of 1 to 32 bits.

class ZBitWriterBE
	{
public:
	ZBitWriterBE();
	~ZBitWriterBE();

	bool WriteBits(const ZStreamW& iStream, size_t iCountBits, uint32 iBits);
	void WriteBits(void* iDest, size_t iCountBits, uint32 iBits, size_t* oCountBytesWritten);

	void Finish(const ZStreamW& iStream);
	void Finish(void* iDest, size_t* oCountBytesWritten);

private:
	uint8 fBuffer;
	size_t fAvailBits;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZBitWriterLE

/// ZBitWriterLE writes a bitstream to a ZStreamW or to memory in increments of 1 to 32 bits.

class ZBitWriterLE
	{
public:
	ZBitWriterLE();
	~ZBitWriterLE();

	bool WriteBits(const ZStreamW& iStream, size_t iCountBits, uint32 iBits);
	void WriteBits(void* iDest, size_t iCountBits, uint32 iBits, size_t* oCountBytesWritten);

	void Finish(const ZStreamW& iStream);
	void Finish(void* iDest, size_t* oCountBytesWritten);

private:
	uint8 fBuffer;
	size_t fAvailBits;
	};

#endif // __ZBitStream__
