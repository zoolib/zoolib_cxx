// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZMACRO_auto_h__
#define __ZMACRO_auto_h__ 1
#include "zconfig.h"

#define ZMACRO_auto(name,expr) auto name(expr)
#define ZMACRO_auto_(name,expr) auto name=(expr)

#endif // __ZMACRO_auto_h__
