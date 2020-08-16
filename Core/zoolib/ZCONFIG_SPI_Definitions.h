// Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZCONFIG_SPI_Definitions_h__
#define __ZCONFIG_SPI_Definitions_h__ 1
#include "zconfig.h"

#define ZCONFIG_SPI_Desired(facility) (ZCONFIG_SPI_Desired__##facility)
#define ZCONFIG_SPI_Avail(facility) (ZCONFIG_SPI_Avail__##facility)

#define ZCONFIG_SPI_Enabled(facility) \
	(ZCONFIG_SPI_Avail__##facility && ZCONFIG_SPI_Desired__##facility)

#endif // __ZCONFIG_SPI_Definitions_h__
