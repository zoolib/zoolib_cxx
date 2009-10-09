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

#ifndef __ZWinRegistry_Yad__
#define __ZWinRegistry_Yad__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZWinHeader.h"
#include "zoolib/ZWinRegistry_Val.h"
#include "zoolib/ZYad_Val_T.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZWinRegistry {

using namespace std;

// =================================================================================================
#pragma mark -
#pragma mark * ZWinRegistry

typedef ZYadMapRPos_Val_T<KeyRef, KeyRef::Index_t> YadMapRPos;

ZRef<ZYadMapR> sMakeYadR(const KeyRef& iKeyRef);

ZRef<ZYadR> sMakeYadR(const Val& iVal);

} // namespace ZWinRegistry

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZWinRegistry_Yad__
