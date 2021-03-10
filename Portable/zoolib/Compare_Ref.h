// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compare_Ref_h__
#define __ZooLib_Compare_Ref_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"
#include "zoolib/ZP.h"

namespace ZooLib {

template <class T>
int sCompare_Ref_T(const ZP<T>& iL, const ZP<T>& iR)
	{
	if (const T* ll = iL.Get())
		{
		if (const T* rr = iR.Get())
			{
			if (int compare = strcmp(typeid(*ll).name(), typeid(*rr).name()))
				return compare;

			return sCompareNew_T(*ll, *rr);
			}
		else
			{
			return 1;
			}
		}
	else if (iR.Get())
		{ return -1; }
	else
		{ return 0; }
	}

template <class T>
int sCompareNew_T(const ZP<T>& iL, const ZP<T>& iR)
	{ return sCompare_Ref_T(iL, iR); }

} // namespace ZooLib

#endif // __ZooLib_Compare_Ref_h__
