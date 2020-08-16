// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/CountedWithoutFinalize.h"

#include "zoolib/ZDebug.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - CountedWithoutFinalize

CountedWithoutFinalize::CountedWithoutFinalize()
:	fRefCount(0)
	{}

CountedWithoutFinalize::~CountedWithoutFinalize()
	{
	ZAssertStopf(1, sAtomic_Get(&fRefCount) == 0,
		"Non-zero refcount at destruction, it is %d", sAtomic_Get(&fRefCount));
	}

void CountedWithoutFinalize::Release()
	{
	if (sAtomic_DecAndTest(&fRefCount))
		delete this;
	}

bool CountedWithoutFinalize::IsShared() const
	{ return sAtomic_Get(&fRefCount) > 1; }

bool CountedWithoutFinalize::IsReferenced() const
	{ return sAtomic_Get(&fRefCount) > 0; }

} // namespace ZooLib
