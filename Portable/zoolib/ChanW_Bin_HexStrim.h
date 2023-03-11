// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanW_Bin_HexStrim_h__
#define __ZooLib_ChanW_Bin_HexStrim_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_Bin.h"
#include "zoolib/ChanW_UTF_InsertSeparator.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanW_Bin_HexStrim_Real

class ChanW_Bin_HexStrim_Real
:	public virtual ChanW_Bin
	{
public:
	ChanW_Bin_HexStrim_Real(bool iLowercaseHex, const ChanW_UTF& iChanW_UTF);

// From ChanW_Bin
	virtual size_t Write(const byte* iSource, size_t iCount);
	virtual void Flush();

protected:
	const ChanW_UTF& fChanW_UTF;
	const char* fHexDigits;
	};

// =================================================================================================
#pragma mark - ChanW_Bin_HexStrim

class ChanW_Bin_HexStrim
:	public virtual ChanW_Bin
	{
public:
	ChanW_Bin_HexStrim(const std::string& iByteSeparator,
		const std::string& iChunkSeparator, size_t iChunkSize,
		bool iLowercaseHex,
		const ChanW_UTF& iChanW_UTF);

	ChanW_Bin_HexStrim(const std::string& iByteSeparator,
		const std::string& iChunkSeparator, size_t iChunkSize,
		const ChanW_UTF& iChanW_UTF);

// From ChanW_Bin
	virtual size_t Write(const byte* iSource, size_t iCount);
	virtual void Flush();

protected:
	ChanW_UTF_InsertSeparator fChanW_UTF;
	ChanW_Bin_HexStrim_Real fChanW_Bin;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanW_Bin_HexStrim_h__
