// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Memory.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

int sMemCompare(const void* iL, size_t iCountL, const void* iR, size_t iCountR)
	{
	if (iCountL < iCountR)
		{
		if (int compare = std::memcmp(iL, iR, iCountL))
			return compare;
		return -1;
		}
	else if (iCountL > iCountR)
		{
		if (int compare = std::memcmp(iL, iR, iCountR))
			return compare;
		return 1;
		}
	else
		{
		return std::memcmp(iL, iR, iCountL);
		}
	}

} // namespace ZooLib
