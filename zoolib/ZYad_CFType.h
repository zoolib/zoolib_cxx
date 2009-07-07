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
#include "zoolib/ZStrim_CFString.h"
#include "zoolib/ZVal_CFType.h"

#include <vector>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_CFType

class ZYadR_CFType : public virtual ZYadR
	{
public:
	ZYadR_CFType(ZRef<CFTypeRef> iCFTypeRef);
	virtual ~ZYadR_CFType();

// Our protocol
	const ZRef<CFTypeRef>& GetVal();

private:
	const ZRef<CFTypeRef> fVal;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamRPos_CFType

typedef ZStreamerRPos_T<ZStreamRPos_CFData> ZStreamerRPos_CFType;

class ZYadStreamRPos_CFType
:	public ZYadR_CFType,
	public ZYadStreamR,
	public ZStreamerRPos_CFType
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
:	public ZYadR_CFType,
	public ZYadStrimR,
	public ZStrimmerR_CFString
	{
public:
	ZYadStrimR_CFType(ZRef<CFStringRef> iStringRef);

// From ZYadR, disambiguating between ZYadR_TValue and ZYadStreamR
//	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_CFType

class ZYadListRPos_CFType
:	public ZYadR_CFType,
	public ZYadListRPos
	{
public:
	ZYadListRPos_CFType(const ZRef<CFArrayRef>& iArray);
	ZYadListRPos_CFType(const ZRef<CFArrayRef>& iArray, uint64 iPosition);

// From ZYadR via ZYadListRPos
	virtual ZRef<ZYadR> ReadInc();

// From ZYadListR via ZYadListRPos
	virtual uint64 GetPosition();

// From ZYadListRPos
	virtual uint64 GetSize();
	virtual void SetPosition(uint64 iPosition);
	virtual ZRef<ZYadListRPos> Clone();

private:
	const ZValList_CFType fList;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_CFType

class ZYadMapRPos_CFType
:	public ZYadR_CFType,
	public ZYadMapRPos
	{
	ZYadMapRPos_CFType(const ZRef<CFDictionaryRef>& iDictionary,
		uint64 iPosition,
		const std::vector<CFStringRef>& iNames,
		const std::vector<CFTypeRef>& iValues);

public:
	ZYadMapRPos_CFType(const ZRef<CFDictionaryRef>& iDictionary);

// From ZYadR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(std::string& oName);

// From ZYadMapRPos
	virtual void SetPosition(const std::string& iName);
	virtual ZRef<ZYadMapRPos> Clone();

private:
	const ZRef<CFDictionaryRef> fDictionary;
	uint64 fPosition;
	std::vector<CFStringRef> fNames;
	std::vector<CFTypeRef> fValues;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_CFType

namespace ZYad_CFType {

ZRef<ZYadR> sMakeYadR(const ZRef<CFTypeRef>& iVal);

ZRef<CFTypeRef> sFromYadR(ZRef<ZYadR>);

} // namespace ZYad_CFType

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZYad_CFType__
