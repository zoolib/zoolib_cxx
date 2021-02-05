cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/POSIX)

set (SourceFiles	
	${SourceDir}/Chan_Bin_POSIXFD.cpp
	${SourceDir}/Chan_Bin_POSIXFD.h
	${SourceDir}/FILE_Channer.cpp
	${SourceDir}/FILE_Channer.h
	${SourceDir}/File_POSIX.cpp
	${SourceDir}/File_POSIX.h
	${SourceDir}/Net_Internet_Socket.cpp
	${SourceDir}/Net_Internet_Socket.h
	${SourceDir}/Net_Socket.cpp
	${SourceDir}/Net_Socket.h
	${SourceDir}/SubProcess_POSIX.cpp
	${SourceDir}/SubProcess_POSIX.h
	${SourceDir}/Util_POSIX.cpp
	${SourceDir}/Util_POSIX.h
	${SourceDir}/Util_POSIXFD.cpp
	${SourceDir}/Util_POSIXFD.h
)

add_library(
	# Specifies the name of the library.
	ZooLib_Platform_POSIX

	# STATIC or SHARED
	STATIC

	${SourceFiles}
	)

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable)

# Put source files in a single IDE group
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${SourceDir}" PREFIX "" FILES ${SourceFiles})
