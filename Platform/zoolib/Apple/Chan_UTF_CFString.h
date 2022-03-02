// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Chan_UTF_CFString_h__
#define __ZooLib_Apple_Chan_UTF_CFString_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/ZP.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include <CoreFoundation/CFString.h>

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_UTF_CFString

class ChanR_UTF_CFString : public virtual ChanR_UTF_Native16
	{
public:
	ChanR_UTF_CFString(CFStringRef iStringRef);
	~ChanR_UTF_CFString();

// From ChanR_UTF_Native16
	virtual void ReadUTF16(UTF16* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

private:
	ZP<CFStringRef> fStringRef;
	size_t fPosition;
	};

// =================================================================================================
#pragma mark - ChanW_UTF_CFString

class ChanW_UTF_CFString : public virtual ChanW_UTF_Native16
	{
public:
	ChanW_UTF_CFString(CFMutableStringRef iStringRef);
	~ChanW_UTF_CFString();

// From ChanW_UTF_Native16
	virtual size_t WriteUTF16(const UTF16* iSource, size_t iCountCU);

private:
	ZP<CFMutableStringRef> fStringRef;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZooLib_Apple_Chan_UTF_CFString_h__
