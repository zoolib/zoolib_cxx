// Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZCONFIG_API_Definitions_h__
#define __ZCONFIG_API_Definitions_h__ 1
#include "zconfig.h"

#define ZCONFIG_API_Avail(facility) (ZCONFIG_API_Avail__##facility)
#define ZCONFIG_API_Desired(facility) (ZCONFIG_API_Desired__##facility)

#define ZCONFIG_API_Enabled(facility) \
	(ZCONFIG_API_Avail__##facility && ZCONFIG_API_Desired__##facility)

#endif // __ZCONFIG_API_Definitions_h__
