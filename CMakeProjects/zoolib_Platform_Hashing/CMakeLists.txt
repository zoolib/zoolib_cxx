cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/Hashing)

set (SourceFiles	
	${SourceDir}/ChanW_Bin_Hash.h
	${SourceDir}/MD5.cpp
	${SourceDir}/MD5.h
	${SourceDir}/SHA1.cpp
	${SourceDir}/SHA1.h
	${SourceDir}/SHA256.cpp
	${SourceDir}/SHA256.h
	)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${SourceDir}" PREFIX "" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Platform)

add_library(
	ZooLib_Platform_Hashing # Name of library
	STATIC # vs shared

	${SourceFiles}
	)
