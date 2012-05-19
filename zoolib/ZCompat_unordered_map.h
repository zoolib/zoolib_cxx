/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZCompat_unordered_map_h__
#define __ZCompat_unordered_map_h__ 1
#include "zconfig.h"

#ifndef ZCONFIG_SPI_Desired__unordered_map
	#define ZCONFIG_SPI_Desired__unordered_map 1
#endif

#if ZCONFIG_SPI_Desired__unordered_map
	#define ZCONFIG_SPI_Avail__unordered_map 1
	#if ZCONFIG_LIBCPP_11
		#include <unordered_map>
		namespace ZooLib { using unordered_map; }
	#elif ZCONFIG_LIBCPP_TR1
		#include <tr1/unordered_map>
		namespace ZooLib { using std::tr1::unordered_map; }
	#elif ZCONFIG_LIBCPP_GCCExtensions
		#include <hash_map>
		namespace ZooLib { typedef hash_map unordered_map; }
	#else
		#undef ZCONFIG_SPI_Avail__unordered_map
		#define ZCONFIG_SPI_Avail__unordered_map 0
	#endif
#endif

#endif // __ZCompat_unordered_map_h__
