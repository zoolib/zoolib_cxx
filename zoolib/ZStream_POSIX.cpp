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

#include "ZStream_POSIX.h"
#include "ZCONFIG_SPI.h"

#include <errno.h>
#include <stdexcept> // for range_error

using std::range_error;

// Neither the MSL headers nor the Windows headers have fseeko or ftello.

#if defined(_MSL_USING_MW_C_HEADERS)
#	define ZCONFIG_Has_fseeko (!_MSL_USING_MW_C_HEADERS)
#else
#	define ZCONFIG_Has_fseeko (!ZCONFIG_SPI_Enabled(Win))
#endif

#if !defined(_MSL_USING_MW_C_HEADERS) || !_MSL_USING_MW_C_HEADERS
// To pick up the definition of BSD, if any. Can't do this
// when using CW and their headers (file doesn't exist).
#	include <sys/param.h>
#endif

static void sRead(FILE* iFILE, void* iDest, size_t iCount, size_t* oCountRead)
	{
	char* localDest = reinterpret_cast<char*>(iDest);
	if (iFILE)
		{
		while (iCount)
			{
			size_t countRead = fread(localDest, 1,  iCount, iFILE);
			if (countRead == 0)
				break;
			iCount -= countRead;
			localDest += countRead;
			}		
		}
	if (oCountRead)
		*oCountRead = reinterpret_cast<char*>(iDest) - localDest;
	}

static void sWrite(FILE* iFILE, const void* iSource, size_t iCount, size_t* oCountWritten)
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

static uint64 sGetPosition(FILE* iFILE)
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

static bool sSetPosition(FILE* iFILE, uint64 iPosition)
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

static uint64 sGetSize(FILE* iFILE)
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
//static void sSetSize(FILE* iFILE, uint64 iSize)

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

void ZStreamR_FILE::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{ ::sRead(fFILE, iDest, iCount, oCountRead); }

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

void ZStreamRPos_FILE::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{ ::sRead(fFILE, iDest, iCount, oCountRead); }

uint64 ZStreamRPos_FILE::Imp_GetPosition()
	{ return ::sGetPosition(fFILE); }

void ZStreamRPos_FILE::Imp_SetPosition(uint64 iPosition)
	{
	if (!::sSetPosition(fFILE, iPosition))
		throw range_error("ZStreamRPos_FILE::Imp_SetPosition");
	}

uint64 ZStreamRPos_FILE::Imp_GetSize()
	{ return ::sGetSize(fFILE); }

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
	{ ::sWrite(fFILE, iSource, iCount, oCountWritten); }

void ZStreamW_FILE::Imp_Flush()
	{
	if (fFILE)
		fflush(fFILE);
	}

// =================================================================================================
#pragma mark -
#pragma mark * FILE backed by a ZStream

#if defined(__USE_GNU)

static ssize_t sReadStreamR(void* iCookie, char* iDest, size_t iCount)
	{
	size_t countRead;
	static_cast<ZStreamR*>(iCookie)->Read(iDest, iCount, &countRead);
	return countRead;
	}

static ssize_t sWriteStreamW(void* iCookie, const char* iSource, size_t iCount)
	{
	size_t countWritten;
	static_cast<ZStreamW*>(iCookie)->Write(iSource, iCount, &countWritten);
	return countWritten;;
	}

static int64 sSeekStreamRPos(const ZStreamRPos& iStreamRPos, int64 iPos, int iWhence)
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

static int sSeekStreamRPos(void* iCookie, _IO_off64_t *iPos, int iWhence)
	{
	return sSeekStreamRPos(*static_cast<ZStreamRPos*>(iCookie), *iPos, iWhence);
	}

static ssize_t sReadStreamerR(void* iCookie, char* iDest, size_t iCount)
	{
	size_t countRead;
	static_cast<ZRef<ZStreamerR>*>(iCookie)[0]->GetStreamR().Read(iDest, iCount, &countRead);
	return countRead;
	}

static ssize_t sReadStreamerRPos(void* iCookie, char* iDest, size_t iCount)
	{
	size_t countRead;
	static_cast<ZRef<ZStreamerRPos>*>(iCookie)[0]->GetStreamR().Read(iDest, iCount, &countRead);
	return countRead;
	}

static int sSeekStreamerRPos(void* iCookie, _IO_off64_t *iPos, int iWhence)
	{
	return sSeekStreamRPos(static_cast<ZRef<ZStreamerRPos>*>(iCookie)[0]->GetStreamRPos(), *iPos, iWhence);
	}

static ssize_t sWriteStreamerW(void* iCookie, const char* iSource, size_t iCount)
	{
	size_t countWritten;
	static_cast<ZRef<ZStreamerW>*>(iCookie)[0]->GetStreamW().Write(iSource, iCount, &countWritten);
	return countWritten;;
	}

static int sCloseStreamerR(void* iCookie)
	{
	delete static_cast<ZRef<ZStreamerR>*>(iCookie);
	return 0;
	}

static int sCloseStreamerRPos(void* iCookie)
	{
	delete static_cast<ZRef<ZStreamerRPos>*>(iCookie);
	return 0;
	}

static int sCloseStreamerW(void* iCookie)
	{
	delete static_cast<ZRef<ZStreamerW>*>(iCookie);
	return 0;
	}

FILE* ZooLib::sStreamOpen(const ZStreamR& iStreamR)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = sReadStreamR;
	theFunctions.write = nil;
	theFunctions.seek = nil;
	theFunctions.close = nil;
	return ::fopencookie(const_cast<ZStreamR*>(&iStreamR), "", theFunctions);
	}

FILE* ZooLib::sStreamOpen(const ZStreamRPos& iStreamRPos)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = sReadStreamR;
	theFunctions.write = nil;
	theFunctions.seek = sSeekStreamRPos;
	theFunctions.close = nil;
	return ::fopencookie(const_cast<ZStreamRPos*>(&iStreamRPos), "", theFunctions);
	}

FILE* ZooLib::sStreamOpen(const ZStreamW& iStreamW)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = nil;
	theFunctions.write = sWriteStreamW;
	theFunctions.seek = nil;
	theFunctions.close = nil;
	return ::fopencookie(const_cast<ZStreamW*>(&iStreamW), "", theFunctions);
	}

FILE* ZooLib::sStreamerOpen(ZRef<ZStreamerR> iStreamerR)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = sReadStreamerR;
	theFunctions.write = nil;
	theFunctions.seek = nil;
	theFunctions.close = sCloseStreamerR;
	return ::fopencookie(new ZRef<ZStreamerR>(iStreamerR), "", theFunctions);
	}

FILE* ZooLib::sStreamerOpen(ZRef<ZStreamerRPos> iStreamerRPos)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = sReadStreamerRPos;
	theFunctions.write = nil;
	theFunctions.seek = sSeekStreamerRPos;
	theFunctions.close = sCloseStreamerRPos;
	return ::fopencookie(new ZRef<ZStreamerRPos>(iStreamerRPos), "", theFunctions);
	}

FILE* ZooLib::sStreamerOpen(ZRef<ZStreamerW> iStreamerW)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = nil;
	theFunctions.write = sWriteStreamerW;
	theFunctions.seek = nil;
	theFunctions.close = sCloseStreamerW;
	return ::fopencookie(new ZRef<ZStreamerW>(iStreamerW), "", theFunctions);
	}

#elif defined(BSD)

static int sReadStreamR(void* iCookie, char* iDest, int iCount)
	{
	size_t countRead;
	static_cast<ZStreamR*>(iCookie)->Read(iDest, iCount, &countRead);
	return countRead;
	}

static fpos_t sSeekStreamRPos(const ZStreamRPos& iStreamRPos, fpos_t iPos, int iWhence)
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

static fpos_t sSeekStreamRPos(void* iCookie, fpos_t iPos, int iWhence)
	{
	return sSeekStreamRPos(*static_cast<ZStreamRPos*>(iCookie), iPos, iWhence);
	}

static int sWriteStreamW(void* iCookie, const char* iSource, int iCount)
	{
	size_t countWritten;
	static_cast<ZStreamW*>(iCookie)->Write(iSource, iCount, &countWritten);
	return countWritten;;
	}

static int sReadStreamerR(void* iCookie, char* iDest, int iCount)
	{
	size_t countRead;
	static_cast<ZRef<ZStreamerR>*>(iCookie)[0]->GetStreamR().Read(iDest, iCount, &countRead);
	return countRead;
	}

static int sReadStreamerRPos(void* iCookie, char* iDest, int iCount)
	{
	size_t countRead;
	static_cast<ZRef<ZStreamerRPos>*>(iCookie)[0]->GetStreamR().Read(iDest, iCount, &countRead);
	return countRead;
	}

static fpos_t sSeekStreamerRPos(void* iCookie, fpos_t iPos, int iWhence)
	{
	return sSeekStreamRPos(static_cast<ZRef<ZStreamerRPos>*>(iCookie)[0]->GetStreamRPos(), iPos, iWhence);
	}

static int sWriteStreamerW(void* iCookie, const char* iSource, int iCount)
	{
	size_t countWritten;
	static_cast<ZRef<ZStreamerW>*>(iCookie)[0]->GetStreamW().Write(iSource, iCount, &countWritten);
	return countWritten;;
	}

static int sCloseStreamerR(void* iCookie)
	{
	delete static_cast<ZRef<ZStreamerR>*>(iCookie);
	return 0;
	}

static int sCloseStreamerRPos(void* iCookie)
	{
	delete static_cast<ZRef<ZStreamerRPos>*>(iCookie);
	return 0;
	}

static int sCloseStreamerW(void* iCookie)
	{
	delete static_cast<ZRef<ZStreamerW>*>(iCookie);
	return 0;
	}

FILE* ZooLib::sStreamOpen(const ZStreamR& iStreamR)
	{ return ::funopen(const_cast<ZStreamR*>(&iStreamR), sReadStreamR, nil, nil, nil); }

FILE* ZooLib::sStreamOpen(const ZStreamRPos& iStreamRPos)
	{ return ::funopen(const_cast<ZStreamRPos*>(&iStreamRPos), sReadStreamR, nil, sSeekStreamRPos, nil); }

FILE* ZooLib::sStreamOpen(const ZStreamW& iStreamW)
	{ return ::funopen(const_cast<ZStreamW*>(&iStreamW), nil, sWriteStreamW, nil, nil); }

FILE* ZooLib::sStreamerOpen(ZRef<ZStreamerR> iStreamerR)
	{ return ::funopen(new ZRef<ZStreamerR>(iStreamerR), sReadStreamerR, nil, nil, sCloseStreamerRPos); }

FILE* ZooLib::sStreamerOpen(ZRef<ZStreamerRPos> iStreamerRPos)
	{ return ::funopen(new ZRef<ZStreamerRPos>(iStreamerRPos), sReadStreamerRPos, nil, sSeekStreamerRPos, sCloseStreamerRPos); }

FILE* ZooLib::sStreamerOpen(ZRef<ZStreamerW> iStreamerW)
	{ return ::funopen(new ZRef<ZStreamerW>(iStreamerW), nil, sWriteStreamerW, nil, sCloseStreamerW); }

#else

FILE* ZooLib::sStreamOpen(const ZStreamR& iStreamR)
	{ return nil; }

FILE* ZooLib::sStreamOpen(const ZStreamRPos& iStreamRPos)
	{ return nil; }

FILE* ZooLib::sStreamOpen(const ZStreamW& iStreamW)
	{ return nil; }

FILE* ZooLib::sStreamerOpen(ZRef<ZStreamerR> iStreamerR)
	{ return nil; }

FILE* ZooLib::sStreamerOpen(ZRef<ZStreamerRPos> iStreamerRPos)
	{ return nil; }

FILE* ZooLib::sStreamerOpen(ZRef<ZStreamerW> iStreamerW)
	{ return nil; }

#endif
