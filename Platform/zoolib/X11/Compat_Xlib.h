// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_X11_Compat_XLib_h__
#define __ZooLib_X11_Compat_XLib_h__
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(X11)
// Unfortunately, QuickDraw also defines Cursor. For the moment this lets us compile
// code that sees both QuickDraw and X11 headers. And don't get me started on BOOL ...
	#define Cursor ZooLib_X11_Cursor
	#include <X11/Xlib.h>
	#undef Cursor
#endif

#endif // __ZooLib_X11_Compat_XLib_h__
