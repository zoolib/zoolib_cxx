// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Pixmap_CGImage_h__
#define __ZooLib_Apple_Pixmap_CGImage_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/Pixels/Pixmap.h"

#if ZCONFIG_SPI_Enabled(CoreGraphics)

#if ZMACRO_IOS
	#include ZMACINCLUDE2(CoreGraphics,CGImage.h)
#else
	#include ZMACINCLUDE3(ApplicationServices,CoreGraphics,CGImage.h)
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark - sPixmap

Pixels::Pixmap sPixmap(ZP<CGImageRef> iImageRef);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)

#endif // __ZooLib_Apple_Pixmap_CGImage_h__
