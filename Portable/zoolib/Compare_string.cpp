// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Compare_string.h"

#include "zoolib/Memory.h" // For sMemCompare
#include "zoolib/size_t.h" // For size_t

namespace ZooLib {

bool FastComparator_String::operator()(const std::string& iLeft, const std::string& iRight) const
	{
	if (const size_t lengthL = iLeft.length())
		{
		if (const size_t lengthR = iRight.length())
			{
			if (lengthL < lengthR)
				return true;
			else if (lengthR < lengthL)
				return false;
			else
				return sMemCompare(iLeft.data(), iRight.data(), lengthL) < 0;
			}
		else
			{
			return false;
			}
		}
	else
		{
		return not iRight.empty();
		}
	}

} // namespace ZooLib
