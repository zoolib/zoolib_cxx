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

#ifndef __ZYad_Any__
#define __ZYad_Any__ 1
#include "zconfig.h"

#include "zoolib/ZVal_Any.h"
#include "zoolib/ZYad_Val_T.h"

NAMESPACE_ZOOLIB_BEGIN

typedef ZYadStreamRPos_Val_T<ZData_Any> ZYadStreamRPos_Any;
typedef ZYadSeqRPos_Val_T<ZSeq_Any> ZYadSeqRPos_Any;
typedef ZYadMapRPos_Val_T<ZMap_Any> ZYadMapRPos_Any;

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const ZAny& iVal);

ZRef<ZYadStreamR> sMakeYadR(const ZData_Any& iData);

ZRef<ZYadSeqR> sMakeYadR(const ZSeq_Any& iSeq);

ZRef<ZYadMapR> sMakeYadR(const ZMap_Any& iMap);

// =================================================================================================
#pragma mark -
#pragma mark * sFromYadR

ZVal_Any sFromYadR(const ZVal_Any& iDefault, ZRef<ZYadR> iYadR);

NAMESPACE_ZOOLIB_END

#endif // __ZYad_Any__
