/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZYad_CFType__
#define __ZYad_CFType__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZYad.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZStream_CFData.h"

#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFDictionary.h>

#include <vector>

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_CFType

class ZYadR_CFType : public virtual ZYadR
	{
public:
	ZYadR_CFType(CFTypeRef iCFTypeRef);
	virtual ~ZYadR_CFType();

// Our protocol
	CFTypeRef GetCFTypeRef();

private:	
	CFTypeRef fCFTypeRef;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_CFType

class ZYadPrimR_CFType
:	public virtual ZYadR_CFType,
	public virtual ZYadPrimR
	{
public:
	ZYadPrimR_CFType(CFTypeRef iCFTypeRef);
	virtual ~ZYadPrimR_CFType();

// From ZYadR via ZYadPrimR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawRPos_CFData

typedef ZStreamerRPos_T<ZStreamRPos_CFData> ZStreamerRPos_CFData;

class ZYadRawRPos_CFData
:	public virtual ZYadR_CFType,
	public virtual ZYadRawR,
	public virtual ZStreamerRPos_CFData
	{
public:
	ZYadRawRPos_CFData(CFDataRef iCFDataRef);
	virtual ~ZYadRawRPos_CFData();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_CFArray

class ZYadListRPos_CFArray
:	public virtual ZYadR_CFType,
	public virtual ZYadListRPos
	{
public:
	ZYadListRPos_CFArray(CFArrayRef iCFArrayRef);
	virtual ~ZYadListRPos_CFArray();

// From ZYadR via ZYadListRPos
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadListR via ZYadListRPos
	virtual size_t GetPosition();

// From ZYadListRPos
	virtual size_t GetSize();
	virtual void SetPosition(size_t iPosition);

private:
	CFArrayRef fCFArrayRef;
	size_t fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapRPos_CFDictionary

class ZYadListMapRPos_CFDictionary
:	public virtual ZYadR_CFType,
	public ZYadListMapRPos
	{
public:
	ZYadListMapRPos_CFDictionary(CFDictionaryRef iCFDictionaryRef);
	virtual ~ZYadListMapRPos_CFDictionary();

// From ZYadR via ZYadListMapRPos
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadListR via ZYadListMapRPos
	virtual size_t GetPosition();

// From ZYadListRPos via ZYadListMapRPos
	virtual size_t GetSize();
	virtual void SetPosition(size_t iPosition);

// From ZYadMapR via ZYadListMapRPos
	virtual std::string Name();

// From ZYadMapRPos via ZYadListMapRPos
	virtual void SetPosition(const std::string& iName);

private:
	CFDictionaryRef fCFDictionaryRef;
	size_t fPosition;
	std::vector<CFStringRef> fNames;
	std::vector<CFTypeRef> fValues;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadUtil_CFType

namespace ZYadUtil_CFType {

ZRef<ZYadR> sMakeYadR(CFTypeRef iCFTypeRef);

CFTypeRef sFromYadR(ZRef<ZYadR>);

} // namespace ZYadUtil_CFType

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZYad_ZooLib__
