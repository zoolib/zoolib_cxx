// Copyright (c) 2014-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ByteSwap_h__
#define __ZooLib_ByteSwap_h__ 1
#include "zconfig.h"

#include "zoolib/StdInt.h"

// From https://gist.github.com/panzi/6856583

#if (defined(_WIN16) || defined(_WIN32) || defined(_WIN64)) && !defined(__WINDOWS__)

#	define __WINDOWS__

#endif

#if defined(__linux__) || defined(__CYGWIN__)

#	include <endian.h>

#elif defined(__APPLE__)

#	include <libkern/OSByteOrder.h>

#	define htobe16(x) OSSwapHostToBigInt16(x)
#	define htole16(x) OSSwapHostToLittleInt16(x)
#	define be16toh(x) OSSwapBigToHostInt16(x)
#	define le16toh(x) OSSwapLittleToHostInt16(x)

#	define htobe32(x) OSSwapHostToBigInt32(x)
#	define htole32(x) OSSwapHostToLittleInt32(x)
#	define be32toh(x) OSSwapBigToHostInt32(x)
#	define le32toh(x) OSSwapLittleToHostInt32(x)

#	define htobe64(x) OSSwapHostToBigInt64(x)
#	define htole64(x) OSSwapHostToLittleInt64(x)
#	define be64toh(x) OSSwapBigToHostInt64(x)
#	define le64toh(x) OSSwapLittleToHostInt64(x)

#	define __BYTE_ORDER    BYTE_ORDER
#	define __BIG_ENDIAN    BIG_ENDIAN
#	define __LITTLE_ENDIAN LITTLE_ENDIAN
#	define __PDP_ENDIAN    PDP_ENDIAN

#elif defined(__OpenBSD__)

#	include <sys/endian.h>

#elif defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__)

#	include <sys/endian.h>

#	define be16toh(x) betoh16(x)
#	define le16toh(x) letoh16(x)

#	define be32toh(x) betoh32(x)
#	define le32toh(x) letoh32(x)

#	define be64toh(x) betoh64(x)
#	define le64toh(x) letoh64(x)

#elif defined(__WINDOWS__)

#	include <winsock2.h>
#	include <sys/param.h>

#	if BYTE_ORDER == LITTLE_ENDIAN

#		define htobe16(x) htons(x)
#		define htole16(x) (x)
#		define be16toh(x) ntohs(x)
#		define le16toh(x) (x)

#		define htobe32(x) htonl(x)
#		define htole32(x) (x)
#		define be32toh(x) ntohl(x)
#		define le32toh(x) (x)

#		define htobe64(x) htonll(x)
#		define htole64(x) (x)
#		define be64toh(x) ntohll(x)
#		define le64toh(x) (x)

#	elif BYTE_ORDER == BIG_ENDIAN

		/* that would be xbox 360 */
#		define htobe16(x) (x)
#		define htole16(x) __builtin_bswap16(x)
#		define be16toh(x) (x)
#		define le16toh(x) __builtin_bswap16(x)

#		define htobe32(x) (x)
#		define htole32(x) __builtin_bswap32(x)
#		define be32toh(x) (x)
#		define le32toh(x) __builtin_bswap32(x)

#		define htobe64(x) (x)
#		define htole64(x) __builtin_bswap64(x)
#		define be64toh(x) (x)
#		define le64toh(x) __builtin_bswap64(x)

#	else

#		error byte order not supported

#	endif

#	define __BYTE_ORDER    BYTE_ORDER
#	define __BIG_ENDIAN    BIG_ENDIAN
#	define __LITTLE_ENDIAN LITTLE_ENDIAN
#	define __PDP_ENDIAN    PDP_ENDIAN

#else

#	error platform not supported

#endif

namespace ZooLib {

// =================================================================================================
#pragma mark -

template <class T, int size_p = sizeof(T)>
struct ByteSwapped_t
	{ static T sSwapped(const T& iT); };

template <class T>
struct ByteSwapped_t<T,1>
	{ static T sSwapped(const T& iT) { return iT; }; };

#define ZMACRO_Swapper(function, inttype, value) \
	union { T asT; inttype asInt; } conv; \
	conv.asT = value; \
	conv.asInt = function(conv.asInt); \
	return conv.asT

#if BYTE_ORDER == LITTLE_ENDIAN

	template <class T>
	struct ByteSwapped_t<T,2>
		{ static T sSwapped(const T& iT) { ZMACRO_Swapper(htobe16, int16, iT); }; };

	template <class T>
	struct ByteSwapped_t<T,4>
		{ static T sSwapped(const T& iT) { ZMACRO_Swapper(htobe32, int32, iT); }; };

	template <class T>
	struct ByteSwapped_t<T,8>
		{ static T sSwapped(const T& iT) { ZMACRO_Swapper(htobe64, int64, iT); }; };

#else

	template <class T>
	struct ByteSwapped_t<T,2>
		{ static T sSwapped(const T& iT) { ZMACRO_Swapper(htole16, int16, iT); }; };

	template <class T>
	struct ByteSwapped_t<T,4>
		{ static T sSwapped(const T& iT) { ZMACRO_Swapper(htole32, int32, iT); }; };

	template <class T>
	struct ByteSwapped_t<T,8>
		{ static T sSwapped(const T& iT) { ZMACRO_Swapper(htole64, int64, iT); }; };

#endif

#undef ZMACRO_Swapper

template <class T>
T sByteSwapped(T iT)
	{ return ByteSwapped_t<T>::sSwapped(iT); }

} // namespace ZooLib

#endif // __ZooLib_ByteSwap_h__
