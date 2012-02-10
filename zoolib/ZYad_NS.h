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
// MARK: - ZYadR_NS

typedef ZYadR_Val_T<ZRef<NSObject> > ZYadR_NS;

// =================================================================================================
// MARK: - ZYadAtomR_NS

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
// MARK: - ZYadStreamRPos_NS

typedef ZStreamerRPos_T<ZStreamRPos_NSData> ZStreamerRPos_NS;

class ZYadStreamRPos_NS
:	public ZYadR_NS
,	public ZYadStreamR
,	public ZStreamerRPos_NS
	{
public:
	ZYadStreamRPos_NS(NSData* iData);
	virtual ~ZYadStreamRPos_NS();

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
// MARK: - ZYadStrimR_NS

typedef ZStrimmerR_T<ZStrimR_NSString> ZStrimmerR_NSString;

class ZYadStrimR_NS
:	public ZYadR_NS
,	public ZYadStrimR
,	public ZStrimmerR_NSString
	{
public:
	ZYadStrimR_NS(NSString* iString);
	};

// =================================================================================================
// MARK: - ZYadSeqAtRPos_NS

class ZYadSeqAtRPos_NS
:	public ZYadR_NS
,	public ZYadSeqAtRPos_Val_Self_T<ZYadSeqAtRPos_NS, ZSeq_NS>
	{
public:
	ZYadSeqAtRPos_NS(NSArray* iArray);
	ZYadSeqAtRPos_NS(NSArray* iArray, uint64 iPosition);
	};

// =================================================================================================
// MARK: - ZYadMapAtRPos_NS

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
	virtual ZRef<ZYadR> ReadInc(std::string& oName);

// From ZYadMapRClone via ZYadMapAtRPos
	virtual ZRef<ZYadMapRClone> Clone();

// From ZYadMapRPos via ZYadMapAtRPos
	virtual void SetPosition(const std::string& iName);

// From ZYadMapAtR
	virtual ZRef<ZYadR> ReadAt(const std::string& iName);	

private:
	void pSetupPosition();

	const ZRef<NSDictionary> fDictionary;
	uint64 fPosition;
	ZSeq_NS fNames;
	ZSeq_NS fValues;
	};

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadR> sYadR(NSObject* iVal);

ZRef<ZYadStrimR> sYadR(NSMutableString* iString);
ZRef<ZYadStrimR> sYadR(NSString* iString);

ZRef<ZYadStreamR> sYadR(NSMutableData* iData);
ZRef<ZYadStreamR> sYadR(NSData* iData);

ZRef<ZYadSeqAtRPos> sYadR(NSMutableArray* iArray);
ZRef<ZYadSeqAtRPos> sYadR(NSArray* iArray);

ZRef<ZYadMapAtRPos> sYadR(NSMutableDictionary* iDictionary);
ZRef<ZYadMapAtRPos> sYadR(NSDictionary* iDictionary);

// =================================================================================================
// MARK: - sFromYadR

ZRef<NSObject> sFromYadR(NSObject* iDefault, ZRef<ZYadR> iYadR);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZYad_NS_h__
