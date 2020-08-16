// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Val_h__
#define __ZooLib_Val_h__ 1
#include "zconfig.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark - sPGeti, case-insensitive get

bool sEquali(const std::string& iLeft, const std::string& iRight);

template <class Map_t>
typename Map_t::Val_t* sPMuti(Map_t& iMap, const std::string& iNamei)
	{
	for (typename Map_t::Index_t ii = iMap.Begin(); ii != iMap.End(); ++ii)
		{
		if (sEquali(iMap.NameOf(ii), iNamei))
			return iMap.PMut(ii);
		}
	return nullptr;
	}

template <class Map_t>
const typename Map_t::Val_t* sPGeti(const Map_t& iMap, const std::string& iNamei)
	{
	for (typename Map_t::Index_t ii = iMap.Begin(); ii != iMap.End(); ++ii)
		{
		if (sEquali(iMap.NameOf(ii), iNamei))
			return iMap.PGet(ii);
		}
	return nullptr;
	}

} // namespace ZooLib

#endif // __ZooLib_Val_h__
