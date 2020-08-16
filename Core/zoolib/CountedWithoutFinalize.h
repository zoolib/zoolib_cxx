// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_CountedWithoutFinalize_h__
#define __ZooLib_CountedWithoutFinalize_h__ 1
#include "zconfig.h"

#include "zoolib/Atomic.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - CountedWithoutFinalize

class CountedWithoutFinalize
	{
public:
	CountedWithoutFinalize();
	virtual ~CountedWithoutFinalize();

	void Retain() { sAtomic_Inc(&fRefCount); }
	void Release();
	bool IsShared() const;
	bool IsReferenced() const;

private:
	ZAtomic_t fRefCount;
	};

// =================================================================================================
#pragma mark - sRetain/sRelease for CountedWithoutFinalize derivatives

inline void sRetain(CountedWithoutFinalize& iObject)
	{ iObject.Retain(); }

inline void sRelease(CountedWithoutFinalize& iObject)
	{ iObject.Release(); }

inline void sCheck(CountedWithoutFinalize* iP)
	{}

} // namespace ZooLib

#endif // __ZooLib_CountedWithoutFinalize_h__
