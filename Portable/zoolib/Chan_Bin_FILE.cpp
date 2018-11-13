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

#include "zoolib/Chan_Bin_FILE.h"
#include "zoolib/ZCONFIG_SPI.h"

#include <errno.h>
#include <stdexcept> // For range_error

// Neither the MSL headers nor the Windows headers have fseeko or ftello.

#if defined(_MSL_USING_MSL_C) || ZCONFIG_SPI_Enabled(Win)
	#define ZCONFIG_Has_fseeko 0
#else
	#define ZCONFIG_Has_fseeko 1
#endif

#if ZCONFIG_Has_fseeko
// To pick up the definition of BSD, if any. Can't do this
// when using CW and their headers (file doesn't exist).
	#include <sys/param.h>
#endif

namespace ZooLib {

static size_t spRead(FILE* iFILE, byte* oDest, size_t iCount)
	{
	byte* localDest = oDest;
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
	return localDest - oDest;
	}

static size_t spWrite(FILE* iFILE, const byte* iSource, size_t iCount)
	{
	const byte* localSource = iSource;
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
	return localSource - iSource;
	}

using std::range_error;

// =================================================================================================
#pragma mark -
#pragma mark FILEHolder

FILEHolder::FILEHolder(FILE* iFILE, bool iAdopt)
:	fFILE(iFILE)
,	fAdopted(iAdopt)
	{}

FILEHolder::~FILEHolder()
	{
	if (fFILE && fAdopted)
		fclose(fFILE);
	}

FILE* FILEHolder::GetFILE()
	{ return fFILE; }

FILE* FILEHolder::OrphanFILE()
	{ return sGetSet(fFILE, nullptr); }

// =================================================================================================
#pragma mark -
#pragma mark ChanR_Bin_FILE

ChanR_Bin_FILE::ChanR_Bin_FILE(FILE* iFILE)
:	FILEHolder(iFILE, false)
	{}

ChanR_Bin_FILE::ChanR_Bin_FILE(FILE* iFILE, bool iAdopt)
:	FILEHolder(iFILE, iAdopt)
	{}

size_t ChanR_Bin_FILE::Read(byte* oDest, size_t iCount)
	{ return spRead(fFILE, oDest, iCount); }

// =================================================================================================
#pragma mark -
#pragma mark ChanW_Bin_FILE

ChanW_Bin_FILE::ChanW_Bin_FILE(FILE* iFILE)
:	FILEHolder(iFILE, false)
	{}

ChanW_Bin_FILE::ChanW_Bin_FILE(FILE* iFILE, bool iAdopt)
:	FILEHolder(iFILE, iAdopt)
	{}

size_t ChanW_Bin_FILE::Write(const byte* iSource, size_t iCount)
	{ return spWrite(fFILE, iSource, iCount); }

void ChanW_Bin_FILE::Flush()
	{ fflush(fFILE); }

} // namespace ZooLib
