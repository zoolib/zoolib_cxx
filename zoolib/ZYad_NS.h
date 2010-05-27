/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZYad_NS__
#define __ZYad_NS__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/ZStream_NSData.h"
#include "zoolib/ZStrim_NSString.h"
#include "zoolib/ZVal_NS.h"
#include "zoolib/ZYad_Val_T.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_NS

typedef ZYadR_Val_T<ZRef<NSObject> > ZYadR_NS;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_NS

class ZYadPrimR_NS
:	public ZYadR_NS
,	public ZYadPrimR
	{
public:
	ZYadPrimR_NS(const ZRef<NSObject>& iVal);

// From ZYadPrimR
	virtual ZAny AsAny();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamRPos_NS

typedef ZStreamerRPos_T<ZStreamRPos_NSData> ZStreamerRPos_NS;

class ZYadStreamRPos_NS
:	public ZYadR_NS
,	public ZYadStreamR
,	public ZStreamerRPos_NS
	{
public:
	ZYadStreamRPos_NS(const ZRef<NSData>& iData);
	virtual ~ZYadStreamRPos_NS();

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_NS

typedef ZStrimmerR_T<ZStrimR_NSString> ZStrimmerR_NSString;

class ZYadStrimR_NS
:	public ZYadR_NS
,	public ZYadStrimR
,	public ZStrimmerR_NSString
	{
public:
	ZYadStrimR_NS(ZRef<NSString> iStringRef);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqRPos_NS

class ZYadSeqRPos_NS
:	public ZYadR_NS
,	public ZYadSeqRPos_Val_Self_T<ZYadSeqRPos_NS, ZSeq_NS>
	{
public:
	ZYadSeqRPos_NS(const ZRef<NSArray>& iArray);
	ZYadSeqRPos_NS(const ZRef<NSArray>& iArray, uint64 iPosition);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_NS

class ZYadMapRPos_NS
:	public ZYadR_NS
,	public ZYadMapRPos
	{
	ZYadMapRPos_NS(const ZRef<NSDictionary>& iDictionary,
		uint64 iPosition,
		const ZSeq_NS& iNames,
		const ZSeq_NS& iValues);

public:
	ZYadMapRPos_NS(const ZRef<NSDictionary>& iDictionary);

// From ZYadMapR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(std::string& oName);

// From ZYadMapRPos
	virtual ZRef<ZYadMapRPos> Clone();

	virtual void SetPosition(const std::string& iName);

private:
	const ZRef<NSDictionary> fDictionary;
	uint64 fPosition;
	ZSeq_NS fNames;
	ZSeq_NS fValues;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const ZRef<NSObject>& iVal);

ZRef<ZYadStrimR> sMakeYadR(const ZRef<NSMutableString>& iString);
ZRef<ZYadStrimR> sMakeYadR(const ZRef<NSString>& iString);

ZRef<ZYadStreamR> sMakeYadR(const ZRef<NSMutableData>& iData);
ZRef<ZYadStreamR> sMakeYadR(const ZRef<NSData>& iData);

ZRef<ZYadSeqRPos> sMakeYadR(const ZRef<NSMutableArray>& iArray);
ZRef<ZYadSeqRPos> sMakeYadR(const ZRef<NSArray>& iArray);

ZRef<ZYadMapRPos> sMakeYadR(const ZRef<NSMutableDictionary>& iDictionary);
ZRef<ZYadMapRPos> sMakeYadR(const ZRef<NSDictionary>& iDictionary);

// =================================================================================================
#pragma mark -
#pragma mark * sFromYadR

ZRef<NSObject> sFromYadR(const ZRef<NSObject>& iDefault, ZRef<ZYadR> iYadR);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZYad_NS__
