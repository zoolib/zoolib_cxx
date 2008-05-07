/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZCONFIG_SPI__
#define __ZCONFIG_SPI__ 1
#include "zconfig.h"

#include "ZCONFIG_SPI_Definitions.h"
#include "ZCONFIG_SPI_Choices.h"

// =================================================================================================
// boost
#ifndef ZCONFIG_SPI_Avail__boost
#	define ZCONFIG_SPI_Avail__boost 0
#endif

#ifndef ZCONFIG_SPI_Desired__boost
#	define ZCONFIG_SPI_Desired__boost 1
#endif


// =================================================================================================
// bzip2
#ifndef ZCONFIG_SPI_Avail__bzip2
#	define ZCONFIG_SPI_Avail__bzip2 0
#endif

#ifndef ZCONFIG_SPI_Desired__bzip2
#	define ZCONFIG_SPI_Desired__bzip2 1
#endif

// =================================================================================================
// iconv
#ifndef ZCONFIG_SPI_Avail__iconv
#	define ZCONFIG_SPI_Avail__iconv 0
#endif

#ifndef ZCONFIG_SPI_Desired__iconv
#	define ZCONFIG_SPI_Desired__iconv 1
#endif


// =================================================================================================
// ICU
#ifndef ZCONFIG_SPI_Avail__ICU
#	define ZCONFIG_SPI_Avail__ICU 0
#endif

#ifndef ZCONFIG_SPI_Desired__ICU
#	define ZCONFIG_SPI_Desired__ICU 1
#endif


// =================================================================================================
// imagehlp
#ifndef ZCONFIG_SPI_Avail__imagehlp
#	define ZCONFIG_SPI_Avail__imagehlp 0
#endif

#ifndef ZCONFIG_SPI_Desired__imagehlp
#	define ZCONFIG_SPI_Desired__imagehlp 1
#endif


// =================================================================================================
// JPEGLib
#ifndef ZCONFIG_SPI_Avail__JPEGLib
#	define ZCONFIG_SPI_Avail__JPEGLib 0
#endif

#ifndef ZCONFIG_SPI_Desired__JPEGLib
#	define ZCONFIG_SPI_Desired__JPEGLib 1
#endif


// =================================================================================================
// libpng
#ifndef ZCONFIG_SPI_Avail__libpng
#	define ZCONFIG_SPI_Avail__libpng 0
#endif

#ifndef ZCONFIG_SPI_Desired__libpng
#	define ZCONFIG_SPI_Desired__libpng 1
#endif


// =================================================================================================
// openssl
#ifndef ZCONFIG_SPI_Avail__openssl
#	define ZCONFIG_SPI_Avail__openssl 0
#endif

#ifndef ZCONFIG_SPI_Desired__openssl
#	define ZCONFIG_SPI_Desired__openssl 1
#endif


// =================================================================================================
// stlsoft
#ifndef ZCONFIG_SPI_Avail__stlsoft
#	define ZCONFIG_SPI_Avail__stlsoft 0
#endif

#ifndef ZCONFIG_SPI_Desired__stlsoft
#	define ZCONFIG_SPI_Desired__stlsoft 1
#endif


// =================================================================================================
// zlib
#ifndef ZCONFIG_SPI_Avail__zlib
#	define ZCONFIG_SPI_Avail__zlib 0
#endif

#ifndef ZCONFIG_SPI_Desired__zlib
#	define ZCONFIG_SPI_Desired__zlib 1
#endif


#endif // __ZCONFIG_SPI__
