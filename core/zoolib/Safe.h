/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZooLib_Safe_h__
#define __ZooLib_Safe_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
// MARK: - Safe

template <class T>
class Safe
	{
public:
	Safe()
		{}

	Safe(const Safe& iOther)
	:	fVal(iOther)
		{}

	const Safe& operator=(const Safe& iOther)
		{
		const T newVal = iOther;
		ZAcqMtx acq(fMtx);
		fVal = newVal;
		return *this;
		}

	~Safe()
		{}

	Safe(const T& iOther)
	:	fVal(iOther)
		{}

	Safe& operator=(const T& iOther)
		{
		ZAcqMtx acq(fMtx);
		fVal = iOther;
		return *this;
		}

	operator T() const
		{
		ZAcqMtx acq(fMtx);
		return fVal;
		}

	const T Get() const
		{
		ZAcqMtx acq(fMtx);
		return fVal;
		}

	void Set(const T& iOther)
		{
		ZAcqMtx acq(fMtx);
		fVal = iOther;
		}

	const T GetSet(const T& iOther)
		{
		ZAcqMtx acq(fMtx);
		return sGetSet(fVal, iOther);
		}

	bool CAS(const T& iOld, const T& iNew)
		{
		ZAcqMtx acq(fMtx);
		if (not (fVal == iOld))
			return false;
		fVal = iNew;
		return true;
		}

private:
	mutable ZMtx fMtx;
	T fVal;
	};

} // namespace ZooLib

#endif // __ZooLib_Safe_h__
