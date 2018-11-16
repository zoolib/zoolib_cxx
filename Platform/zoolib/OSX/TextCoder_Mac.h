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

#ifndef __ZooLib_OSX_TextCoder_Mac_h__
#define __ZooLib_OSX_TextCoder_Mac_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__TextCoder_Mac
	#define ZCONFIG_API_Avail__TextCoder_Mac \
	(ZCONFIG_SPI_Enabled(Carbon64) \
	|| ZCONFIG_SPI_Enabled(MacClassic))
#endif

#ifndef ZCONFIG_API_Desired__TextCoder_Mac
	#define ZCONFIG_API_Desired__TextCoder_Mac 1
#endif

#include "zoolib/TextCoder.h"

#if ZCONFIG_API_Enabled(TextCoder_Mac)

#include ZMACINCLUDE3(CoreServices,CarbonCore,UnicodeConverter.h)

namespace ZooLib {

// =================================================================================================
#pragma mark - 

ZRef<TextDecoder> sMake_TextDecoder_Mac(const std::string& iSourceName);

ZRef<TextEncoder> sMake_TextEncoder_Mac(const std::string& iDestName);

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(TextCoder_Mac)

#endif // __ZooLib_OSX_TextCoder_Mac_h__
