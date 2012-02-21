/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZVal_h__
#define __ZVal_h__ 1
#include "zconfig.h"

#include <string>

namespace ZooLib {

// =================================================================================================
// MARK: - sPGeti, case-insensitive get

bool sEquali(const std::string& iLeft, const std::string& iRight);

template <class Map_t>
typename Map_t::Val* sPGetMutablei(Map_t& iMap, const std::string& iNamei)
	{
	for (typename Map_t::Index_t ii = iMap.Begin(); ii != iMap.End(); ++ii)
		{
		if (sEquali(iMap.NameOf(ii), iNamei))
			return iMap.PGetMutable(ii);
		}
	return nullptr;
	}

template <class Map_t>
const typename Map_t::Val* sPGeti(const Map_t& iMap, const std::string& iNamei)
	{
	for (typename Map_t::Index_t ii = iMap.Begin(); ii != iMap.End(); ++ii)
		{
		if (sEquali(iMap.NameOf(ii), iNamei))
			return iMap.PGet(ii);
		}
	return nullptr;
	}

} // namespace ZooLib

#endif // __ZVal_h__
