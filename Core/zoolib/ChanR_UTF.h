// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanR_UTF_h__
#define __ZooLib_ChanR_UTF_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_UTF.h"
#include "zoolib/ChanR.h"
#include "zoolib/UnicodeString.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

void sRead(const ChanR_UTF& iChanR,
	UTF32* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

void sRead(const ChanR_UTF& iChanR,
	UTF16* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

void sRead(const ChanR_UTF& iChanR,
	UTF8* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

// =================================================================================================
#pragma mark -

ZQ<string32> sQReadUTF32(const ChanR_UTF& iChanR, size_t iCountCP);
ZQ<string16> sQReadUTF16(const ChanR_UTF& iChanR, size_t iCountCP);
ZQ<string8> sQReadUTF8(const ChanR_UTF& iChanR, size_t iCountCP);

string32 sEReadUTF32(const ChanR_UTF& iChanR, size_t iCountCP);
string16 sEReadUTF16(const ChanR_UTF& iChanR, size_t iCountCP);
string8 sEReadUTF8(const ChanR_UTF& iChanR, size_t iCountCP);

string8 sReadAllUTF8(const ChanR_UTF& iChanR);

// =================================================================================================
#pragma mark - ChanR_UTF_Native16

class ChanR_UTF_Native16
:	public ChanR_UTF
	{
public:
// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCountCU);

// Our protocol
	virtual void ReadUTF16(UTF16* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP) = 0;
	};

// =================================================================================================
#pragma mark - ChanR_UTF_Native8

class ChanR_UTF_Native8
:	public ChanR_UTF
	{
public:
// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCountCU);

// Our protocol
	virtual void ReadUTF8(UTF8* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP) = 0;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_UTF_h__
