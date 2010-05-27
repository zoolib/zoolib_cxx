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

#include "zoolib/ZStream_POSIX.h"
#include "zoolib/ZCONFIG_SPI.h"

#include <errno.h>
#include <stdexcept> // for range_error

// Neither the MSL headers nor the Windows headers have fseeko or ftello.

#if defined(_MSL_USING_MSL_C) || ZCONFIG_SPI_Enabled(Win)
#	define ZCONFIG_Has_fseeko 0
#else
#	define ZCONFIG_Has_fseeko 1
#endif

#if ZCONFIG_Has_fseeko
// To pick up the definition of BSD, if any. Can't do this
// when using CW and their headers (file doesn't exist).
#	include <sys/param.h>
#endif

namespace ZooLib {

static void spRead(FILE* iFILE, void* oDest, size_t iCount, size_t* oCountRead)
	{
	char* localDest = reinterpret_cast<char*>(oDest);
	if (iFILE)
		{
		while (iCount)
			{
			size_t countRead = fread(localDest, 1, iCount, iFILE);
			if (countRead == 0)
				break;
			iCount -= countRead;
			localDest += countRead;
			}		
		}
	if (oCountRead)
		*oCountRead = reinterpret_cast<char*>(oDest) - localDest;
	}

static void spWrite(FILE* iFILE, const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const char* localSource = reinterpret_cast<const char*>(iSource);
	if (iFILE)
		{
		while (iCount)
			{
			size_t countWritten = fwrite(localSource, 1, iCount, iFILE);
			if (countWritten == 0)
				break;
			iCount -= countWritten;
			localSource += countWritten;
			}
		}
	if (oCountWritten)
		*oCountWritten = localSource - reinterpret_cast<const char*>(iSource);
	}

static uint64 spGetPosition(FILE* iFILE)
	{
	if (iFILE)
		{
		#if ZCONFIG_Has_fseeko
			off_t result = ftello(iFILE);
			if (result >= 0)
				return result;
		#else
			long result = ftell(iFILE);
			if (result >= 0)
				return result;
		#endif
		}
	return 0;
	}

static bool spSetPosition(FILE* iFILE, uint64 iPosition)
	{
	if (iFILE)
		{
		#if ZCONFIG_Has_fseeko
			return 0 < fseeko(iFILE, iPosition, SEEK_SET);
		#else
			if (iPosition != size_t(iPosition))
				return false;
			return 0 < fseek(iFILE, iPosition, SEEK_SET);
		#endif
		}
	return false;
	}

static uint64 spGetSize(FILE* iFILE)
	{
	if (iFILE)
		{
		#if ZCONFIG_Has_fseeko
			off_t oldPos = fseeko(iFILE, 0, SEEK_CUR);
			off_t endPos = fseeko(iFILE, 0, SEEK_END);
			fseeko(iFILE, oldPos, SEEK_SET);
			return endPos;
		#else
			long oldPos = fseek(iFILE, 0, SEEK_CUR);
			long endPos = fseek(iFILE, 0, SEEK_END);
			fseek(iFILE, oldPos, SEEK_SET);
			return endPos;
		#endif
		}
	return 0;
	}

// This one's difficult to support
//static void spSetSize(FILE* iFILE, uint64 iSize)

using std::range_error;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_FILE

ZStreamR_FILE::ZStreamR_FILE(FILE* iFILE)
:	fFILE(iFILE),
	fAdopted(false)
	{}

ZStreamR_FILE::ZStreamR_FILE(FILE* iFILE, bool iAdopt)
:	fFILE(iFILE),
	fAdopted(iAdopt)
	{}

ZStreamR_FILE::~ZStreamR_FILE()
	{
	if (fFILE && fAdopted)
		fclose(fFILE);
	}

void ZStreamR_FILE::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{ spRead(fFILE, oDest, iCount, oCountRead); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_FILE

ZStreamRPos_FILE::ZStreamRPos_FILE(FILE* iFILE)
:	fFILE(iFILE),
	fAdopted(false)
	{}

ZStreamRPos_FILE::ZStreamRPos_FILE(FILE* iFILE, bool iAdopt)
:	fFILE(iFILE),
	fAdopted(iAdopt)
	{}

ZStreamRPos_FILE::~ZStreamRPos_FILE()
	{
	if (fFILE && fAdopted)
		fclose(fFILE);
	}

void ZStreamRPos_FILE::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{ spRead(fFILE, oDest, iCount, oCountRead); }

uint64 ZStreamRPos_FILE::Imp_GetPosition()
	{ return spGetPosition(fFILE); }

void ZStreamRPos_FILE::Imp_SetPosition(uint64 iPosition)
	{
	if (!spSetPosition(fFILE, iPosition))
		throw range_error("ZStreamRPos_FILE::Imp_SetPosition");
	}

uint64 ZStreamRPos_FILE::Imp_GetSize()
	{ return spGetSize(fFILE); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_FILE

ZStreamW_FILE::ZStreamW_FILE(FILE* iFILE)
:	fFILE(iFILE),
	fAdopted(false)
	{}

ZStreamW_FILE::ZStreamW_FILE(FILE* iFILE, bool iAdopt)
:	fFILE(iFILE),
	fAdopted(iAdopt)
	{}

ZStreamW_FILE::~ZStreamW_FILE()
	{
	if (fFILE && fAdopted)
		fclose(fFILE);
	}

void ZStreamW_FILE::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{ spWrite(fFILE, iSource, iCount, oCountWritten); }

void ZStreamW_FILE::Imp_Flush()
	{
	if (fFILE)
		fflush(fFILE);
	}

// =================================================================================================
#pragma mark -
#pragma mark * FILE backed by a ZStream

#if defined(__USE_GNU)

static ssize_t spReadStreamR(void* iCookie, char* oDest, size_t iCount)
	{
	size_t countRead;
	static_cast<ZStreamR*>(iCookie)->Read(oDest, iCount, &countRead);
	return countRead;
	}

static ssize_t spWriteStreamW(void* iCookie, const char* iSource, size_t iCount)
	{
	size_t countWritten;
	static_cast<ZStreamW*>(iCookie)->Write(iSource, iCount, &countWritten);
	return countWritten;;
	}

static int64 spSeekStreamRPos(const ZStreamRPos& iStreamRPos, int64 iPos, int iWhence)
	{
	switch (iWhence)
		{
		case SEEK_SET:
			{
			iStreamRPos.SetPosition(iPos);
			return iStreamRPos.GetPosition();
			}
		case SEEK_CUR:
			{
			size_t newPos = iStreamRPos.GetPosition() + iPos;
			iStreamRPos.SetPosition(newPos);
			return newPos;
			}
		case SEEK_END:
			{
			size_t newPos = iStreamRPos.GetSize() + iPos;
			iStreamRPos.SetPosition(newPos);
			return newPos;
			}
		}
	ZUnimplemented();
	return -1;
	}

static int spSeekStreamRPos(void* iCookie, _IO_off64_t *iPos, int iWhence)
	{
	return spSeekStreamRPos(*static_cast<ZStreamRPos*>(iCookie), *iPos, iWhence);
	}

static ssize_t spReadStreamerR(void* iCookie, char* oDest, size_t iCount)
	{
	size_t countRead;
	static_cast<ZRef<ZStreamerR>*>(iCookie)[0]->GetStreamR().Read(oDest, iCount, &countRead);
	return countRead;
	}

static ssize_t spReadStreamerRPos(void* iCookie, char* oDest, size_t iCount)
	{
	size_t countRead;
	static_cast<ZRef<ZStreamerRPos>*>(iCookie)[0]->GetStreamR().Read(oDest, iCount, &countRead);
	return countRead;
	}

static int spSeekStreamerRPos(void* iCookie, _IO_off64_t *iPos, int iWhence)
	{
	return spSeekStreamRPos(
		static_cast<ZRef<ZStreamerRPos>*>(iCookie)[0]->GetStreamRPos(), *iPos, iWhence);
	}

static ssize_t spWriteStreamerW(void* iCookie, const char* iSource, size_t iCount)
	{
	size_t countWritten;
	static_cast<ZRef<ZStreamerW>*>(iCookie)[0]->GetStreamW().Write(iSource, iCount, &countWritten);
	return countWritten;;
	}

static int spCloseStreamerR(void* iCookie)
	{
	delete static_cast<ZRef<ZStreamerR>*>(iCookie);
	return 0;
	}

static int spCloseStreamerRPos(void* iCookie)
	{
	delete static_cast<ZRef<ZStreamerRPos>*>(iCookie);
	return 0;
	}

static int spCloseStreamerW(void* iCookie)
	{
	delete static_cast<ZRef<ZStreamerW>*>(iCookie);
	return 0;
	}

FILE* sStreamOpen(const ZStreamR& iStreamR)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = spReadStreamR;
	theFunctions.write = nullptr;
	theFunctions.seek = nullptr;
	theFunctions.close = nullptr;
	return ::fopencookie(const_cast<ZStreamR*>(&iStreamR), "", theFunctions);
	}

FILE* sStreamOpen(const ZStreamRPos& iStreamRPos)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = spReadStreamR;
	theFunctions.write = nullptr;
	theFunctions.seek = spSeekStreamRPos;
	theFunctions.close = nullptr;
	return ::fopencookie(const_cast<ZStreamRPos*>(&iStreamRPos), "", theFunctions);
	}

FILE* sStreamOpen(const ZStreamW& iStreamW)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = nullptr;
	theFunctions.write = spWriteStreamW;
	theFunctions.seek = nullptr;
	theFunctions.close = nullptr;
	return ::fopencookie(const_cast<ZStreamW*>(&iStreamW), "", theFunctions);
	}

FILE* sStreamerOpen(ZRef<ZStreamerR> iStreamerR)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = spReadStreamerR;
	theFunctions.write = nullptr;
	theFunctions.seek = nullptr;
	theFunctions.close = spCloseStreamerR;
	return ::fopencookie(new ZRef<ZStreamerR>(iStreamerR), "", theFunctions);
	}

FILE* sStreamerOpen(ZRef<ZStreamerRPos> iStreamerRPos)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = spReadStreamerRPos;
	theFunctions.write = nullptr;
	theFunctions.seek = spSeekStreamerRPos;
	theFunctions.close = spCloseStreamerRPos;
	return ::fopencookie(new ZRef<ZStreamerRPos>(iStreamerRPos), "", theFunctions);
	}

FILE* sStreamerOpen(ZRef<ZStreamerW> iStreamerW)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = nullptr;
	theFunctions.write = spWriteStreamerW;
	theFunctions.seek = nullptr;
	theFunctions.close = spCloseStreamerW;
	return ::fopencookie(new ZRef<ZStreamerW>(iStreamerW), "", theFunctions);
	}

#elif defined(BSD)

static int spReadStreamR(void* iCookie, char* oDest, int iCount)
	{
	size_t countRead;
	static_cast<ZStreamR*>(iCookie)->Read(oDest, iCount, &countRead);
	return countRead;
	}

static fpos_t spSeekStreamRPos(const ZStreamRPos& iStreamRPos, fpos_t iPos, int iWhence)
	{
	switch (iWhence)
		{
		case SEEK_SET:
			{
			iStreamRPos.SetPosition(iPos);
			return iStreamRPos.GetPosition();
			}
		case SEEK_CUR:
			{
			size_t newPos = iStreamRPos.GetPosition() + iPos;
			iStreamRPos.SetPosition(newPos);
			return newPos;
			}
		case SEEK_END:
			{
			size_t newPos = iStreamRPos.GetSize() + iPos;
			iStreamRPos.SetPosition(newPos);
			return newPos;
			}
		}
	ZUnimplemented();
	return -1;
	}

static fpos_t spSeekStreamRPos(void* iCookie, fpos_t iPos, int iWhence)
	{
	return spSeekStreamRPos(*static_cast<ZStreamRPos*>(iCookie), iPos, iWhence);
	}

static int spWriteStreamW(void* iCookie, const char* iSource, int iCount)
	{
	size_t countWritten;
	static_cast<ZStreamW*>(iCookie)->Write(iSource, iCount, &countWritten);
	return countWritten;;
	}

static int spReadStreamerR(void* iCookie, char* oDest, int iCount)
	{
	size_t countRead;
	static_cast<ZRef<ZStreamerR>*>(iCookie)[0]->GetStreamR().Read(oDest, iCount, &countRead);
	return countRead;
	}

static int spReadStreamerRPos(void* iCookie, char* oDest, int iCount)
	{
	size_t countRead;
	static_cast<ZRef<ZStreamerRPos>*>(iCookie)[0]->GetStreamR().Read(oDest, iCount, &countRead);
	return countRead;
	}

static fpos_t spSeekStreamerRPos(void* iCookie, fpos_t iPos, int iWhence)
	{
	return spSeekStreamRPos(
		static_cast<ZRef<ZStreamerRPos>*>(iCookie)[0]->GetStreamRPos(), iPos, iWhence);
	}

static int spWriteStreamerW(void* iCookie, const char* iSource, int iCount)
	{
	size_t countWritten;
	static_cast<ZRef<ZStreamerW>*>(iCookie)[0]->GetStreamW().Write(iSource, iCount, &countWritten);
	return countWritten;;
	}

static int spCloseStreamerR(void* iCookie)
	{
	delete static_cast<ZRef<ZStreamerR>*>(iCookie);
	return 0;
	}

static int spCloseStreamerRPos(void* iCookie)
	{
	delete static_cast<ZRef<ZStreamerRPos>*>(iCookie);
	return 0;
	}

static int spCloseStreamerW(void* iCookie)
	{
	delete static_cast<ZRef<ZStreamerW>*>(iCookie);
	return 0;
	}

FILE* sStreamOpen(const ZStreamR& iStreamR)
	{
	return ::funopen(const_cast<ZStreamR*>(&iStreamR), spReadStreamR, nullptr, nullptr, nullptr);
	}

FILE* sStreamOpen(const ZStreamRPos& iStreamRPos)
	{
	return ::funopen(
		const_cast<ZStreamRPos*>(&iStreamRPos), spReadStreamR, nullptr, spSeekStreamRPos, nullptr);
	}

FILE* sStreamOpen(const ZStreamW& iStreamW)
	{
	return ::funopen(const_cast<ZStreamW*>(&iStreamW), nullptr, spWriteStreamW, nullptr, nullptr);
	}

FILE* sStreamerOpen(ZRef<ZStreamerR> iStreamerR)
	{
	return ::funopen(
		new ZRef<ZStreamerR>(iStreamerR), spReadStreamerR, nullptr, nullptr, spCloseStreamerRPos);
	}

FILE* sStreamerOpen(ZRef<ZStreamerRPos> iStreamerRPos)
	{
	return ::funopen(
		new ZRef<ZStreamerRPos>(iStreamerRPos),
		spReadStreamerRPos, nullptr, spSeekStreamerRPos, spCloseStreamerRPos);
	}

FILE* sStreamerOpen(ZRef<ZStreamerW> iStreamerW)
	{
	return ::funopen(new ZRef<ZStreamerW>(iStreamerW),
		nullptr, spWriteStreamerW, nullptr, spCloseStreamerW);
	}

#else

FILE* sStreamOpen(const ZStreamR& iStreamR)
	{ return nullptr; }

FILE* sStreamOpen(const ZStreamRPos& iStreamRPos)
	{ return nullptr; }

FILE* sStreamOpen(const ZStreamW& iStreamW)
	{ return nullptr; }

FILE* sStreamerOpen(ZRef<ZStreamerR> iStreamerR)
	{ return nullptr; }

FILE* sStreamerOpen(ZRef<ZStreamerRPos> iStreamerRPos)
	{ return nullptr; }

FILE* sStreamerOpen(ZRef<ZStreamerW> iStreamerW)
	{ return nullptr; }

#endif

} // namespace ZooLib
