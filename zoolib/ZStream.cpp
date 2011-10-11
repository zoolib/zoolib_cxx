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

#include "zoolib/ZStream.h"

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZDebug.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h> // For strlen
#include <string> // because range_error may require it
#include <vector>

#if ZCONFIG(Compiler, MSVC)
#	define vsnprintf _vsnprintf
#endif

using std::max;
using std::min;
using std::nothrow;
using std::range_error;
using std::string;

namespace ZooLib {

#define kDebug_Stream 2

/**
\defgroup Stream
\sa Streamer

\section ReadStreams Read Streams
A read stream is an entity from which you can read bytes. At its simplest
you can say 'here is a block of memory, read X bytes into it'. The read
stream will do so, perhaps after blocking until the requested bytes arrive
from whatever source produces them. A read stream has no knowable extent
(aka size) and has no notion of a current position -- you simply read bytes
from it. The only error that can occur is that at some point the read
stream will likely determine that it can no longer provide bytes to you; its
finite limit has been reached, and a request for X bytes cannot now and
never will in the future be satisfiable. The read request thus has three
parameters: the address of the buffer into which the bytes should be placed,
a count of how many bytes we would like to read, and the address of a count
that will be modified to indicate the number of bytes that were actually
read. Usually the count actually read will equal the count requested. If the
stream's limit is reached during a call to read X bytes, the count read will
be less than X. Every subsequent call thereafter will read nothing at all,
and the count read will be zero.

\section WriteStreams Write Streams
The complement to a read stream is (obviously) a write stream. This is
an entity that bytes can be written to. Again we pass it the address of a
buffer, this time the buffer contains the bytes we want written. We also
pass it a count, indicating how many bytes from the buffer should be
written, and we pass the address of a count which will be modified to
indicate how many bytes were successfully written. Just as with the read
stream, the write stream has no 'position' and no size, and it also might
(will) be finite. This is an important point; something that is finite
obviously *must* have a size (curved space notwithstanding), but in the case
of read and write streams the size is knowable only *in retrospect* --
when we hit the limit we know where it was, but we have *no* way of knowing
that limit ahead of time (unless that information is conveyed by some other
mechanism -- but any such mechanism is not part of the standard definition
of read or write streams.)

\section Lifetime
The stream base classes have protected constructor, destructor, copy constructor and assignment
methods. Although a particular subclass may have value semantics, you can't create, destroy or
assign a stream through the base interface.


\section PositionableStreams Positionable Streams

\section ConnectedStreams Connected Streams

*/

/* These checks ensure that floats and doubles are the same size as
32 and 64 bit ints, and thus the same size as IEEE 754 single and double
precision floating point numbers. Of course it doesn't guarantee that
they have the same physical layout as IEEE 754 floating point numbers,
which is the format ZStream has standardized on. Fortunately most
compilers and machines do use the IEEE formats, but if yours doesn't
you'll need to unpack your data in ZStreamR::ReadFloat and ZStreamR::ReadDouble,
and pack it in ZStreamW::WriteFloat and ZStreamW::WriteDouble. */

ZAssertCompile(sizeof(float) == sizeof(int32));
ZAssertCompile(sizeof(double) == sizeof(int64));

// =================================================================================================
#pragma mark -
#pragma mark * Utility methods

static void spCopyReadToWrite(void* iBuffer, size_t iBufferSize,
	const ZStreamR& iStreamR, const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	uint64 countRemaining = iCount;
	while (countRemaining > 0)
		{
		size_t countRead;
		iStreamR.Read(iBuffer, min(countRemaining, uint64(iBufferSize)), &countRead);
		if (countRead == 0)
			break;
		if (oCountRead)
			*oCountRead += countRead;
		countRemaining -= countRead;
		const uint8* tempSource = static_cast<const uint8*>(iBuffer);
		while (countRead > 0)
			{
			size_t countWritten;
			iStreamW.Write(tempSource, countRead, &countWritten);
			if (countWritten == 0)
				{
				countRemaining = 0;
				break;
				}
			tempSource += countWritten;
			countRead -= countWritten;
			if (oCountWritten)
				*oCountWritten += countWritten;
			}
		}
	}

/**
Copy data from \a iStreamR to \a iStreamW by reading it into a buffer and writing
from that buffer. If more than 8K is to be copied we try to allocate an 8K buffer
in the heap. If less than 8K is to be copied, or the heap buffer could not be allocated,
we use a 1K buffer on the stack.
*/
void ZStream::sCopyReadToWrite(const ZStreamR& iStreamR, const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;

	if (iCount == 0)
		return;

	if (iCount > 8192)
		{
		// Try to allocate and use an 8K heap-based buffer.
		if (uint8* heapBuffer = new(nothrow) uint8[8192])
			{
			try
				{
				spCopyReadToWrite(heapBuffer, 8192,
					iStreamR, iStreamW, iCount,
					oCountRead, oCountWritten);
				}
			catch (...)
				{
				delete[] heapBuffer;
				throw;
				}
			delete[] heapBuffer;
			return;
			}
		}

	// We'll get to here if we need to move 8192 bytes or less, or if
	// allocation of the heap buffer failed.

	// Use a stack-based buffer if we're moving less than 8K and thus will iterate
	// fewer than 8 times. Previously we'd unconditionally used one of size 4096, but that's
	// fairly large and can contribute to blowing the stack on MacOS.
	uint8 localBuffer[sStackBufferSize];
	spCopyReadToWrite(localBuffer, sizeof(localBuffer),
		iStreamR, iStreamW, iCount,
		oCountRead, oCountWritten);
	}

static void spCopy(const ZStreamR& iStreamR,
	uint64 iChunkSize,
	const ZStreamW& iStreamW,
	size_t& oCountRead, size_t& oCountWritten)
	{
	try
		{
		if (iChunkSize > sStackBufferSize)
			{
			std::vector<uint8> buffer(iChunkSize, 0);
			iStreamR.Read(&buffer[0], buffer.size(), &oCountRead);
			iStreamW.Write(&buffer[0], oCountRead, &oCountWritten);
			return;
			}
		}
	catch (...)
		{}

	char buffer[sStackBufferSize];
	iStreamR.Read(buffer, sStackBufferSize, &oCountRead);
	iStreamW.Write(buffer, oCountRead, &oCountWritten);
	}

bool ZStream::sCopyAllCon
	(const ZStreamRCon& iStreamRCon,
	size_t iChunkSize,
	const ZStreamWCon& iStreamWCon,
	double iDisconnectTimeout)
	{
	size_t countRead, countWritten;
	spCopy(iStreamRCon, iChunkSize, iStreamWCon, countRead, countWritten);

	if (countRead == 0)
		{
		iStreamRCon.ReceiveDisconnect(iDisconnectTimeout);
		iStreamWCon.SendDisconnect();
		return false;
		}

	if (countRead != countWritten)
		{
		iStreamRCon.Abort();
		iStreamWCon.Abort();
		return false;
		}

	return true;
	}

ZStream::ExEndOfStream::ExEndOfStream(const char* iWhat)
:	range_error(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR

/**
\class ZStreamR
\ingroup Stream
\sa Stream

The ZStreamR interface is heavily used by ZooLib entities that need to read formatted and
unformatted data from arbitrary sources. It represents a continuous stream of bytes, with
no <em>a priori</em> known limit. Subclasses that implement this interface may know ahead
of time how much data will be readable, but this interface doesn't.
*/


/** \brief Read \a iCount bytes into memory starting at \a oDest.

\param oDest The address in memory at which read data is to be placed. It is the
caller's responsibilty to ensure that \a oDest to \a oDest + \a iCount is correctly
allocated and writeable.
\param iCount The number of bytes to read

If \a iCount bytes cannot be read from the stream an end of read stream exception
will be thrown.
*/
void ZStreamR::Read(void* oDest, size_t iCount) const
	{
	char* localDest = reinterpret_cast<char*>(oDest);
	size_t countRemaining = iCount;
	while (countRemaining > 0)
		{
		size_t countRead;
		const_cast<ZStreamR*>(this)->Imp_Read(localDest, countRemaining, &countRead);
		ZAssertStop(kDebug_Stream, countRead <= countRemaining);
		if (countRead == 0)
			sThrowEndOfStream();
		countRemaining -= countRead;
		localDest += countRead;
		}
	}

void ZStreamR::ReadAll(void* oDest, size_t iCount, size_t* oCountRead) const
	{
	char* localDest = reinterpret_cast<char*>(oDest);
	size_t countRemaining = iCount;
	while (countRemaining > 0)
		{
		size_t countRead;
		const_cast<ZStreamR*>(this)->Imp_Read(localDest, countRemaining, &countRead);
		ZAssertStop(kDebug_Stream, countRead <= countRemaining);
		if (countRead == 0)
			break;
		countRemaining -= countRead;
		localDest += countRead;
		}

	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<char*>(oDest);
	}

/** \brief Read data from this stream and write it
to \a iStreamW until this stream reaches its end.
*/
const ZStreamR& ZStreamR::CopyAllTo(const ZStreamW& iStreamW) const
	{
	uint64 countRead, countWritten;
	this->CopyAllTo(iStreamW, &countRead, &countWritten);
	if (countRead > countWritten)
		ZStreamW::sThrowEndOfStream();
	return *this;
	}


/**
\brief Read data from this stream and write it to
\a iStreamW until one or other stream reaches its end.

\param iStreamW The stream to which the data should be written.
\param oCountRead (optional output) The number of bytes that were actually read.
\param oCountWritten (optional output) The number of bytes that were actually written.

\a oCountWritten will be less than or equal to \a oCountRead. If \a oCountWritten is less
than \a oCountRead it's a strong indication that \a iStreamW has reached its end, and a
subsequent call to \c CopyTo or the \a iStreamW's \c Write will likely return zero in
\a oCountWritten.
*/
const ZStreamR& ZStreamR::CopyAllTo(const ZStreamW& iStreamW,
	uint64* oCountRead, uint64* oCountWritten) const
	{
	if (oCountRead)
		*oCountRead = 0;

	if (oCountWritten)
		*oCountWritten = 0;

	for (;;)
		{
		uint64 countRead, countWritten;
		this->CopyTo(iStreamW, 1024*1024*1024, &countRead, &countWritten);

		if (oCountRead)
			*oCountRead += countRead;

		if (oCountWritten)
			*oCountWritten += countWritten;

		if (countRead != countWritten || countRead == 0 || countWritten == 0)
			break;
		}

	return *this;
	}


/** \brief Read data from this stream and write it to \a iStreamW.

\param iStreamW The stream to which the data should be written.
\param iCount The number of bytes to be read from this stream and written to \a iStreamW.

If \a iCount bytes could not be read then an end of read stream exception is thrown. If
\a iCount bytes could not be written then an end of write stream exception is thrown.
*/
const ZStreamR& ZStreamR::CopyTo(const ZStreamW& iStreamW, uint64 iCount) const
	{
	uint64 countRead, countWritten;
	const_cast<ZStreamR*>(this)->Imp_CopyToDispatch(iStreamW, iCount, &countRead, &countWritten);
	if (countRead != iCount)
		ZStreamR::sThrowEndOfStream();
	if (countWritten != iCount)
		ZStreamW::sThrowEndOfStream();
	return *this;
	}


/** \brief Read data from this stream and write it to \a iStreamW.

\param iStreamW The stream to which the data should be written.
\param iCount The number of bytes to be read from this stream and written to \a iStreamW.
\param oCountRead (optional output) The number of bytes that were actually read.
\param oCountWritten (optional output) The number of bytes that were actually written.
*/
const ZStreamR& ZStreamR::CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten) const
	{
	const_cast<ZStreamR*>(this)->Imp_CopyToDispatch(iStreamW, iCount, oCountRead, oCountWritten);
	return *this;
	}


/** \brief Skip over \a iCount bytes.

\param iCount The number of bytes to be skipped over. If the stream reaches its
end prematurely then an end of read stream exception is thrown.
*/
void ZStreamR::Skip(uint64 iCount) const
	{
	uint64 actualCount;
	this->Skip(iCount, &actualCount);
	if (actualCount != iCount)
		sThrowEndOfStream();
	}


/** \brief Skip bytes until the end of the stream is reached, return the count actually skipped.

\param oCountSkipped (optional output) The number of bytes actually skipped. If zero
is returned then the end of the stream had already been reached.
*/
void ZStreamR::SkipAll(uint64* oCountSkipped) const
	{
	if (oCountSkipped)
		*oCountSkipped = 0;

	for (;;)
		{
		uint64 countSkipped;
		this->Skip(1024*1024*1024, &countSkipped);

		if (countSkipped == 0)
			break;

		if (oCountSkipped)
			*oCountSkipped += countSkipped;
		}
	}

/**
Read a single byte from the stream.
May throw an end of read stream exception.
*/
int8 ZStreamR::ReadInt8() const
	{
	int8 theVal;
	this->Read(&theVal, 1);
	return theVal;
	}

/**
Read a big endian 16 bit integer from the stream.
May throw an end of read stream exception.
*/
int16 ZStreamR::ReadInt16() const
	{
	int16 theVal;
	this->Read(&theVal, 2);
	return ZByteSwap_ReadBig16(&theVal);
	}

/**
Read a big endian 32 bit integer from the stream.
May throw an end of read stream exception.
*/
int32 ZStreamR::ReadInt32() const
	{
	int32 theVal;
	this->Read(&theVal, 4);
	return ZByteSwap_ReadBig32(&theVal);
	}

/**
Read a big endian 64 bit integer from the stream.
May throw an end of read stream exception.
*/
int64 ZStreamR::ReadInt64() const
	{
	int64 theVal;
	this->Read(&theVal, 8);
	return ZByteSwap_ReadBig64(&theVal);
	}

/**
Read a big endian IEEE754 single precision float from the stream.
May throw an end of read stream exception.
*/
float ZStreamR::ReadFloat() const
	{
	union { float f; int32 i; } u;
	u.i = this->ReadInt32();
	return u.f;
	}

/**
Read a big endian IEEE754 double precision float from the stream.
May throw an end of read stream exception.
*/
double ZStreamR::ReadDouble() const
	{
	union { double d; int64 i; } u;
	u.i = this->ReadInt64();
	return u.d;
	}

/**
Read a little endian 16 bit integer from the stream.
May throw an end of read stream exception.
*/
int16 ZStreamR::ReadInt16LE() const
	{
	int16 theVal;
	this->Read(&theVal, 2);
	return ZByteSwap_ReadLittle16(&theVal);
	}

/**
Read a little endian 32 bit integer from the stream.
May throw an end of read stream exception.
*/
int32 ZStreamR::ReadInt32LE() const
	{
	int32 theVal;
	this->Read(&theVal, 4);
	return ZByteSwap_ReadLittle32(&theVal);
	}

/**
Read a little endian 64 bit integer from the stream.
May throw an end of read stream exception.
*/
int64 ZStreamR::ReadInt64LE() const
	{
	int64 theVal;
	this->Read(&theVal, 8);
	return ZByteSwap_ReadLittle64(&theVal);
	}

/**
Read a little endian IEEE754 single precision float from the stream.
May throw an end of read stream exception.
*/
float ZStreamR::ReadFloatLE() const
	{
	union { float f; int32 i; } u;
	u.i = this->ReadInt32LE();
	return u.f;
	}

/**
Read a little endian IEEE754 double precision float from the stream.
May throw an end of read stream exception.
*/
double ZStreamR::ReadDoubleLE() const
	{
	union { double d; int64 i; } u;
	u.i = this->ReadInt64LE();
	return u.d;
	}

/**
Read a single byte from the stream, returning true if it is non-zero.
May throw an end of read stream exception.
*/
bool ZStreamR::ReadBool() const
	{
	int8 theVal;
	this->Read(&theVal, 1);
	return theVal != 0;
	}

/**
Read a single byte from the stream into \a oResult. If the stream
reached its end return false, otherwise return true.
*/
bool ZStreamR::ReadByte(uint8& oResult) const
	{
	size_t countRead;
	this->Read(&oResult, 1, &countRead);
	return countRead != 0;
	}

/**
Read a single byte from the stream into \a oResult. If the stream
reached its end return false, otherwise return true.
*/
bool ZStreamR::ReadChar(char& oResult) const
	{
	size_t countRead;
	this->Read(&oResult, 1, &countRead);
	return countRead != 0;
	}

/**
Read \a iSize bytes from the stream and return them in a string.
May throw an end of read stream exception.
*/
string ZStreamR::ReadString(size_t iSize) const
	{
	string theString(iSize, 0);
	if (iSize)
		this->Read(const_cast<char*>(theString.data()), iSize);
	return theString;
	}

/**
Read \a iSize bytes from the stream into \a oString. If successful then
return true. If fewer than \a iSize bytes can be read \a oString is
resized to match and false returned.
*/
bool ZStreamR::ReadString(size_t iSize, string& oString) const
	{
	oString = string(iSize, 0);
	if (iSize)
		{
		size_t countRead;
		this->Read(const_cast<char*>(oString.data()), iSize, &countRead);
		if (countRead != iSize)
			{
			oString.reserve(countRead);
			oString.resize(countRead);
			return false;
			}
		}
	return true;
	}

uint32 ZStreamR::ReadCount() const
	{
	uint8 firstByte = this->ReadUInt8();
	if (firstByte < 0xFF)
		return firstByte;
	return this->ReadUInt32();
	}

/** \brief Returns the number of bytes guaranteed to be readable without blocking

If the stream does not or cannot know this information it should return zero. A zero
return thus does not mean no data is available, simply that a subsequent Read or
Skip may block indefinitely.
 */
size_t ZStreamR::Imp_CountReadable()
	{ return 0;}

bool ZStreamR::Imp_WaitReadable(double iTimeout)
	{ return true; }

/** \brief Invoke \a iStreamW's \c Imp_CopyFrom method.

The first part of the two-stage dispatch initiated by ZStreamR::CopyTo and ZStreamR::CopyAllTo.
Override this method if your stream's data is in memory and thus can be written to \a iStreamW
by a single call to its \c Write method.
*/
void ZStreamR::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ const_cast<ZStreamW&>(iStreamW).Imp_CopyFrom(*this, iCount, oCountRead, oCountWritten); }


/** \brief Actually copy data from this stream to \a iStreamW.

The second part of the two-stage dispatch initiated by ZStreamW::CopyFrom and
ZStreamW::CopyAllFrom. The default implementation calls \c sCopyReadToWrite. Override this
method if your stream's data is in memory and thus can be written to \a iStreamW by a single
call to its \c Write method.
\sa sCopyReadToWrite
*/
void ZStreamR::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ ZStream::sCopyReadToWrite(*this, iStreamW, iCount, oCountRead, oCountWritten); }


/** \brief Read and discard iCount bytes.

Override this method if your stream implementation can skip past bytes in the stream
without actually reading them.
\param iCount Count of bytes to be skipped.
\param oCountSkipped Number of bytes that were actually skipped. If iCount is non zero and
oCountSkipped is set to zero then this indicates that the stream has reached its end.
*/
void ZStreamR::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 countRemaining = iCount;
	while (countRemaining)
		{
		size_t countRead;
		this->Read(sGarbageBuffer,
			min(countRemaining, uint64(sizeof(sGarbageBuffer))), &countRead);

		ZAssertStop(kDebug_Stream, countRead <= iCount);
		if (countRead == 0)
			break;
		countRemaining -= countRead;
		}
	if (oCountSkipped)
		*oCountSkipped = iCount - countRemaining;
	}

void ZStreamR::sThrowEndOfStream()
	{ throw ExEndOfStream(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR::ExEndOfStream

ZStreamR::ExEndOfStream::ExEndOfStream()
:	ZStream::ExEndOfStream("ZStreamR::ExEndOfStream")
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRCon

/**
\class ZStreamRCon
\ingroup Stream
*/

bool ZStreamRCon::ReceiveDisconnect(double iTimeout) const
	{ return const_cast<ZStreamRCon*>(this)->Imp_ReceiveDisconnect(iTimeout); }

void ZStreamRCon::Abort() const
	{ const_cast<ZStreamRCon*>(this)->Imp_Abort(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamU

/**
\class ZStreamU
\ingroup Stream
*/

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos

/**
\class ZStreamRPos
\ingroup Stream
A ZStreamRPos augments ZStreamW with two attributes, an immutable size and a position. The
position may take any value, but reads will only succeed when it is less then size.
As data is read the position is updated by the number of bytes consumed.
*/

size_t ZStreamRPos::Imp_CountReadable()
	{ return ZStream::sClampedSize(this->GetSize(), this->GetPosition()); }

bool ZStreamRPos::Imp_WaitReadable(double iTimeout)
	{
	return true; // Hmmm
	}

void ZStreamRPos::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	if (iCount)
		{
		uint64 curPos = this->GetPosition();
		uint64 size = this->GetSize();
		if (size > curPos)
			{
			if (uint64 realSkip = min(iCount, size - curPos))
				{
				this->SetPosition(curPos + realSkip);
				if (oCountSkipped)
					*oCountSkipped = realSkip;
				return;
				}
			}
		}

	if (oCountSkipped)
		*oCountSkipped = 0;
	}

void ZStreamRPos::Imp_Unread()
	{
	uint64 curPosition = this->GetPosition();
	ZAssertStop(kDebug_Stream, curPosition > 0);
	this->SetPosition(curPosition - 1);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW

/**
\class ZStreamW
\ingroup Stream
*/

/** \brief Write to the stream the \a iCount bytes starting at the address \a iSource.

\param iSource The address in memory from which data should be read. It is the caller's
responsibility to ensure that \c iSource to \c iSource + \c iCount are allocated and readable.
\param iCount The number of bytes to write.

If \a iCount bytes cannot be written to the stream an end of write stream exception
will be thrown.
*/
void ZStreamW::Write(const void* iSource, size_t iCount) const
	{
	const char* localSource = reinterpret_cast<const char*>(iSource);
	size_t countRemaining = iCount;
	while (countRemaining > 0)
		{
		size_t countWritten;
		const_cast<ZStreamW*>(this)->Imp_Write(localSource, countRemaining, &countWritten);

		ZAssertStopf(kDebug_Stream, countWritten <= countRemaining,
			("countWritten = %d, countRemaining = %d", countWritten, countRemaining));

		if (countWritten == 0)
			ZStreamW::sThrowEndOfStream();
		countRemaining -= countWritten;
		localSource += countWritten;
		}
	}


/** \brief Write to the stream the \a iCount bytes starting at the address
\a iSource, returning the count actually written.

\param iSource The address in memory from which data should be read. It is the caller's
responsibility to ensure that \c iSource to \c iSource + \c iCount are allocated and readable.
\param iCount The number of bytes to write.
\param oCountWritten (optional output) The number of bytes actually written. If iCount is
non zero and oCountWritten is zero this indicates that the stream has reached its end.
*/
void ZStreamW::Write(const void* iSource, size_t iCount, size_t* oCountWritten) const
	{
	if (oCountWritten)
		*oCountWritten = 0;
	const char* localSource = reinterpret_cast<const char*>(iSource);
	size_t countRemaining = iCount;
	while (countRemaining > 0)
		{
		size_t countWritten;
		const_cast<ZStreamW*>(this)->Imp_Write(localSource, countRemaining, &countWritten);
		ZAssertStop(kDebug_Stream, countWritten <= countRemaining);
		if (countWritten == 0)
			break;
		countRemaining -= countWritten;
		localSource += countWritten;
		if (oCountWritten)
			*oCountWritten += countWritten;
		}
	}


/** \brief Read data from \a iStreamR and write it to this stream until
one or the other reaches its end.
*/
const ZStreamW& ZStreamW::CopyAllFrom(const ZStreamR& iStreamR) const
	{
	uint64 countRead, countWritten;
	this->CopyAllFrom(iStreamR, &countRead, &countWritten);
	if (countRead > countWritten)
		ZStreamW::sThrowEndOfStream();
	return *this;
	}


/** \brief Read data from \a iStreamR and write it to this stream until
one or other reaches its end.

\param iStreamR The stream from which data should be read.
\param iCount The number of bytes to be read from \a iStreamR and written to this stream.
\param oCountRead (optional output) The number of bytes that were actually read.
\param oCountWritten (optional output) The number of bytes that were actually written.

\a oCountWritten will be less than or equal to \a oCountRead.
*/
const ZStreamW& ZStreamW::CopyAllFrom(const ZStreamR& iStreamR,
	uint64* oCountRead, uint64* oCountWritten) const
	{
	if (oCountRead)
		*oCountRead = 0;

	if (oCountWritten)
		*oCountWritten = 0;

	for (;;)
		{
		uint64 countRead, countWritten;
		this->CopyFrom(iStreamR, 1024*1024*1024, &countRead, &countWritten);

		if (oCountRead)
			*oCountRead += countRead;

		if (oCountWritten)
			*oCountWritten += countWritten;

		if (countRead != countWritten || countRead == 0 || countWritten == 0)
			break;
		}

	return *this;
	}


/** \brief Read data from \a iStreamR and write it to this stream.

\param iStreamR The stream from which data should be read.
\param iCount The number of bytes to be read from \a iStreamR and written to this stream.

If \a iCount bytes could not be read then an end of read stream exception is thrown. If
\a iCount bytes could not be written then an end of write stream exception is thrown.
*/
const ZStreamW& ZStreamW::CopyFrom(const ZStreamR& iStreamR, uint64 iCount) const
	{
	uint64 countRead, countWritten;
	this->CopyFrom(iStreamR, iCount, &countRead, &countWritten);
	if (countRead != iCount)
		ZStreamR::sThrowEndOfStream();
	if (countWritten != iCount)
		ZStreamW::sThrowEndOfStream();
	return *this;
	}


/** \brief Read data from \a iStreamR and write it to this stream.

\param iStreamR The stream from which data should be read.
\param iCount The number of bytes to be read from \a iStreamR and written to this stream.
\param oCountRead (optional output) The number of bytes that were actually read.
\param oCountWritten (optional output) The number of bytes that were actually written.
*/
const ZStreamW& ZStreamW::CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten) const
	{
	const_cast<ZStreamW*>(this)->Imp_CopyFromDispatch(iStreamR, iCount, oCountRead, oCountWritten);
	return *this;
	}

/**
Write a single byte to the stream.
May throw an end of write stream exception.
*/
void ZStreamW::WriteInt8(int8 iVal) const
	{ this->Write(&iVal, 1); }

/**
Write a 16 bit integer to the stream in big endian order.
May throw an end of write stream exception.
*/
void ZStreamW::WriteInt16(int16 iVal) const
	{
	ZByteSwap_HostToBig16(&iVal);
	this->Write(&iVal, 2);
	}

/**
Write a 32 bit integer to the stream in big endian order.
May throw an end of write stream exception.
*/
void ZStreamW::WriteInt32(int32 iVal) const
	{
	ZByteSwap_HostToBig32(&iVal);
	this->Write(&iVal, 4);
	}

/**
Write a 64 bit integer to the stream in big endian order.
May throw an end of write stream exception.
*/
void ZStreamW::WriteInt64(int64 iVal) const
	{
	ZByteSwap_HostToBig64(&iVal);
	this->Write(&iVal, 8);
	}

/**
Write a floating point number to the stream in a big endian IEEE754 single precision format.
May throw an end of write stream exception.
*/
void ZStreamW::WriteFloat(float iVal) const
	{
	union { float f; int32 i; } u;
	u.f = iVal;
	this->WriteInt32(u.i);
	}

/**
Write a floating point number to the stream in a big endian IEEE754 double precision format.
May throw an end of write stream exception.
*/
void ZStreamW::WriteDouble(double iVal) const
	{
	union { double d; int64 i; } u;
	u.d = iVal;
	this->WriteInt64(u.i);
	}

/**
Write a 16 bit integer to the stream in little endian order.
May throw an end of write stream exception.
*/
void ZStreamW::WriteInt16LE(int16 iVal) const
	{
	ZByteSwap_HostToLittle16(&iVal);
	this->Write(&iVal, 2);
	}

/**
Write a 32 bit integer to the stream in little endian order.
May throw an end of write stream exception.
*/
void ZStreamW::WriteInt32LE(int32 iVal) const
	{
	ZByteSwap_HostToLittle32(&iVal);
	this->Write(&iVal, 4);
	}

/**
Write a 64 bit integer to the stream in little endian order.
May throw an end of write stream exception.
*/
void ZStreamW::WriteInt64LE(int64 iVal) const
	{
	ZByteSwap_HostToLittle64(&iVal);
	this->Write(&iVal, 8);
	}

/**
Write a floating point number to the stream in a little endian IEEE754 single precision format.
May throw an end of write stream exception.
*/
void ZStreamW::WriteFloatLE(float iVal) const
	{
	union { float f; int32 i; } u;
	u.f = iVal;
	this->WriteInt32LE(u.i);
	}

/**
Write a floating point number to the stream in a little endian IEEE754 double precision format.
May throw an end of write stream exception.
*/
void ZStreamW::WriteDoubleLE(double iVal) const
	{
	union { double d; int64 i; } u;
	u.d = iVal;
	this->WriteInt64LE(u.i);
	}

/**
Write a non-zero byte to the stream if iVal is true, otherwise write a zero byte.
May throw an end of write stream exception.
*/
void ZStreamW::WriteBool(bool iVal) const
	{ this->WriteInt8(iVal ? 1 : 0); }

/**
Write the bytes pointed to by \a iString, up to but not including the terminating zero byte.
May throw an end of write stream exception.
*/
void ZStreamW::WriteString(const char* iString) const
	{
	if (iString)
		{
		if (size_t length = strlen(iString))
			this->Write(iString, length);
		}
	}

/**
Write the bytes contained in \a iString, with no terminating zero byte.
May throw an end of write stream exception.
*/
void ZStreamW::WriteString(const string& iString) const
	{
	if (size_t length = iString.size())
		this->Write(iString.data(), length);
	}


/**
Write the bytes pointed to by \a iString, up to but not including the terminating zero byte.
Standard printf-style parameter substitution is applied to the string before writing. The
number of bytes successfully written is returned.
*/
size_t ZStreamW::Writef(const char* iString, ...) const
	{
	string buffer(512, ' ');
	for (;;)
		{
		va_list args;
		va_start(args, iString);

		int count = vsnprintf(const_cast<char*>(buffer.data()), buffer.size(), iString, args);

		va_end(args);

		if (count < 0)
			{
			// Handle -ve result from glibc prior to version 2.1 by growing the string.
			buffer.resize(buffer.size() * 2);
			}
		else if (size_t(count) > buffer.size())
			{
			// Handle C99 standard, where count indicates how much space would have been needed.
			buffer.resize(count);
			}
		else
			{
			// The string fitted, we can now write it out.
			buffer.resize(count);
			this->WriteString(buffer);
			return count;
			}
		}
	}


/**
Write a single byte to the stream. If the stream
reached its end return false, otherwise return true.
*/
bool ZStreamW::WriteByte(uint8 iVal) const
	{
	size_t countWritten;
	const_cast<ZStreamW*>(this)->Imp_Write(&iVal, 1, &countWritten);
	return countWritten != 0;
	}


void ZStreamW::WriteCount(uint32 iCount) const
	{
	if (iCount < 0xFF)
		{
		this->WriteUInt8(iCount);
		}
	else
		{
		this->WriteUInt8(0xFF);
		this->WriteUInt32(iCount);
		}
	}

/** \brief Invoke \a iStreamR's \c Imp_CopyTo method.

The first part of the two-stage dispatch initiated by ZStreamW::CopyFrom and
ZStreamW::CopyAllFrom. Override if your stream's data is in memory and thus
\a iStreamR can have its \c Read method called with your stream's data as its destination.
*/
void ZStreamW::Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ const_cast<ZStreamR&>(iStreamR).Imp_CopyTo(*this, iCount, oCountRead, oCountWritten); }


/** \brief Actually copy data to this stream from \a iStreamR.

The second part of the two-stage dispatch initiated by ZStreamR::CopyTo and ZStreamR::CopyAllTo.
The default implementation calls \c sCopyReadToWrite. Override this method if your stream's data
is in memory and thus can be modified by calling \a iStreamR's \c Read method.
\sa sCopyReadToWrite
*/
void ZStreamW::Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ ZStream::sCopyReadToWrite(iStreamR, *this, iCount, oCountRead, oCountWritten); }


/** \brief If this stream buffers data then pass it on to its ultimate destination.

Some write streams buffer the data written to them, or are built atop a foundation that
may buffer data. Such streams should override \c Imp_Flush and push that buffered data
on to its ultimate destination. When the method returns the data may still not have reached
its ultimate destination, but is guaranteed to do at some point in the future with
no further action necessary on the caller's part.
*/
void ZStreamW::Flush() const
	{ const_cast<ZStreamW*>(this)->Imp_Flush(); }

void ZStreamW::Imp_Flush()
	{}

void ZStreamW::sThrowEndOfStream()
	{ throw ExEndOfStream(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW::ExEndOfStream

ZStreamW::ExEndOfStream::ExEndOfStream()
:	ZStream::ExEndOfStream("ZStreamW::ExEndOfStream")
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWCon

/**
\class ZStreamWCon
\ingroup Stream
*/

void ZStreamWCon::SendDisconnect() const
	{ const_cast<ZStreamWCon*>(this)->Imp_SendDisconnect(); }

void ZStreamWCon::Abort() const
	{ const_cast<ZStreamWCon*>(this)->Imp_Abort(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos

/**
\class ZStreamWPos
\ingroup Stream
A ZStreamWPos augments ZStreamW with two attributes, a size and a position.
*/

/** \brief Set the stream's size to match its current position.

The default implementation uses the stream's own \c SetSize and \c GetPosition
methods. Subclasses may be able to implement this more efficiently.
*/
void ZStreamWPos::Imp_Truncate()
	{ this->SetSize(this->GetPosition()); }

void ZStreamWPos::sThrowBadSize()
	{ throw ExBadSize(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos::ExBadSize

ZStreamWPos::ExBadSize::ExBadSize()
:	range_error("ZStreamWPos::ExBadSize")
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos

/**
\class ZStreamRWPos
\ingroup Stream

ZStreamRWPos inherits from both ZStreamRPos and ZStreamWPos without extending either protocol.
It serves as a standard designation for an entity that is considered to contain a list of bytes
that can be both read and written to. Calls to read and write are fulfilled at the current
position, and the current position (and size) is updated.
*/

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamU_Unreader

ZStreamU_Unreader::ZStreamU_Unreader(const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource),
	fStreamSourceU(dynamic_cast<ZStreamU*>(const_cast<ZStreamR*>(&iStreamSource))),
	fState(eStateFresh)
	{}

void ZStreamU_Unreader::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (fStreamSourceU)
		{
		fStreamSourceU->Imp_Read(oDest, iCount, oCountRead);
		return;
		}

	char* localDest = reinterpret_cast<char*>(oDest);
	char* localDestEnd = localDest + iCount;

	while (localDest < localDestEnd)
		{
		if (fState == eStateUnread)
			{
			*localDest++ = fChar;
			fState = eStateNormal;
			}
		else
			{
			size_t countRead;
			fStreamSource.Read(localDest, localDestEnd - localDest, &countRead);
			if (countRead == 0)
				break;
			localDest += countRead;
			}
		}

	if (iCount)
		{
		if (localDest == reinterpret_cast<char*>(oDest))
			{
			fState = eStateHitEnd;
			}
		else
			{
			fState = eStateNormal;
			fChar = localDest[-1];
			}
		}

	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<char*>(oDest);
	}

void ZStreamU_Unreader::Imp_Unread()
	{
	if (fStreamSourceU)
		{
		fStreamSourceU->Imp_Unread();
		return;
		}

	switch (fState)
		{
		case eStateFresh:
			{
			ZDebugStopf(2,
				("You called ZStreamU_Unreader::Imp_Unread without having read anything."));
			break;
			}
		case eStateNormal:
			{
			fState = eStateUnread;
			break;
			}
		case eStateUnread:
			{
			ZDebugStopf(2, ("You called ZStreamU_Unreader::Imp_Unread twice consecutively."));
			break;
			}
		case eStateHitEnd:
			{
			ZDebugStopf(2,
				("You called ZStreamU_Unreader::Imp_Unread when end of stream has been seen."));
			break;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Null

/**
\class ZStreamR_Null
\ingroup stream
*/

ZStreamR_Null::ZStreamR_Null()
	{}

ZStreamR_Null::~ZStreamR_Null()
	{}

void ZStreamR_Null::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;
	}

void ZStreamR_Null::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;
	}

void ZStreamR_Null::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;
	}

void ZStreamR_Null::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	if (oCountSkipped)
		*oCountSkipped = 0;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_Null

/**
\class ZStreamRPos_Null
\ingroup stream
ZStreamRPos_Null responds to all requests as if it had no content. Its size is always zero, and
any attempt to read, skip or copy from it will return zero as the count of bytes transferred.
*/

ZStreamRPos_Null::ZStreamRPos_Null()
:	fPosition(0)
	{}

ZStreamRPos_Null::~ZStreamRPos_Null()
	{}

void ZStreamRPos_Null::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;
	}

void ZStreamRPos_Null::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;
	}

void ZStreamRPos_Null::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;
	}

void ZStreamRPos_Null::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	if (oCountSkipped)
		*oCountSkipped = 0;
	}

uint64 ZStreamRPos_Null::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRPos_Null::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRPos_Null::Imp_GetSize()
	{ return 0; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Null

/**
\class ZStreamW_Null
\ingroup stream
ZStreamW_Null discards all data written to it, regardless of how that data is written.
It's useful as the sink of a write filter stream when you're only interested in the filter
stream's side effects.
*/

ZStreamW_Null::ZStreamW_Null()
	{}

ZStreamW_Null::~ZStreamW_Null()
	{}

void ZStreamW_Null::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = iCount;
	}

void ZStreamW_Null::Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	this->pCopyFrom(iStreamR, iCount, oCountRead, oCountWritten);
	}

void ZStreamW_Null::Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	this->pCopyFrom(iStreamR, iCount, oCountRead, oCountWritten);
	}

void ZStreamW_Null::pCopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	uint64 countSkipped;
	iStreamR.Skip(iCount, &countSkipped);
	if (oCountRead)
		*oCountRead = countSkipped;
	if (oCountWritten)
		*oCountWritten = countSkipped;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos_Null

ZStreamWPos_Null::ZStreamWPos_Null()
:	fSize(0),
	fPosition(0)
	{}

ZStreamWPos_Null::~ZStreamWPos_Null()
	{}

void ZStreamWPos_Null::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = iCount;

	if (iCount)
		{
		fPosition += iCount;
		fSize = max(fSize, fPosition);
		}
	}

void ZStreamWPos_Null::Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyFrom(iStreamR, iCount, oCountRead, oCountWritten); }

void ZStreamWPos_Null::Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyFrom(iStreamR, iCount, oCountRead, oCountWritten); }

uint64 ZStreamWPos_Null::Imp_GetPosition()
	{ return fPosition; }

void ZStreamWPos_Null::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamWPos_Null::Imp_GetSize()
	{ return fSize; }

void ZStreamWPos_Null::Imp_SetSize(uint64 iSize)
	{ fSize = iSize; }

void ZStreamWPos_Null::pCopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	uint64 countSkipped;
	iStreamR.Skip(iCount, &countSkipped);

	if (oCountRead)
		*oCountRead = countSkipped;
	if (oCountWritten)
		*oCountWritten = countSkipped;

	fPosition += countSkipped;
	fSize = max(fSize, fPosition);
	}

} // namespace ZooLib
