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

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZStream_CFData.h"
#include "zoolib/ZStrim_CFString.h"
#include "zoolib/ZVal_CFType.h"
#include "zoolib/ZYad_Val_T.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_CFType

typedef ZYadR_Val_T<ZRef<CFTypeRef> > ZYadR_CFType;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_CFType

class ZYadPrimR_CFType
:	public ZYadR_CFType
,	public ZYadPrimR
	{
public:
	ZYadPrimR_CFType(const ZRef<CFTypeRef>& iVal);

// From ZYadPrimR
	virtual ZAny AsAny();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamRPos_CFType

typedef ZStreamerRPos_T<ZStreamRPos_CFData> ZStreamerRPos_CFType;

class ZYadStreamRPos_CFType
:	public ZYadR_CFType
,	public ZYadStreamR
,	public ZStreamerRPos_CFType
	{
public:
	ZYadStreamRPos_CFType(const ZRef<CFDataRef>& iData);
	virtual ~ZYadStreamRPos_CFType();

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_CFType

typedef ZStrimmerR_T<ZStrimR_CFString> ZStrimmerR_CFString;

class ZYadStrimR_CFType
:	public ZYadR_CFType
,	public ZYadStrimR
,	public ZStrimmerR_CFString
	{
public:
	ZYadStrimR_CFType(const ZRef<CFStringRef>& iStringRef);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqRPos_CFType

class ZYadSeqRPos_CFType
:	public ZYadR_CFType
,	public ZYadSeqRPos_Val_Self_T<ZYadSeqRPos_CFType, ZSeq_CFType>
	{
public:
	ZYadSeqRPos_CFType(const ZRef<CFArrayRef>& iArray);
	ZYadSeqRPos_CFType(const ZRef<CFArrayRef>& iArray, uint64 iPosition);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_CFType

class ZYadMapRPos_CFType
:	public ZYadR_CFType
,	public ZYadMapRPos
	{
	ZYadMapRPos_CFType(const ZRef<CFDictionaryRef>& iDictionary,
		uint64 iPosition,
		const std::vector<CFStringRef>& iNames,
		const std::vector<CFTypeRef>& iValues);

public:
	ZYadMapRPos_CFType(const ZRef<CFDictionaryRef>& iDictionary);

// From ZYadMapR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(std::string& oName);

// From ZYadMapRPos
	virtual ZRef<ZYadMapRPos> Clone();

	virtual void SetPosition(const std::string& iName);

private:
	const ZRef<CFDictionaryRef> fDictionary;
	uint64 fPosition;
	std::vector<CFStringRef> fNames;
	std::vector<CFTypeRef> fValues;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const ZRef<CFTypeRef>& iVal);

ZRef<ZYadStrimR> sMakeYadR(const ZRef<CFMutableStringRef>& iString);
ZRef<ZYadStrimR> sMakeYadR(const ZRef<CFStringRef>& iString);

ZRef<ZYadStreamR> sMakeYadR(const ZRef<CFMutableDataRef>& iData);
ZRef<ZYadStreamR> sMakeYadR(const ZRef<CFDataRef>& iData);

ZRef<ZYadSeqRPos> sMakeYadR(const ZRef<CFMutableArrayRef>& iArray);
ZRef<ZYadSeqRPos> sMakeYadR(const ZRef<CFArrayRef>& iArray);

ZRef<ZYadMapRPos> sMakeYadR(const ZRef<CFMutableDictionaryRef>& iDictionary);
ZRef<ZYadMapRPos> sMakeYadR(const ZRef<CFDictionaryRef>& iDictionary);

// =================================================================================================
#pragma mark -
#pragma mark * sFromYadR

ZRef<CFTypeRef> sFromYadR(const ZRef<CFTypeRef>& iDefault, ZRef<ZYadR> iYadR);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZYad_CFType__
