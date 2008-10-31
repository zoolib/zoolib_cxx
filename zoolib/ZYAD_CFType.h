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

#ifndef __ZYAD_CFType__
#define __ZYAD_CFType__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZYAD.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include <vector>

#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFDictionary.h>

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD_CFType

class ZYAD_CFType : public ZYAD
	{
public:
	ZYAD_CFType(CFTypeRef iCFTypeRef);
	virtual ~ZYAD_CFType();

	virtual bool GetTValue(ZTValue& oYalue);

	CFTypeRef GetCFTypeRef();
	
private:
	CFTypeRef fCFTypeRef;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep_CFType

class ZYADReaderRep_CFType: public ZYADReaderRep
	{
public:
	ZYADReaderRep_CFType(CFTypeRef iCFTypeRef);
	virtual ~ZYADReaderRep_CFType();

	virtual bool HasValue();
	virtual ZType Type();

	virtual ZRef<ZMapReaderRep> ReadMap();
	virtual ZRef<ZListReaderRep> ReadList();
	virtual ZRef<ZStreamerR> ReadRaw();
	virtual ZRef<ZYAD> ReadYAD();

private:
	CFTypeRef fCFTypeRef;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep_CFType

class ZMapReaderRep_CFType : public ZMapReaderRep
	{
public:
	ZMapReaderRep_CFType(CFDictionaryRef iCFDictionaryRef);
	virtual ~ZMapReaderRep_CFType();

	virtual bool HasValue();
	virtual std::string Name();
	virtual ZRef<ZYADReaderRep> Read();
	virtual void Skip();

private:
	CFDictionaryRef fCFDictionaryRef;
	size_t fIndex;
	std::vector<CFStringRef> fNames;
	std::vector<CFTypeRef> fValues;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep_CFType

class ZListReaderRep_CFType : public ZListReaderRep
	{
public:
	ZListReaderRep_CFType(CFArrayRef iCFArrayRef);
	virtual ~ZListReaderRep_CFType();

	virtual bool HasValue();
	virtual size_t Index() const;
	virtual ZRef<ZYADReaderRep> Read();
	virtual void Skip();

private:
	CFArrayRef fCFArrayRef;
	size_t fIndex;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADUtil_CFType

namespace ZYADUtil_CFType {

bool sFromReader(ZYADReader iYADReader, CFTypeRef& oCFTypeRef);
CFTypeRef sFromReader(ZYADReader iYADReader);

} // namespace ZYADUtil_CFType

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZYAD_CFType__
