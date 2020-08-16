// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

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

ZP<TextDecoder> sMake_TextDecoder_Mac(const std::string& iSourceName);

ZP<TextEncoder> sMake_TextEncoder_Mac(const std::string& iDestName);

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(TextCoder_Mac)

#endif // __ZooLib_OSX_TextCoder_Mac_h__
