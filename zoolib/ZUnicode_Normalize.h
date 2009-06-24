/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZUnicode_Normalize__
#define __ZUnicode_Normalize__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Unicode_Normalize
#	define ZCONFIG_API_Avail__Unicode_Normalize \
	(ZCONFIG_SPI_Enabled(CFType) || ZCONFIG_SPI_Enabled(ICU))
#endif

#ifndef ZCONFIG_API_Desired__Unicode_Normalize
#	define ZCONFIG_API_Desired__Unicode_Normalize 1
#endif

#include "zoolib/ZUnicodeString.h"

#if ZCONFIG_API_Enabled(Unicode_Normalize)

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZUnicode, normalization -- very incomplete for now

namespace ZUnicode {

enum ENormForm
	{
	/// Canonical Decomposition
	eNormForm_D,

	/// Compatibility Decomposition
	eNormForm_KD,

	/// Canonical Decomposition followed by Canonical Composition
	eNormForm_C,

	/// Compatibility Decomposition followed by Canonical Composition
	eNormForm_KC
	};

string16 sNormalized(const string16& iString, ENormForm iNormForm);

} // namespace ZUnicode


NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(Unicode_Normalize)

#endif // __ZUnicode_Normalize__
