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

#ifndef __ZYad_ZooLib_h__
#define __ZYad_ZooLib_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZVal_ZooLib.h"
#include "zoolib/ZYad_Val_T.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZYadR_ZooLib

typedef ZYadR_Val_T<ZVal_ZooLib> ZYadR_ZooLib;

// =================================================================================================
// MARK: - ZYadAtomR_ZooLib

class ZYadAtomR_ZooLib
:	public ZYadR_ZooLib,
	public ZYadAtomR
	{
public:
	ZYadAtomR_ZooLib();
	ZYadAtomR_ZooLib(const ZVal_ZooLib& iVal);
	ZYadAtomR_ZooLib(ZType iType, const ZStreamR& iStreamR);

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZYadAtomR
	ZAny AsAny();
	};

// =================================================================================================
// MARK: - ZYadStreamRPos_ZooLib

typedef ZYadStreamRPos_Val_T<ZData_ZooLib> ZYadStreamRPos_ZooLib;

// =================================================================================================
// MARK: - ZYadSeqRPos_ZooLib

class ZYadSeqRPos_ZooLib
:	public ZYadR_ZooLib
,	public ZYadSeqRPos_Val_Self_T<ZYadSeqRPos_ZooLib, ZSeq_ZooLib>
	{
public:
	ZYadSeqRPos_ZooLib(const ZSeq_ZooLib& iSeq);
	ZYadSeqRPos_ZooLib(const ZSeq_ZooLib& iSeq, uint64 iPosition);

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
// MARK: - ZYadMapRPos_ZooLib

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
// MARK: - sYadR

ZRef<ZYadR> sYadR(const ZVal_ZooLib& iVal);

ZRef<ZYadSeqRPos> sYadR(const ZSeq_ZooLib& iSeq);

ZRef<ZYadMapRPos> sYadR(const ZMap_ZooLib& iMap);

// =================================================================================================
// MARK: - sFromYadR

ZVal_ZooLib sFromYadR(const ZVal_ZooLib& iDefault, ZRef<ZYadR> iYadR);

} // namespace ZooLib

#endif // __ZYad_ZooLib_h__
