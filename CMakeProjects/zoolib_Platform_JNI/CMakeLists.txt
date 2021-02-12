cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/JNI)

set (SourceFiles	
	${SourceDir}/Chan_JavaInputStream.cpp
	${SourceDir}/Chan_JavaInputStream.h
	${SourceDir}/JNI.cpp
	${SourceDir}/JNI.h
	)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${SourceDir}" PREFIX "" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Platform)

add_library(
	ZooLib_Platform_JNI # Name of library
	STATIC # vs shared

	${SourceFiles}
	)
