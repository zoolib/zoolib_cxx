// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZMACRO_foreach_h__
#define __ZMACRO_foreach_h__ 1
#include "zconfig.h"

#ifndef foreachv
	#define foreachv(vardecl, container) for (vardecl : container)
#endif

#ifndef foreacha
	#define foreacha(varname, container) for (auto&& varname : container)
#endif

#endif // __ZMACRO_foreach_h__
