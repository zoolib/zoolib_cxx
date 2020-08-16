// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Safe_h__
#define __ZooLib_Safe_h__ 1
#include "zconfig.h"

#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Safe

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
