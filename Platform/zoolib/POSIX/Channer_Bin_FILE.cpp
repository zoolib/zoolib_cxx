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

#include "zoolib/POSIX/Channer_Bin_FILE.h"

#include "zoolib/ZCONFIG_SPI.h" // For ZCONFIG_SPI_Enabled(BSD)

namespace ZooLib {

// =================================================================================================
#pragma mark - FILE backed by a Channer

#if defined(__USE_GNU)

static ssize_t spReadR(void* iCookie, char* oDest, size_t iCount)
	{ return sReadMem(*static_cast<ChannerR_Bin*>(iCookie), oDest, iCount); }

static ssize_t spReadRPos(void* iCookie, char* oDest, size_t iCount)
	{ return sReadMem(*static_cast<ChannerRPos_Bin*>(iCookie), oDest, iCount); }

static ssize_t spWrite(void* iCookie, const char* iSource, size_t iCount)
	{ return sWriteMem(*static_cast<ChannerW_Bin*>(iCookie), iSource, iCount); }

static int spSeek(void* iCookie, _IO_off64_t *iPos, int iWhence)
	{
	ChannerRPos_Bin* theChannerRPos = static_cast<ChannerRPos_Bin*>(iCookie);
	switch (iWhence)
		{
		case SEEK_SET:
			{
			sPosSet(*theChannerRPos, *iPos);
			return 0;
			}
		case SEEK_CUR:
			{
			_IO_off64_t newPos = sPos(*theChannerRPos) + *iPos;
			sPosSet(*theChannerRPos, newPos);
			return 0;
			}
		case SEEK_END:
			{
			_IO_off64_t newPos = sSize(*theChannerRPos) + *iPos;
			sPosSet(*theChannerRPos, newPos);
			return 0;
			}
		}
	ZUnimplemented();
	return -1;
	}

static int spCloseR(void* iCookie)
	{
	static_cast<ChannerR_Bin*>(iCookie)->Release();
	return 0;
	}

static int spCloseRPos(void* iCookie)
	{
	static_cast<ChannerRPos_Bin*>(iCookie)->Release();
	return 0;
	}

static int spCloseW(void* iCookie)
	{
	static_cast<ChannerW_Bin*>(iCookie)->Release();
	return 0;
	}

FILE* sFILE(ZRef<ChannerR_Bin> iChannerR)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = spReadR;
	theFunctions.write = nullptr;
	theFunctions.seek = nullptr;
	theFunctions.close = spCloseR;
	return ::fopencookie(iChannerR.Get(), "", theFunctions);
	}

FILE* sFILE(ZRef<ChannerRPos_Bin> iChannerRPos)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = spReadRPos;
	theFunctions.write = nullptr;
	theFunctions.seek = spSeek;
	theFunctions.close = spCloseRPos;
	return ::fopencookie(iChannerRPos.Get(), "", theFunctions);
	}

FILE* sFILE(ZRef<ChannerW_Bin> iChannerW)
	{
	_IO_cookie_io_functions_t theFunctions;
	theFunctions.read = nullptr;
	theFunctions.write = spWrite;
	theFunctions.seek = nullptr;
	theFunctions.close = spCloseW;
	return ::fopencookie(iChannerW.Get(), "", theFunctions);
	}

#elif ZCONFIG_SPI_Enabled(BSD)

static int spReadR(void* iCookie, char* oDest, int iCount)
	{ return sReadMem(*static_cast<ChannerR_Bin*>(iCookie), oDest, iCount); }

static int spReadRPos(void* iCookie, char* oDest, int iCount)
	{ return sReadMem(*static_cast<ChannerRPos_Bin*>(iCookie), oDest, iCount); }

static int spWrite(void* iCookie, const char* iSource, int iCount)
	{ return sWriteMem(*static_cast<ChannerW_Bin*>(iCookie), iSource, iCount); }

static fpos_t spSeek(void* iCookie, fpos_t iPos, int iWhence)
	{
	ChannerRPos_Bin* theChannerRPos = static_cast<ChannerRPos_Bin*>(iCookie);
	switch (iWhence)
		{
		case SEEK_SET:
			{
			sPosSet(*theChannerRPos, iPos);
			return iPos;
			}
		case SEEK_CUR:
			{
			fpos_t newPos = sPos(*theChannerRPos) + iPos;
			sPosSet(*theChannerRPos, newPos);
			return newPos;
			}
		case SEEK_END:
			{
			fpos_t newPos = sSize(*theChannerRPos) + iPos;
			sPosSet(*theChannerRPos, newPos);
			return newPos;
			}
		}
	ZUnimplemented();
	return -1;
	}

static int spCloseR(void* iCookie)
	{
	static_cast<ChannerR_Bin*>(iCookie)->Release();
	return 0;
	}

static int spCloseRPos(void* iCookie)
	{
	static_cast<ChannerRPos_Bin*>(iCookie)->Release();
	return 0;
	}

static int spCloseW(void* iCookie)
	{
	static_cast<ChannerW_Bin*>(iCookie)->Release();
	return 0;
	}

FILE* sFILE(ZRef<ChannerR_Bin> iChannerR)
	{
	if (iChannerR)
		{
		iChannerR->Retain();
		return ::funopen(iChannerR.Get(), spReadR, nullptr, nullptr, spCloseR);
		}
	return nullptr;
	}

FILE* sFILE(ZRef<ChannerRPos_Bin> iChannerRPos)
	{
	if (iChannerRPos)
		{
		iChannerRPos->Retain();
		return ::funopen(iChannerRPos.Get(), spReadRPos, nullptr, spSeek, spCloseRPos);
		}
	return nullptr;
	}

FILE* sFILE(ZRef<ChannerW_Bin> iChannerW)
	{
	if (iChannerW)
		{
		iChannerW->Retain();
		return ::funopen(iChannerW.Get(), nullptr, spWrite, nullptr, spCloseW);
		}
	return nullptr;
	}

#else

FILE* sFILE(ZRef<ChannerR_Bin> iChannerR)
	{ return nullptr; }

FILE* sFILE(ZRef<ChannerRPos_Bin> iChannerRPos)
	{ return nullptr; }

FILE* sFILE(ZRef<ChannerW_Bin> iChannerW)
	{ return nullptr; }

#endif

} // namespace ZooLib
