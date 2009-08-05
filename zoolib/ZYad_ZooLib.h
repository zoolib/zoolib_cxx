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

#ifndef __ZYad_ZooLib__
#define __ZYad_ZooLib__ 1
#include "zconfig.h"

#include "zoolib/ZStream_ValData_T.h"
#include "zoolib/ZVal_ZooLib.h"
#include "zoolib/ZYad_Val_T.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_ZooLib

class ZYadR_ZooLib
:	public ZYadR_Val_T<ZVal_ZooLib>
	{
public:
	ZYadR_ZooLib();
	ZYadR_ZooLib(const ZVal_ZooLib& iVal);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_ZooLib

class ZYadPrimR_ZooLib
:	public ZYadR_ZooLib,
	public ZYadPrimR
	{
public:
	ZYadPrimR_ZooLib();
	ZYadPrimR_ZooLib(const ZVal_ZooLib& iVal);
	ZYadPrimR_ZooLib(ZType iType, const ZStreamR& iStreamR);

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZYadPrimR
	ZAny AsAny();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamRPos_ZooLib

typedef ZYadStreamRPos_Val_T<ZData_ZooLib> ZYadStreamRPos_ZooLib;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_ZooLib

class ZYadListRPos_ZooLib
:	public ZYadR_ZooLib
,	public ZYadListRPos_Val_Self_T<ZYadListRPos_ZooLib, ZList_ZooLib>
	{
public:
	ZYadListRPos_ZooLib(const ZList_ZooLib& iList);
	ZYadListRPos_ZooLib(const ZList_ZooLib& iList, uint64 iPosition);

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_ZooLib

//typedef ZYadMapRPos_Val_T<ZMap_ZooLib, ZMap_ZooLib::Index_t> ZYadMapRPos_ZooLib;

#if 1
class ZYadMapRPos_ZooLib
:	public ZYadR_ZooLib
,	public ZYadMapRPos_Val_Self_T<ZYadMapRPos_ZooLib, ZMap_ZooLib, ZMap_ZooLib::Index_t>
	{
public:
	ZYadMapRPos_ZooLib(const ZMap_ZooLib& iMap);
	ZYadMapRPos_ZooLib(const ZMap_ZooLib& iMap, const Index_t& iPosition);

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};
#endif

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const ZVal_ZooLib& iVal);

ZRef<ZYadListR> sMakeYadR(const ZList_ZooLib& iList);

ZRef<ZYadMapR> sMakeYadR(const ZMap_ZooLib& iMap);

// =================================================================================================
#pragma mark -
#pragma mark * sFromYadR

ZVal_ZooLib sFromYadR(ZRef<ZYadR> iYadR, const ZVal_ZooLib& iDefault);

NAMESPACE_ZOOLIB_END

#endif // __ZYad_ZooLib__
