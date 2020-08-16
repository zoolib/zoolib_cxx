// Copyright (c) 2013 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZCompat_WinSock_h__
#define __ZCompat_WinSock_h__ 1

// -----
// Maddening crud to be able to reference winsock2 stuff
#ifndef NOMINMAX
#	define NOMINMAX
#endif
#include <winsock2.h>
#include "zoolib/ZCompat_Win.h"
#include <ws2tcpip.h>
// -----

#endif // __ZCompat_WinSock_h__
