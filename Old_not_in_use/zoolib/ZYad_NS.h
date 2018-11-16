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

#ifndef __ZYad_NS_h__
#define __ZYad_NS_h__ 1
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
#pragma mark - ZYadR_NS

typedef ZYadR_Val_T<ZRef<NSObject> > ZYadR_NS;

// =================================================================================================
#pragma mark - ZYadAtomR_NS

class ZYadAtomR_NS
:	public ZYadR_NS
,	public ZYadAtomR
	{
public:
	ZYadAtomR_NS(NSObject* iVal);

// From ZYadAtomR
	virtual ZAny AsAny();
	};

// =================================================================================================
#pragma mark - ZYadStreamerRPos_NS

typedef ZStreamerRPos_T<ZStreamRPos_NSData> ZStreamerRPos_NS;

class ZYadStreamerRPos_NS
:	public ZYadR_NS
,	public ZYadStreamerR
,	public ZStreamerRPos_NS
	{
public:
	ZYadStreamerRPos_NS(NSData* iData);
	virtual ~ZYadStreamerRPos_NS();

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark - ZYadStrimmerR_NS

typedef ZStrimmerR_T<ZStrimR_NSString> ZStrimmerR_NSString;

class ZYadStrimmerR_NS
:	public ZYadR_NS
,	public ZYadStrimmerR
,	public ZStrimmerR_NSString
	{
public:
	ZYadStrimmerR_NS(NSString* iString);
	};

// =================================================================================================
#pragma mark - ZYadSeqAtRPos_NS

class ZYadSeqAtRPos_NS
:	public ZYadR_NS
,	public ZYadSeqAtRPos_Val_Self_T<ZYadSeqAtRPos_NS, ZSeq_NS>
	{
public:
	ZYadSeqAtRPos_NS(NSArray* iArray);
	ZYadSeqAtRPos_NS(NSArray* iArray, uint64 iPosition);
	};

// =================================================================================================
#pragma mark - ZYadMapAtRPos_NS

class ZYadMapAtRPos_NS
:	public ZYadR_NS
,	public ZYadMapAtRPos
	{
	ZYadMapAtRPos_NS(NSDictionary* iDictionary,
		uint64 iPosition,
		const ZSeq_NS& iNames,
		const ZSeq_NS& iValues);

public:
	ZYadMapAtRPos_NS(NSDictionary* iDictionary);

// From ZYadMapR via ZYadMapAtRPos
	virtual ZRef<ZYadR> ReadInc(Name& oName);

// From ZYadMapRClone via ZYadMapAtRPos
	virtual ZRef<ZYadMapRClone> Clone();

// From ZYadMapRPos via ZYadMapAtRPos
	virtual void SetPosition(const Name& iName);

// From ZYadMapAtR
	virtual ZRef<ZYadR> ReadAt(const Name& iName);	

private:
	void pSetupPosition();

	const ZRef<NSDictionary> fDictionary;
	uint64 fPosition;
	ZSeq_NS fNames;
	ZSeq_NS fValues;
	};

// =================================================================================================
#pragma mark - sYadR

ZRef<ZYadR> sYadR(NSObject* iVal);

ZRef<ZYadStrimmerR> sYadR(NSMutableString* iString);
ZRef<ZYadStrimmerR> sYadR(NSString* iString);

ZRef<ZYadStreamerR> sYadR(NSMutableData* iData);
ZRef<ZYadStreamerR> sYadR(NSData* iData);

ZRef<ZYadSeqAtRPos> sYadR(NSMutableArray* iArray);
ZRef<ZYadSeqAtRPos> sYadR(NSArray* iArray);

ZRef<ZYadMapAtRPos> sYadR(NSMutableDictionary* iDictionary);
ZRef<ZYadMapAtRPos> sYadR(NSDictionary* iDictionary);

// =================================================================================================
#pragma mark - sFromYadR

ZRef<NSObject> sFromYadR(NSObject* iDefault, ZRef<ZYadR> iYadR);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZYad_NS_h__
