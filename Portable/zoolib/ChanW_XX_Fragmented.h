// Copyright (c) 2007 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanW_XX_Fragmented_h__
#define __ZooLib_ChanW_XX_Fragmented_h__ 1
#include "zconfig.h"

#include "zoolib/ChanFilter.h"
#include "zoolib/ChanW.h"
#include "zoolib/Compat_algorithm.h" // For min

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanW_XX_Fragmented

/// A write filter stream that passes on data in chunks no larger than a limit.

template <class Chan_p>
class ChanW_XX_Fragmented
:	public virtual ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
	typedef typename Chan_p::Element_t EE;
public:
	ChanW_XX_Fragmented(const Chan_p& iChan, size_t iFragmentCount)
	:	inherited(iChan)
	,	fFragmentCount(iFragmentCount)
		{}

	virtual size_t Write(const EE* iSource, size_t iCount)
		{ return sWrite(inherited::pGetChan(), iSource, min(fFragmentCount, iCount)); }

protected:
	const size_t fFragmentCount;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanW_XX_Fragmented_h__
