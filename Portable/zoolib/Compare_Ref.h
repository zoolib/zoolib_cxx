// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compare_Ref_h__
#define __ZooLib_Compare_Ref_h__ 1
#include "zconfig.h"

#include "zoolib/Compare.h"
#include "zoolib/ZP.h"

#include <string.h> // For strcmp

namespace ZooLib {

template <class T>
int sCompare_Ref_T(const ZP<T>& iL, const ZP<T>& iR)
	{
	if (const T* l = iL.Get())
		{
		if (const T* r = iR.Get())
			{
			const char* typeName = typeid(*l).name();
			if (int compare = strcmp(typeName, typeid(*r).name()))
				return compare;

			return Comparer::sCompare(typeName, l, r);
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
int sCompare_T(const ZP<T>& iL, const ZP<T>& iR)
	{ return sCompare_Ref_T(iL, iR); }

} // namespace ZooLib

#endif // __ZooLib_Compare_Ref_h__
