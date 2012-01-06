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

#ifndef __ZPhotoshop_h__
#define __ZPhotoshop_h__ 1
#include "zconfig.h"

// When building for Windows, PITypes.h pulls in Windows.h, and pollutes
// the preprocessor macro space with all kinds of crap. So we first pull in
// ZCompat_Win, which undoes the worst of it.
#include "zoolib/ZCompat_Win.h"

// Pick up some photoshop definitions
#include "PIGeneral.h"

// Use them to generate a value for ZCONFIG_Photoshop_SDKVersion

#define ZCONFIG_Photoshop_SDKVersion_PS7 7
#define ZCONFIG_Photoshop_SDKVersion_CS1 8
#define ZCONFIG_Photoshop_SDKVersion_CS2 9
#define ZCONFIG_Photoshop_SDKVersion_CS3 10
#define ZCONFIG_Photoshop_SDKVersion_CS4 11
#define ZCONFIG_Photoshop_SDKVersion_CS5 12

#if 0
#elif defined(__PSIntTypes__)
	#define ZCONFIG_Photoshop_SDKVersion ZCONFIG_Photoshop_SDKVersion_CS5
#elif defined(PIWin64X86CodeProperty)
	#define ZCONFIG_Photoshop_SDKVersion ZCONFIG_Photoshop_SDKVersion_CS4
#elif defined(PICodeMacIntel64Property)
	#define ZCONFIG_Photoshop_SDKVersion ZCONFIG_Photoshop_SDKVersion_CS3
#elif defined(PIPowerPCMachOCodeProperty)
	#define ZCONFIG_Photoshop_SDKVersion ZCONFIG_Photoshop_SDKVersion_CS2
#elif defined(errHostVersionIncompatible)
	#define ZCONFIG_Photoshop_SDKVersion ZCONFIG_Photoshop_SDKVersion_CS1
#else
	#define ZCONFIG_Photoshop_SDKVersion ZCONFIG_Photoshop_SDKVersion_PS7
#endif

#endif // __ZPhotoshop_h__
