cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/Hashing)

set (SourceFiles	
	${SourceDir}/MD5.cpp
	${SourceDir}/MD5.h
	${SourceDir}/SHA1.cpp
	${SourceDir}/SHA1.h
	${SourceDir}/SHA256.cpp
	${SourceDir}/SHA256.h
)

add_library(
	# Specifies the name of the library.
	ZooLib_Platform_Hashing

	# STATIC or SHARED
	STATIC

	${SourceFiles}
	)

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable)

# Put source files in a single IDE group
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${SourceDir}" PREFIX "" FILES ${SourceFiles})
