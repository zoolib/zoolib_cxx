// http://www.zoolib.org
// Copyright (c) 2019 Andrew Green
// MIT License

#ifndef __ZooLib_pdesc_h__
#define __ZooLib_pdesc_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_MSVCStaticLib.h"

ZMACRO_MSVCStaticLib_Reference(PdescDistinguisher);

struct ZooLib_MarkAsUsed
	{
	static void spMarkIt(const void*);

	template <class T>
	ZooLib_MarkAsUsed(T thing)
		{
//		spMarkIt(0);
		}
	};

#define ZMACRO_UniqueMarkAsUsed(aa) \
	static ZooLib_MarkAsUsed ZMACRO_Concat(UniqueMarkAsUsed, __COUNTER__)(aa)

#define ZMACRO_pdesc(param) \
	void pdesc(param); \
	ZMACRO_UniqueMarkAsUsed(static_cast<void(&)(param)>(pdesc)); \
	void pdesc(param)

#endif // __ZooLib_pdesc_h__
