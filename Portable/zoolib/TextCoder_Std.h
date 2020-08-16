// Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_TextCoder_Std_h__
#define __ZooLib_TextCoder_Std_h__ 1
#include "zconfig.h"

#include "zoolib/TextCoder.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

ZP<TextDecoder> sMake_TextDecoder_Std(const std::string& iSourceName);

ZP<TextEncoder> sMake_TextEncoder_Std(const std::string& iDestName);

} // namespace ZooLib

#endif // __ZooLib_TextCoder_Std_h__
