/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/ZStream_Indirect.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamR_Indirect

ZStreamR_Indirect::ZStreamR_Indirect(const ZStreamR* iStream)
:	ZStream_Indirect<ZStreamR>(iStream)
	{}

ZStreamR_Indirect::~ZStreamR_Indirect()
	{}

void ZStreamR_Indirect::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (fStream)
		fStream->Imp_Read(oDest, iCount, oCountRead);
	else if (oCountRead)
		*oCountRead = 0;
	}

size_t ZStreamR_Indirect::Imp_CountReadable()
	{
	if (fStream)
		return fStream->Imp_CountReadable();
	else
		return 0;
	}

bool ZStreamR_Indirect::Imp_WaitReadable(double iTimeout)
	{
	if (fStream)
		return fStream->Imp_WaitReadable(iTimeout);
	return true;
	}

void ZStreamR_Indirect::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	if (fStream)
		fStream->Imp_Skip(iCount, oCountSkipped);
	else if (oCountSkipped)
		*oCountSkipped = 0;
	}

void ZStreamR_Indirect::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (fStream)
		{
		fStream->Imp_CopyToDispatch(iStreamW, iCount, oCountRead, oCountWritten);
		}
	else
		{
		if (oCountRead)
			*oCountRead = 0;
		if (oCountWritten)
			*oCountWritten = 0;
		}
	}

void ZStreamR_Indirect::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (fStream)
		{
		fStream->Imp_CopyTo(iStreamW, iCount, oCountRead, oCountWritten);
		}
	else
		{
		if (oCountRead)
			*oCountRead = 0;
		if (oCountWritten)
			*oCountWritten = 0;
		}
	}

// =================================================================================================
// MARK: - ZStreamRCon_Indirect

ZStreamRCon_Indirect::ZStreamRCon_Indirect(const ZStreamRCon* iStream)
:	ZStream_Indirect<ZStreamRCon>(iStream)
	{}

ZStreamRCon_Indirect::~ZStreamRCon_Indirect()
	{}

void ZStreamRCon_Indirect::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (fStream)
		fStream->Imp_Read(oDest, iCount, oCountRead);
	else if (oCountRead)
		*oCountRead = 0;
	}

size_t ZStreamRCon_Indirect::Imp_CountReadable()
	{
	if (fStream)
		return fStream->Imp_CountReadable();
	else
		return 0;
	}

bool ZStreamRCon_Indirect::Imp_WaitReadable(double iTimeout)
	{
	if (fStream)
		return fStream->Imp_WaitReadable(iTimeout);
	return true;
	}

void ZStreamRCon_Indirect::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	if (fStream)
		fStream->Imp_Skip(iCount, oCountSkipped);
	else if (oCountSkipped)
		*oCountSkipped = 0;
	}

bool ZStreamRCon_Indirect::Imp_ReceiveDisconnect(double iTimeout)
	{
	if (fStream)
		return fStream->Imp_ReceiveDisconnect(iTimeout);
	return true;
	}

void ZStreamRCon_Indirect::Imp_Abort()
	{
	if (fStream)
		fStream->Imp_Abort();
	}

// =================================================================================================
// MARK: - ZStreamU_Indirect

ZStreamU_Indirect::ZStreamU_Indirect(const ZStreamU* iStream)
:	ZStream_Indirect<ZStreamU>(iStream)
	{}

ZStreamU_Indirect::~ZStreamU_Indirect()
	{}

void ZStreamU_Indirect::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (fStream)
		fStream->Imp_Read(oDest, iCount, oCountRead);
	else if (oCountRead)
		*oCountRead = 0;
	}

size_t ZStreamU_Indirect::Imp_CountReadable()
	{
	if (fStream)
		return fStream->Imp_CountReadable();
	else
		return 0;
	}

bool ZStreamU_Indirect::Imp_WaitReadable(double iTimeout)
	{
	if (fStream)
		return fStream->Imp_WaitReadable(iTimeout);
	return true;
	}

void ZStreamU_Indirect::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	if (fStream)
		fStream->Imp_Skip(iCount, oCountSkipped);
	else if (oCountSkipped)
		*oCountSkipped = 0;
	}

void ZStreamU_Indirect::Imp_Unread()
	{
	if (fStream)
		return fStream->Imp_Unread();
	}

// =================================================================================================
// MARK: - ZStreamRPos_Indirect

ZStreamRPos_Indirect::ZStreamRPos_Indirect(const ZStreamRPos* iStream)
:	ZStream_Indirect<ZStreamRPos>(iStream)
	{}

ZStreamRPos_Indirect::~ZStreamRPos_Indirect()
	{}

void ZStreamRPos_Indirect::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (fStream)
		fStream->Imp_Read(oDest, iCount, oCountRead);
	else if (oCountRead)
		*oCountRead = 0;
	}

size_t ZStreamRPos_Indirect::Imp_CountReadable()
	{
	if (fStream)
		return fStream->Imp_CountReadable();
	else
		return 0;
	}

void ZStreamRPos_Indirect::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	if (fStream)
		fStream->Imp_Skip(iCount, oCountSkipped);
	else if (oCountSkipped)
		*oCountSkipped = 0;
	}

uint64 ZStreamRPos_Indirect::Imp_GetPosition()
	{
	if (fStream)
		return fStream->Imp_GetPosition();
	return 0;
	}

void ZStreamRPos_Indirect::Imp_SetPosition(uint64 iPosition)
	{
	if (fStream)
		return fStream->Imp_SetPosition(iPosition);
	}

uint64 ZStreamRPos_Indirect::Imp_GetSize()
	{
	if (fStream)
		return fStream->Imp_GetSize();
	return 0;
	}

// =================================================================================================
// MARK: - ZStreamW_Indirect

ZStreamW_Indirect::ZStreamW_Indirect(const ZStreamW* iStream)
:	ZStream_Indirect<ZStreamW>(iStream)
	{}

ZStreamW_Indirect::~ZStreamW_Indirect()
	{}

void ZStreamW_Indirect::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (fStream)
		fStream->Imp_Write(iSource, iCount, oCountWritten);
	else if (oCountWritten)
		*oCountWritten = 0;
	}

void ZStreamW_Indirect::Imp_Flush()
	{
	if (fStream)
		fStream->Imp_Flush();
	}

// =================================================================================================
// MARK: - ZStreamWCon_Indirect

ZStreamWCon_Indirect::ZStreamWCon_Indirect(const ZStreamWCon* iStream)
:	ZStream_Indirect<ZStreamWCon>(iStream)
	{}

ZStreamWCon_Indirect::~ZStreamWCon_Indirect()
	{}

void ZStreamWCon_Indirect::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (fStream)
		fStream->Imp_Write(iSource, iCount, oCountWritten);
	else if (oCountWritten)
		*oCountWritten = 0;
	}

void ZStreamWCon_Indirect::Imp_Flush()
	{
	if (fStream)
		fStream->Imp_Flush();
	}

void ZStreamWCon_Indirect::Imp_SendDisconnect()
	{
	if (fStream)
		fStream->Imp_SendDisconnect();
	}

void ZStreamWCon_Indirect::Imp_Abort()
	{
	if (fStream)
		fStream->Imp_Abort();
	}

// =================================================================================================
// MARK: - ZStreamWPos_Indirect

ZStreamWPos_Indirect::ZStreamWPos_Indirect(const ZStreamWPos* iStream)
:	ZStream_Indirect<ZStreamWPos>(iStream)
	{}

ZStreamWPos_Indirect::~ZStreamWPos_Indirect()
	{}

void ZStreamWPos_Indirect::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (fStream)
		fStream->Imp_Write(iSource, iCount, oCountWritten);
	else if (oCountWritten)
		*oCountWritten = 0;
	}

void ZStreamWPos_Indirect::Imp_Flush()
	{
	if (fStream)
		fStream->Imp_Flush();
	}

uint64 ZStreamWPos_Indirect::Imp_GetPosition()
	{
	if (fStream)
		return fStream->Imp_GetPosition();
	return 0;
	}

void ZStreamWPos_Indirect::Imp_SetPosition(uint64 iPosition)
	{
	if (fStream)
		return fStream->Imp_SetPosition(iPosition);
	}

uint64 ZStreamWPos_Indirect::Imp_GetSize()
	{
	if (fStream)
		return fStream->Imp_GetSize();
	return 0;
	}

void ZStreamWPos_Indirect::Imp_SetSize(uint64 iSize)
	{
	if (fStream)
		return fStream->Imp_SetSize(iSize);
	}

// =================================================================================================
// MARK: - ZStreamRWPos_Indirect

ZStreamRWPos_Indirect::ZStreamRWPos_Indirect(const ZStreamRWPos* iStream)
:	ZStream_Indirect<ZStreamRWPos>(iStream)
	{}

ZStreamRWPos_Indirect::~ZStreamRWPos_Indirect()
	{}

void ZStreamRWPos_Indirect::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (fStream)
		fStream->Imp_Read(oDest, iCount, oCountRead);
	else if (oCountRead)
		*oCountRead = 0;
	}

size_t ZStreamRWPos_Indirect::Imp_CountReadable()
	{
	if (fStream)
		return fStream->Imp_CountReadable();
	else
		return 0;
	}

void ZStreamRWPos_Indirect::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	if (fStream)
		fStream->Imp_Skip(iCount, oCountSkipped);
	else if (oCountSkipped)
		*oCountSkipped = 0;
	}

void ZStreamRWPos_Indirect::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (fStream)
		fStream->Imp_Write(iSource, iCount, oCountWritten);
	else if (oCountWritten)
		*oCountWritten = 0;
	}

void ZStreamRWPos_Indirect::Imp_Flush()
	{
	if (fStream)
		fStream->Imp_Flush();
	}

uint64 ZStreamRWPos_Indirect::Imp_GetPosition()
	{
	if (fStream)
		return fStream->Imp_GetPosition();
	return 0;
	}

void ZStreamRWPos_Indirect::Imp_SetPosition(uint64 iPosition)
	{
	if (fStream)
		return fStream->Imp_SetPosition(iPosition);
	}

uint64 ZStreamRWPos_Indirect::Imp_GetSize()
	{
	if (fStream)
		return fStream->Imp_GetSize();
	return 0;
	}

void ZStreamRWPos_Indirect::Imp_SetSize(uint64 iSize)
	{
	if (fStream)
		return fStream->Imp_SetSize(iSize);
	}

} // namespace ZooLib
