// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_ObjC_h__
#define __ZooLib_Apple_ObjC_h__ 1
#include "zconfig.h"

#ifdef __OBJC__
	#include <objc/objc.h>
	#define ZMACRO_ObjCClass(p) @class p
#else
	typedef struct objc_class *Class;
	typedef struct objc_object { Class isa; } *id;
	typedef struct objc_selector *SEL;
	typedef id (*IMP)(id, SEL, ...);
	#define ZMACRO_ObjCClass(p) typedef struct objc_##p p
#endif

#endif // __ZooLib_Apple_ObjC_h__
