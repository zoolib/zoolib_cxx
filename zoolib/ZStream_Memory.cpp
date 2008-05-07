/* ------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZStream_Memory.h"
#include "ZMemory.h" // For ZBlockMove & ZBlockCopy

using std::min;
using std::pair;

//#define kDebug_Stream_Memory 2

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Memory

/**
\class ZStreamR_Memory
\ingroup stream
ZStreamR_Memory provides read stream access to normal memory. The memory is only referenced by the
stream, no copy of it is taken, and no attempt is made to dispose it or otherwise interpret what
kind of memory it might be. Access to the memory is unbounded. If you need to be able to set a
limit on the range of bytes accessed, use ZStreamRPos_Memory.
\sa ZStreamRPos_Memory
*/

/**
\param iAddress The address from which to start reading data.
*/
ZStreamR_Memory::ZStreamR_Memory(const void* iAddress)
:	fAddress(static_cast<const char*>(iAddress))
	{}

void ZStreamR_Memory::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	ZBlockCopy(fAddress, iDest, iCount);
	fAddress += iCount;
	if (oCountRead)
		*oCountRead = iCount;
	}

void ZStreamR_Memory::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyTo(iStreamW, iCount, oCountRead, oCountWritten); }

void ZStreamR_Memory::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyTo(iStreamW, iCount, oCountRead, oCountWritten); }

void ZStreamR_Memory::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	fAddress += iCount;
	if (oCountSkipped)
		*oCountSkipped = iCount;
	}

const void* ZStreamR_Memory::GetCurrentAddress()
	{ return fAddress; }

void ZStreamR_Memory::pCopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;
	while (iCount)
		{
		size_t countWritten;
		iStreamW.Write(fAddress, iCount, &countWritten);
		if (countWritten == 0)
			break;
		fAddress += countWritten;
		iCount -= countWritten;
		if (oCountRead)
			*oCountRead += countWritten;
		if (oCountWritten)
			*oCountWritten += countWritten;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_Memory

/**
\class ZStreamRPos_Memory
\ingroup stream
ZStreamRPos_Memory provides positionable read stream access to normal memory. If you need to pass a
ZStreamR to a method, and want that stream to be sourced from normal memory but want to restrict
the range of addresses accessed you should use a ZStreamRPos_Memory, even though its positionable
API is not directly required.
\sa ZStreamR_Memory
*/

/**
\param iAddress The base address from which data is to be read.
\param iSize The size of the block of memory to which access should be restricted.
*/
ZStreamRPos_Memory::ZStreamRPos_Memory(const void* iAddress, size_t iSize)
:	fAddress(static_cast<const char*>(iAddress)),
	fPosition(0),
	fSize(iSize)
	{}

/**
\param iParam A pair containing the base address from which to read and the size of the block of
memory to which access should be restricted. Useful when used in a template situtation where only
one parameter can be passed through the template API.
*/
ZStreamRPos_Memory::ZStreamRPos_Memory(const pair<const void*, size_t>& iParam)
:	fAddress(static_cast<const char*>(iParam.first)),
	fPosition(0),
	fSize(iParam.second)
	{}

void ZStreamRPos_Memory::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countToMove = min(uint64(iCount), sDiffPosR(fSize, fPosition));
	ZBlockCopy(fAddress + fPosition, iDest, countToMove);
	fPosition += countToMove;
	if (oCountRead)
		*oCountRead = countToMove;
	}

size_t ZStreamRPos_Memory::Imp_CountReadable()
	{ return fSize - fPosition; }

void ZStreamRPos_Memory::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyTo(iStreamW, iCount, oCountRead, oCountWritten); }

void ZStreamRPos_Memory::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyTo(iStreamW, iCount, oCountRead, oCountWritten); }

void ZStreamRPos_Memory::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 realSkip = min(iCount, sDiffPosR(fSize, fPosition));
	fPosition += realSkip;
	if (oCountSkipped)
		*oCountSkipped = realSkip;
	}

uint64 ZStreamRPos_Memory::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRPos_Memory::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRPos_Memory::Imp_GetSize()
	{ return fSize; }

const void* ZStreamRPos_Memory::GetCurrentAddress()
	{ return fAddress + fPosition; }

void ZStreamRPos_Memory::Reset(const void* iAddress, size_t iSize)
	{
	fAddress = static_cast<const char*>(iAddress);
	fPosition = 0;
	fSize = iSize;
	}

void ZStreamRPos_Memory::pCopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;
	while (iCount)
		{
		uint64 countToWrite = min(uint64(iCount), sDiffPosR(fSize, fPosition));
		size_t countWritten;
		iStreamW.Write(fAddress + fPosition, countToWrite, &countWritten);
		if (countWritten == 0)
			break;
		fPosition += countWritten;
		iCount -= countWritten;
		if (oCountRead)
			*oCountRead += countWritten;
		if (oCountWritten)
			*oCountWritten += countWritten;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_Memory

ZStreamerRPos_Memory::ZStreamerRPos_Memory(const void* iAddress, size_t iSize)
:	fStream(iAddress, iSize)
	{}

ZStreamerRPos_Memory::~ZStreamerRPos_Memory()
	{}

const ZStreamRPos& ZStreamerRPos_Memory::GetStreamRPos()
	{ return fStream; }

const void* ZStreamerRPos_Memory::GetCurrentAddress()
	{ return fStream.GetCurrentAddress(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Memory

/**
\class ZStreamW_Memory
\ingroup stream
ZStreamW_Memory provides write stream access to normal memory. The memory is only referenced by
the stream, no copy of it is taken, and no attempt is made to dispose it or otherwise interpret
what kind of memory it might be. The caller is responsible for passing an address to which write
access is permitted. The stream does not bound its accesses, it just writes the data it's passed
and works its way through memory. If you need bounded access use a ZStreamWPos_Memory.
\sa ZStreamWPos_Memory
*/

ZStreamW_Memory::ZStreamW_Memory(void* iAddress)
:	fAddress(static_cast<char*>(iAddress))
	{}

void ZStreamW_Memory::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	ZBlockCopy(iSource, fAddress, iCount);
	fAddress += iCount;
	if (oCountWritten)
		*oCountWritten = iCount;
	}

void ZStreamW_Memory::Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	this->pCopyFrom(iStreamR, iCount, oCountRead, oCountWritten);
	}

void ZStreamW_Memory::Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	this->pCopyFrom(iStreamR, iCount, oCountRead, oCountWritten);
	}

void* ZStreamW_Memory::GetCurrentAddress()
	{ return fAddress; }

void ZStreamW_Memory::pCopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;
	while (iCount)
		{
		size_t countRead;
		iStreamR.Read(fAddress, iCount, &countRead);
		if (countRead == 0)
			break;

		fAddress += countRead;
		iCount -= countRead;

		if (oCountRead)
			*oCountRead += countRead;
		if (oCountWritten)
			*oCountWritten += countRead;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos_Memory

/**
\class ZStreamWPos_Memory
\ingroup stream
ZStreamWPos_Memory provides positionable write stream access to normal memory. If you need to pass a
ZStreamW to a method, and want that stream to be sourced from normal memory but want to restrict
the range of addresses accessed you should use a ZStreamWPos_Memory, even though its positionable
API is not directly required.

\sa ZStreamW_Memory
*/

/**
\param iAddress The base address to which data is to be written.
\param iSize Both the capacity of the block of memory to which access should be
restricted and the nominal starting size of the stream.
*/
ZStreamWPos_Memory::ZStreamWPos_Memory(void* iAddress, size_t iSize)
:	fAddress(static_cast<char*>(iAddress)),
	fSize(iSize),
	fCapacity(iSize),
	fPosition(0)
	{}

/**
\param iAddress The base address to which data is to be written.
\param iSize The nominal starting size of the stream (can be zero).
\param iCapacity The capacity of the block of memory and thus the maximum
size to which the stream can grow.
*/
ZStreamWPos_Memory::ZStreamWPos_Memory(void* iAddress, size_t iSize, size_t iCapacity)
:	fAddress(static_cast<char*>(iAddress)),
	fSize(iSize),
	fCapacity(iCapacity),
	fPosition(0)
	{}


void ZStreamWPos_Memory::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t countToMove = min(uint64(iCount), sDiffPosW(fCapacity, fPosition));
	ZBlockCopy(iSource, fAddress + fPosition, countToMove);
	fPosition += countToMove;
	if (fSize < fPosition)
		fSize = fPosition;
	if (oCountWritten)
		*oCountWritten = countToMove;
	}

void ZStreamWPos_Memory::Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	this->pCopyFrom(iStreamR, iCount, oCountRead, oCountWritten);
	}

void ZStreamWPos_Memory::Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	this->pCopyFrom(iStreamR, iCount, oCountRead, oCountWritten);
	}

uint64 ZStreamWPos_Memory::Imp_GetPosition()
	{ return fPosition; }

void ZStreamWPos_Memory::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamWPos_Memory::Imp_GetSize()
	{ return fSize; }

void ZStreamWPos_Memory::Imp_SetSize(uint64 iSize)
	{
	if (iSize > fCapacity)
		sThrowBadSize();
	fSize = iSize;
	}

void* ZStreamWPos_Memory::GetCurrentAddress()
	{ return fAddress + fPosition; }

void ZStreamWPos_Memory::pCopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;
	while (iCount)
		{
		uint64 countToMove = min(iCount, sDiffPosW(fCapacity, fPosition));
		size_t countRead;
		iStreamR.Read(fAddress + fPosition, countToMove, &countRead);
		if (countRead == 0)
			break;

		fPosition += countRead;
		if (fSize < fPosition)
			fSize = fPosition;
		iCount -= countRead;

		if (oCountRead)
			*oCountRead += countRead;
		if (oCountWritten)
			*oCountWritten += countRead;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWPos_Memory

ZStreamerWPos_Memory::ZStreamerWPos_Memory(void* iAddress, size_t iSize)
:	fStream(iAddress, iSize)
	{}

ZStreamerWPos_Memory::ZStreamerWPos_Memory(void* iAddress, size_t iSize, size_t iCapacity)
:	fStream(iAddress, iSize, iCapacity)
	{}

ZStreamerWPos_Memory::~ZStreamerWPos_Memory()
	{}

const ZStreamWPos& ZStreamerWPos_Memory::GetStreamWPos()
	{ return fStream; }

void* ZStreamerWPos_Memory::GetCurrentAddress()
	{ return fStream.GetCurrentAddress(); }
