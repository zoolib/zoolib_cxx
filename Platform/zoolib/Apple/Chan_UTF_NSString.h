// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Chan_UTF_NSString_h__
#define __ZooLib_Apple_Chan_UTF_NSString_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/ZP.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include <Foundation/NSString.h>

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_UTF_NSString

class ChanR_UTF_NSString : public virtual ChanR_UTF
	{
public:
	ChanR_UTF_NSString(NSString* iString);
	~ChanR_UTF_NSString();

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

private:
	ZP<NSString> fString;
	size_t fPosition;
	};

// =================================================================================================
#pragma mark - ChanW_UTF_NSString

class ChanW_UTF_NSString : public virtual ChanW_UTF_Native16
	{
public:
	ChanW_UTF_NSString(NSMutableString* ioString);
	~ChanW_UTF_NSString();

// From ChanW_UTF_Native16
	virtual size_t WriteUTF16(const UTF16* iSource, size_t iCountCU);

private:
	ZP<NSMutableString> fString;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZooLib_Apple_Chan_UTF_NSString_h__
