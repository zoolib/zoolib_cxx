/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_CGData_h__
#define __ZStream_CGData_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CoreGraphics)

#include ZMACINCLUDE3(ApplicationServices,CoreGraphics,CGDataProvider.h)
#include ZMACINCLUDE3(ApplicationServices,CoreGraphics,CGDataConsumer.h)

#include "zoolib/ZRef.h"
#include "zoolib/ZStreamer.h"

namespace ZooLib {
namespace ZStream_CGData {

// =================================================================================================
#pragma mark -
#pragma mark * ZStream_CGData


ZRef<CGDataProviderRef> sCGDataProviderCreate(ZRef<ZStreamerR> iStreamer);

ZRef<CGDataProviderRef> sCGDataProviderCreateRewind(ZRef<ZStreamerRPos> iStreamer);

ZRef<CGDataConsumerRef> sCGDataConsumerCreate(ZRef<ZStreamerW> iStreamer);

} // namespace ZStream_CGData
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)

#endif // __ZStream_CGData_h__
