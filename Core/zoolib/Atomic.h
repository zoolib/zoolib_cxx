// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Atomic_h__
#define __ZooLib_Atomic_h__ 1
#include "zconfig.h"

#include <atomic>

namespace ZooLib {

// =================================================================================================
#pragma mark - sAtomic_CAS

template <class T>
inline bool sAtomic_CAS(std::atomic<T>* ioAtomic, T iOldValue, T iNewValue)
	{
	return std::atomic_compare_exchange_strong_explicit<T>(ioAtomic, &iOldValue, iNewValue,
		std::memory_order_relaxed, std::memory_order_relaxed);
	}

template <class T>
inline bool sAtomicPtr_CAS(T** iPtrAddress, T* iOldValue, T* iNewValue)
	{ return sAtomic_CAS<T*>((std::atomic<T*>*)iPtrAddress, iOldValue, iNewValue); }

// =================================================================================================
#pragma mark - ZAtomic_t

template <class T>
inline T sAtomic_GetSet(std::atomic<T>* ioAtomic, T iOther)
	{ return std::atomic_exchange(ioAtomic, iOther); }

typedef std::atomic<int> ZAtomic_t;

inline int sAtomic_Get(const ZAtomic_t* iAtomic)
	{ return *iAtomic; }

inline void sAtomic_Set(ZAtomic_t* iAtomic, int iParam)
	{ *iAtomic = iParam; }

inline int sAtomic_Add(ZAtomic_t* iAtomic, int iParam)
	{ return std::atomic_fetch_add(iAtomic, iParam); }

inline void sAtomic_Inc(ZAtomic_t* iAtomic)
	{ ++*iAtomic; }

inline void sAtomic_Dec(ZAtomic_t* iAtomic)
	{ --*iAtomic; }

inline bool sAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{ return 0 == --*iAtomic; }

} // namespace ZooLib

#endif // __ZooLib_Atomic_h__
