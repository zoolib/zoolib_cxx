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

#ifndef __ZSafe__
#define __ZSafe__ 1
#include "zconfig.h"

#include "zoolib/ZGetSet.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZSafe

template <class T>
class ZSafe
	{
public:
	ZSafe()
		{}

	ZSafe(const ZSafe& iOther)
	:	fVal(iOther)
		{}

	const ZSafe& operator=(const ZSafe& iOther)
		{
		const T newVal = iOther;
		ZAcqMtx acq(fMtx);
		fVal = newVal;
		return *this;
		}

	~ZSafe()
		{}

	ZSafe(const T& iOther)
	:	fVal(iOther)
		{}

	ZSafe& operator=(const T& iOther)
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

	T Get() const
		{
		ZAcqMtx acq(fMtx);
		return fVal;
		}

	void Set(const T& iOther)
		{
		ZAcqMtx acq(fMtx);
		fVal = iOther;
		}

	T GetSet(const T& iOther)
		{
		ZAcqMtx acq(fMtx);
		return ZooLib::GetSet(fVal, iOther);
		}

	bool CompareAndSwap(const T& iOld, const T& iNew)
		{
		ZAcqMtx acq(fMtx);
		if (fVal == iOld)
			{
			fVal = iNew;
			return true;
			}
		return false;
		}

private:
	mutable ZMtx fMtx;
	T fVal;
	};

} // namespace ZooLib

#endif // __ZSafe__
