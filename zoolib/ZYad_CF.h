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

#ifndef __ZYad_CF_h__
#define __ZYad_CF_h__ 1
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
// MARK: - ZYadR_CF

typedef ZYadR_Val_T<ZRef<CFTypeRef> > ZYadR_CF;

// =================================================================================================
// MARK: - ZYadAtomR_CF

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
// MARK: - ZYadStreamerRPos_CF

typedef ZStreamerRPos_T<ZStreamRPos_CFData> ZStreamerRPos_CF;

class ZYadStreamerRPos_CF
:	public ZYadR_CF
,	public ZYadStreamerR
,	public ZStreamerRPos_CF
	{
public:
	ZYadStreamerRPos_CF(CFDataRef iData);
	virtual ~ZYadStreamerRPos_CF();

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
// MARK: - ZYadStrimmerR_CF

typedef ZStrimmerR_T<ZStrimR_CFString> ZStrimmerR_CFString;

class ZYadStrimmerR_CF
:	public ZYadR_CF
,	public ZYadStrimmerR
,	public ZStrimmerR_CFString
	{
public:
	ZYadStrimmerR_CF(CFStringRef iStringRef);
	};

// =================================================================================================
// MARK: - ZYadSeqAtRPos_CF

class ZYadSeqAtRPos_CF
:	public ZYadR_CF
,	public ZYadSeqAtRPos_Val_Self_T<ZYadSeqAtRPos_CF, ZSeq_CF>
	{
public:
	ZYadSeqAtRPos_CF(CFArrayRef iArray);
	ZYadSeqAtRPos_CF(CFArrayRef iArray, uint64 iPosition);
	};

// =================================================================================================
// MARK: - ZYadMapAtRPos_CF

class ZYadMapAtRPos_CF
:	public ZYadR_CF
,	public ZYadMapAtRPos
	{
	ZYadMapAtRPos_CF(CFDictionaryRef iDictionary,
		uint64 iPosition,
		const std::vector<CFStringRef>& iNames,
		const std::vector<CFTypeRef>& iValues);

public:
	ZYadMapAtRPos_CF(CFDictionaryRef iDictionary);

// From ZYadMapR via ZYadMapAtRPos
	virtual ZRef<ZYadR> ReadInc(ZName& oName);

// From ZYadMapRClone via ZYadMapAtRPos
	virtual ZRef<ZYadMapRClone> Clone();

// From ZYadMapRPos via ZYadMapAtRPos
	virtual void SetPosition(const ZName& iName);

// From ZYadMapAtR
	virtual ZRef<ZYadR> ReadAt(const ZName& iName);

private:
	void pSetupPosition();

	const ZRef<CFDictionaryRef> fDictionary;
	uint64 fPosition;
	std::vector<CFStringRef> fNames;
	std::vector<CFTypeRef> fValues;
	};

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadR> sYadR(CFTypeRef iVal);
ZRef<ZYadR> sYadR(const ZRef<CFTypeRef>& iVal);

ZRef<ZYadStrimmerR> sYadR(CFMutableStringRef iString);
ZRef<ZYadStrimmerR> sYadR(CFStringRef iString);
ZRef<ZYadStrimmerR> sYadR(const ZRef<CFStringRef>& iString);

ZRef<ZYadStreamerR> sYadR(CFMutableDataRef iData);
ZRef<ZYadStreamerR> sYadR(CFDataRef iData);
ZRef<ZYadStreamerR> sYadR(const ZRef<CFDataRef>& iData);

ZRef<ZYadSeqAtRPos> sYadR(CFMutableArrayRef iArray);
ZRef<ZYadSeqAtRPos> sYadR(CFArrayRef iArray);
ZRef<ZYadSeqAtRPos> sYadR(const ZRef<CFArrayRef>& iArray);

ZRef<ZYadMapAtRPos> sYadR(CFMutableDictionaryRef iDictionary);
ZRef<ZYadMapAtRPos> sYadR(CFDictionaryRef iDictionary);
ZRef<ZYadMapAtRPos> sYadR(const ZRef<CFDictionaryRef>& iDictionary);

// =================================================================================================
// MARK: - sFromYadR

ZRef<CFTypeRef> sFromYadR(CFTypeRef iDefault, ZRef<ZYadR> iYadR);
ZRef<CFTypeRef> sFromYadR(const ZRef<CFTypeRef>& iDefault, ZRef<ZYadR> iYadR);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZYad_CF_h__
