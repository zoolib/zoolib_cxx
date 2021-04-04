// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_Bin_string_h__
#define __ZooLib_Chan_Bin_string_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanW_Bin_string

class ChanW_Bin_string
:	public ChanW_Bin
	{
public:
	ChanW_Bin_string(std::string* ioString);

	virtual size_t Write(const byte* iSource, size_t iCount);

protected:
	std::string* fStringPtr;
	};

// =================================================================================================
#pragma mark - ChanRPos_Bin_string

class ChanRPos_Bin_string
:	public ChanRPos_Bin
	{
public:
	ChanRPos_Bin_string(const std::string& iString);

// From ChanPos
	virtual uint64 Pos();
	virtual void PosSet(uint64 iPos);

// From ChanR
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

// From ChanSize
	virtual uint64 Size();

// From ChanU
	virtual size_t Unread(const byte* iSource, size_t iCount);

private:
	const std::string fString;
	size_t fPosition;
	};

// =================================================================================================
#pragma mark - ChanRWPos_Bin_string

class ChanRWPos_Bin_string
:	public ChanRWPos<byte>
	{
public:
	ChanRWPos_Bin_string(std::string* ioStringPtr);

// From ChanPos
	virtual uint64 Pos();
	virtual void PosSet(uint64 iPos);

// From ChanR
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

// From ChanSize
	virtual uint64 Size();

// From ChanSizeSet
	virtual void SizeSet(uint64 iSize);

// From ChanU
	virtual size_t Unread(const byte* iSource, size_t iCount);

// From ChanW
	virtual size_t Write(const byte* iSource, size_t iCount);

private:
	std::string* fStringPtr;
	size_t fPosition;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Bin_string_h__
