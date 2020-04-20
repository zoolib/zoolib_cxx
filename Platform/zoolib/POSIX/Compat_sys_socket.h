// Copyright (c) 2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_POSIX_compat_sys_socket_h__
#define __ZooLib_POSIX_compat_sys_socket_h__
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include <sys/socket.h> // For getpeername, SOCK_MAXADDRLEN

#if not defined(SOCK_MAXADDRLEN)
	#define SOCK_MAXADDRLEN (255)
#endif

#endif // ZCONFIG_SPI_Enabled(POSIX)

#endif // __ZooLib_POSIX_compat_sys_socket_h__
