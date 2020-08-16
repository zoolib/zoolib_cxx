// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Python_Compat_Python_h__
#define __ZooLib_Python_Compat_Python_h__ 1
#include "zconfig.h"

// =================================================================================================
#pragma mark - defined(ZProjectHeader_Python)

#if defined(ZProjectHeader_Python)

#include ZProjectHeader_Python

#endif // defined(ZProjectHeader_Python)

// =================================================================================================
#pragma mark - not defined(ZProjectHeader_Python)

#if not defined(ZProjectHeader_Python)

extern "C" {
#include <Python.h>
} // extern "C"

#endif // not defined(ZProjectHeader_Python)

#endif // __ZooLib_Python_Compat_Python_h__
