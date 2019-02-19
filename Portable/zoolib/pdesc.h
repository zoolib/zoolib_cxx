// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_pdesc_h__
#define __ZooLib_pdesc_h__ 1
#include "zconfig.h"

struct ZooLib_MarkAsUsed
	{
	template <class T>
	ZooLib_MarkAsUsed(T thing) {}
	};

#define ZMACRO_UniqueMarkAsUsed(aa) \
	static ZooLib_MarkAsUsed ZMACRO_Concat(UniqueMarkAsUsed, __COUNTER__)(aa)

#define ZMACRO_pdesc(param) \
	void pdesc(param); \
	ZMACRO_UniqueMarkAsUsed(static_cast<void(&)(param)>(pdesc)); \
	void pdesc(param)

#endif // __ZooLib_pdesc_h__
