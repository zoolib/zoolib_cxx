// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_Chan_Bin_Operators.h"

#include "zoolib/Util_Chan.h"

#include <string.h> // For strlen

// =================================================================================================
#pragma mark - Util_Chan_Bin_Operators

namespace ZooLib {
namespace Util_Chan_Bin_Operators {

const ChanW_Bin& operator<<(const ChanW_Bin& ww, const char* iString)
	{
	sEWriteMem(ww, iString, strlen(iString));
	return ww;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& ww, char* iString)
	{
	sEWriteMem(ww, iString, strlen(iString));
	return ww;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& ww, const std::string& iString)
	{
	sEWriteMem(ww, iString.c_str(), iString.length());
	return ww;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& ww, const ChanR_Bin& r)
	{
	sCopyAll<byte>(r, ww);
	return ww;
	}

} // namespace Util_Chan_Bin_Operators
} // namespace ZooLib
