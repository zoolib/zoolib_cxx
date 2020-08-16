// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

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
				{
				if (errno != EINTR)
					break;
				clearerr(iFILE);
				}
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
				{
				if (errno != EINTR)
					break;
				clearerr(iFILE);
				}
			iCount -= countWritten;
			localSource += countWritten;
			}
		}
	return localSource - iSource;
	}

using std::range_error;

// =================================================================================================
#pragma mark - FILEHolder

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
#pragma mark - ChanR_Bin_FILE

ChanR_Bin_FILE::ChanR_Bin_FILE(FILE* iFILE)
:	FILEHolder(iFILE, false)
	{}

ChanR_Bin_FILE::ChanR_Bin_FILE(FILE* iFILE, bool iAdopt)
:	FILEHolder(iFILE, iAdopt)
	{}

size_t ChanR_Bin_FILE::Read(byte* oDest, size_t iCount)
	{ return spRead(fFILE, oDest, iCount); }

// =================================================================================================
#pragma mark - ChanW_Bin_FILE

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
