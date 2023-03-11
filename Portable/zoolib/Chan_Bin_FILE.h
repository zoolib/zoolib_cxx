// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_Bin_FILE_h__
#define __ZooLib_Chan_Bin_FILE_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"

#include <stdio.h> // For FILE

namespace ZooLib {

// =================================================================================================
#pragma mark - FILEHolder

class FILEHolder
	{
public:
	FILEHolder(FILE* iFILE, bool iAdopt);
	~FILEHolder();

	FILE* GetFILE();
	FILE* OrphanFILE();

protected:
	FILE* fFILE;
	bool fAdopted;
	};

// =================================================================================================
#pragma mark - ChanR_Bin_FILE

/// A read stream that wraps a POSIX FILE*.

class ChanR_Bin_FILE
:	public virtual ChanR_Bin
,	public FILEHolder
	{
public:
	ChanR_Bin_FILE(FILE* iFILE);
	ChanR_Bin_FILE(FILE* iFILE, bool iAdopt);

// From ChanR_Bin
	virtual size_t Read(byte* oDest, size_t iCount);
	};

// =================================================================================================
#pragma mark - ChanW_Bin_FILE

/// A write stream that wraps a POSIX FILE*.

class ChanW_Bin_FILE
:	public virtual ChanW_Bin
,	public FILEHolder
	{
public:
	ChanW_Bin_FILE(FILE* iFILE);
	ChanW_Bin_FILE(FILE* iFILE, bool iAdopt);

// From ChanW_Bin
	virtual size_t Write(const byte* iSource, size_t iCount);
	virtual void Flush();
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Bin_FILE_h__
