// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compare_string_h__
#define __ZooLib_Compare_string_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"

#include <string>

namespace ZooLib {

template <>
inline int sCompare_T(const std::string& iL, const std::string& iR)
	{ return iL.compare(iR); }

// =================================================================================================
#pragma mark - FastComparator_String

struct FastComparator_String : public std::binary_function<std::string,std::string,bool>
	{
	bool operator()(const std::string& iLeft, const std::string& iRight) const;
	};

} // namespace ZooLib

#endif // __ZooLib_Compare_string_h__
