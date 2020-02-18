cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(ZDIR ${ZOOLIB_CXX}/Core)

set (CppFiles
	${ZDIR}/zoolib/AnyBase.cpp
	${ZDIR}/zoolib/ChanR_UTF.cpp
	${ZDIR}/zoolib/ChanW_UTF.cpp
	${ZDIR}/zoolib/Compare.cpp
	${ZDIR}/zoolib/Counted.cpp
	${ZDIR}/zoolib/CountedWithoutFinalize.cpp
	${ZDIR}/zoolib/Memory.cpp
	${ZDIR}/zoolib/Stringf.cpp
	${ZDIR}/zoolib/Time.cpp
	${ZDIR}/zoolib/Unicode.cpp
	${ZDIR}/zoolib/ZDebug.cpp
	${ZDIR}/zoolib/ZThread_pthread.cpp
	${ZDIR}/zoolib/ZThread_Win.cpp
	${ZDIR}/zoolib/ZThread.cpp
	)

add_library(
	# Specifies the name of the library.
	ZooLib_Core

	# STATIC or SHARED
	STATIC

	${CppFiles}
	)

# Specifies a path to native header files.
include_directories(${ZDIR} ${ZOOLIB_CXX}/default_config)
