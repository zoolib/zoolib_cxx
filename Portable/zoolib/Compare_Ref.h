/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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
