// Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_PixmapCoder_PNG_h__
#define __ZooLib_Pixels_PixmapCoder_PNG_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/Pixmap.h"
#include "zoolib/ChanR_Bin.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark -

Pixmap sReadPixmap_PNG(const ChanR_Bin& iStream);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_PixmapCoder_PNG_h__
