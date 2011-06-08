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

#ifndef __ZYad_CF__
#define __ZYad_CF__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZStream_CFData.h"
#include "zoolib/ZStrim_CFString.h"
#include "zoolib/ZVal_CF.h"
#include "zoolib/ZYad_Val_T.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_CF

typedef ZYadR_Val_T<ZRef<CFTypeRef> > ZYadR_CF;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadAtomR_CF

class ZYadAtomR_CF
:	public ZYadR_CF
,	public ZYadAtomR
	{
public:
	ZYadAtomR_CF(CFTypeRef iVal);

// From ZYadAtomR
	virtual ZAny AsAny();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamRPos_CF

typedef ZStreamerRPos_T<ZStreamRPos_CFData> ZStreamerRPos_CF;

class ZYadStreamRPos_CF
:	public ZYadR_CF
,	public ZYadStreamR
,	public ZStreamerRPos_CF
	{
public:
	ZYadStreamRPos_CF(CFDataRef iData);
	virtual ~ZYadStreamRPos_CF();

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_CF

typedef ZStrimmerR_T<ZStrimR_CFString> ZStrimmerR_CFString;

class ZYadStrimR_CF
:	public ZYadR_CF
,	public ZYadStrimR
,	public ZStrimmerR_CFString
	{
public:
	ZYadStrimR_CF(CFStringRef iStringRef);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqRPos_CF

class ZYadSeqRPos_CF
:	public ZYadR_CF
,	public ZYadSeqRPos_Val_Self_T<ZYadSeqRPos_CF, ZSeq_CF>
	{
public:
	ZYadSeqRPos_CF(CFArrayRef iArray);
	ZYadSeqRPos_CF(CFArrayRef iArray, uint64 iPosition);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_CF

class ZYadMapRPos_CF
:	public ZYadR_CF
,	public ZYadMapRPos
	{
	ZYadMapRPos_CF(CFDictionaryRef iDictionary,
		uint64 iPosition,
		const std::vector<CFStringRef>& iNames,
		const std::vector<CFTypeRef>& iValues);

public:
	ZYadMapRPos_CF(CFDictionaryRef iDictionary);

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

ZRef<ZYadR> sMakeYadR(CFTypeRef iVal);
ZRef<ZYadR> sMakeYadR(const ZRef<CFTypeRef>& iVal);

ZRef<ZYadStrimR> sMakeYadR(CFMutableStringRef iString);
ZRef<ZYadStrimR> sMakeYadR(CFStringRef iString);
ZRef<ZYadStrimR> sMakeYadR(const ZRef<CFStringRef>& iString);

ZRef<ZYadStreamR> sMakeYadR(CFMutableDataRef iData);
ZRef<ZYadStreamR> sMakeYadR(CFDataRef iData);
ZRef<ZYadStreamR> sMakeYadR(const ZRef<CFDataRef>& iData);

ZRef<ZYadSeqRPos> sMakeYadR(CFMutableArrayRef iArray);
ZRef<ZYadSeqRPos> sMakeYadR(CFArrayRef iArray);
ZRef<ZYadSeqRPos> sMakeYadR(const ZRef<CFArrayRef>& iArray);

ZRef<ZYadMapRPos> sMakeYadR(CFMutableDictionaryRef iDictionary);
ZRef<ZYadMapRPos> sMakeYadR(CFDictionaryRef iDictionary);
ZRef<ZYadMapRPos> sMakeYadR(const ZRef<CFDictionaryRef>& iDictionary);

// =================================================================================================
#pragma mark -
#pragma mark * sFromYadR

ZRef<CFTypeRef> sFromYadR(CFTypeRef iDefault, ZRef<ZYadR> iYadR);
ZRef<CFTypeRef> sFromYadR(const ZRef<CFTypeRef>& iDefault, ZRef<ZYadR> iYadR);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZYad_CF__
