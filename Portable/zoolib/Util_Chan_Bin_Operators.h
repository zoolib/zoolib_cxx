// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Chan_Bin_Operators_h__
#define __ZooLib_Util_Chan_Bin_Operators_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"

#include <string>

// =================================================================================================
#pragma mark - ZUtil_Stream_Operators

namespace ZooLib {
namespace Util_Chan_Bin_Operators {

const ChanW_Bin& operator<<(const ChanW_Bin& ww, const char* iString);

const ChanW_Bin& operator<<(const ChanW_Bin& ww, char* iString);

const ChanW_Bin& operator<<(const ChanW_Bin& ww, const std::string& iString);

const ChanW_Bin& operator<<(const ChanW_Bin& ww, const ChanR_Bin& r);

} // namespace Util_Chan_Bin_Operators

#if not defined(ZMACRO_Util_Chan_Bin_Operators_Suppress) || not ZMACRO_Util_Chan_Bin_Operators_Suppress
	using Util_Chan_Bin_Operators::operator<<;
#endif

} // namespace ZooLib

#endif // __ZooLib_Util_Chan_Bin_Operators_h__
