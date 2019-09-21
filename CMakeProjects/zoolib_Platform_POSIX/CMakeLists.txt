cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(ZDIR ${ZOOLIB_CXX}/Platform)

set (CppFiles	
	${ZDIR}/zoolib/POSIX/Chan_Bin_POSIXFD.cpp
	${ZDIR}/zoolib/POSIX/FILE_Channer.cpp
	${ZDIR}/zoolib/POSIX/File_POSIX.cpp
	${ZDIR}/zoolib/POSIX/Net_Internet_Socket.cpp
	${ZDIR}/zoolib/POSIX/Net_Socket.cpp
	${ZDIR}/zoolib/POSIX/SubProcess_POSIX.cpp
	${ZDIR}/zoolib/POSIX/Util_POSIX.cpp
	${ZDIR}/zoolib/POSIX/Util_POSIXFD.cpp
)

add_library(
	# Specifies the name of the library.
	ZooLib_Platform_POSIX

	# STATIC or SHARED
	STATIC

	${CppFiles}
	)

# Specifies a path to native header files.
include_directories(${ZDIR} ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/default_config)
