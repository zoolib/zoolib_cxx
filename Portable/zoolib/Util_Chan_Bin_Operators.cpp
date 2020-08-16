// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_Chan_Bin_Operators.h"

#include "zoolib/Util_Chan.h"

// =================================================================================================
#pragma mark - Util_Chan_Bin_Operators

namespace ZooLib {
namespace Util_Chan_Bin_Operators {

const ChanW_Bin& operator<<(const ChanW_Bin& w, const char* iString)
	{
	sEWriteMem(w, iString, strlen(iString));
	return w;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& w, char* iString)
	{
	sEWriteMem(w, iString, strlen(iString));
	return w;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& w, const std::string& iString)
	{
	sEWriteMem(w, iString.c_str(), iString.length());
	return w;
	}

const ChanW_Bin& operator<<(const ChanW_Bin& w, const ChanR_Bin& r)
	{
	sCopyAll<byte>(r, w);
	return w;
	}

} // namespace Util_Chan_Bin_Operators
} // namespace ZooLib
