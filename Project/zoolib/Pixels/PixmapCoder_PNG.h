// Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_PixmapCoder_PNG_h__
#define __ZooLib_Pixels_PixmapCoder_PNG_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/Pixmap.h"
#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark -

Pixmap sReadPixmap_PNG(const ChanR_Bin& iChanR);

void sWritePixmap_PNG(const Pixmap& iPixmap, const ChanW_Bin& iChanW);

void sWritePixmap_PNG(
	const void* iBaseAddress,
	const RasterDesc& iRasterDesc,
	const PixelDesc& iPixelDesc,
	const RectPOD& iBounds,
	const ChanW_Bin& iChanW);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_PixmapCoder_PNG_h__
