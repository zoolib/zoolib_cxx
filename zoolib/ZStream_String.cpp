/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZStream_String.h"

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h" // For ZMemCopy

using std::min;
using std::string;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_String

ZStreamRPos_String::ZStreamRPos_String(const string& inString)
:	fString(inString),
	fPosition(0)
	{}

ZStreamRPos_String::~ZStreamRPos_String()
	{}

void ZStreamRPos_String::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (size_t countToRead = ZStream::sClampedSize(iCount, fString.size(), fPosition))
		{
		ZMemCopy(oDest, &fString.at(fPosition), countToRead);
		fPosition += countToRead;
		if (oCountRead)
			*oCountRead = countToRead;
		}
	else if (oCountRead)
		{
		*oCountRead = 0;
		}
	}

uint64 ZStreamRPos_String::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRPos_String::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRPos_String::Imp_GetSize()
	{ return fString.size(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos_String

ZStreamWPos_String::ZStreamWPos_String(string& inString)
:	fString(inString),
	fPosition(0)
	{}

ZStreamWPos_String::~ZStreamWPos_String()
	{}

void ZStreamWPos_String::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t neededSpace = ZStream::sClampedSize(fPosition + iCount);
	if (fString.size() < neededSpace)
		{
		try
			{
			fString.resize(neededSpace);
			}
		catch (...)
			{}
		}

	if (size_t countToWrite = ZStream::sClampedSize(iCount, fString.size(), fPosition))
		{
		ZMemCopy(&fString.at(fPosition), iSource, countToWrite);
		fPosition += countToWrite;
		if (oCountWritten)
			*oCountWritten = countToWrite;
		}
	else if (oCountWritten)
		{
		*oCountWritten = 0;
		}
	}

uint64 ZStreamWPos_String::Imp_GetPosition()
	{ return fPosition; }

void ZStreamWPos_String::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamWPos_String::Imp_GetSize()
	{ return fString.size(); }

void ZStreamWPos_String::Imp_SetSize(uint64 iSize)
	{
	size_t realSize = iSize;
	if (realSize != iSize)
		sThrowBadSize();

	fString.resize(realSize);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_String

ZStreamRWPos_String::ZStreamRWPos_String(string& inString)
:	fString(inString),
	fPosition(0)
	{}

ZStreamRWPos_String::~ZStreamRWPos_String()
	{}

void ZStreamRWPos_String::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (size_t countToRead = ZStream::sClampedSize(iCount, fString.size(), fPosition))
		{
		ZMemCopy(oDest, &fString.at(fPosition), countToRead);
		fPosition += countToRead;
		if (oCountRead)
			*oCountRead = countToRead;
		}
	else if (oCountRead)
		{
		*oCountRead = 0;
		}
	}

void ZStreamRWPos_String::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t neededSpace = ZStream::sClampedSize(fPosition + iCount);
	if (fString.size() < neededSpace)
		{
		try
			{
			fString.resize(neededSpace);
			}
		catch (...)
			{}
		}

	if (size_t countToMove = ZStream::sClampedSize(iCount, fString.size(), fPosition))
		{
		ZMemCopy(&fString.at(fPosition), iSource, countToMove);
		fPosition += countToMove;
		if (oCountWritten)
			*oCountWritten = countToMove;
		}
	else if (oCountWritten)
		{
		*oCountWritten = 0;
		}
	}

void ZStreamRWPos_String::Imp_Flush()
	{}

uint64 ZStreamRWPos_String::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRWPos_String::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRWPos_String::Imp_GetSize()
	{ return fString.size(); }

void ZStreamRWPos_String::Imp_SetSize(uint64 iSize)
	{
	size_t realSize = iSize;
	if (realSize != iSize)
		sThrowBadSize();

	fString.resize(realSize);
	}

NAMESPACE_ZOOLIB_END
