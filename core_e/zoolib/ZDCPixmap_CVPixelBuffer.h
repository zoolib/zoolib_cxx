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

#ifndef __ZDCPixmap_CVPixelBuffer_h__
#define __ZDCPixmap_CVPixelBuffer_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(MacOSX)

#include "zoolib/ZDCPixmap.h"

#include ZMACINCLUDE2(CoreVideo,CVPixelBuffer.h)

#if !defined(MAC_OS_X_VERSION_10_5) || MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5
	// Target OS is pre-10.5, so we don't have the kCVPixelFormatType_XXX enums.
	#include ZMACINCLUDE3(ApplicationServices,QD,QuickDraw.h)
	#define ZMACRO_CVPixelFormat(a) k##a##PixelFormat
#else
	#define ZMACRO_CVPixelFormat(a) kCVPixelFormatType_##a
#endif

namespace ZooLib {
namespace ZDCPixmap_CVPixelBuffer {

// =================================================================================================
// MARK: - ZDCPixmap_CVPixelBuffer

ZDCPixmap sPixmap(ZRef<CVPixelBufferRef> iPBR);

} // namespace ZDCPixmap_CVPixelBuffer
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(MacOSX)

#endif // __ZDCPixmap_CVPixelBuffer_h__
