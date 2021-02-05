cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/JNI)

set (SourceFiles	
	${SourceDir}/Chan_JavaInputStream.cpp
	${SourceDir}/Chan_JavaInputStream.h
	${SourceDir}/JNI.cpp
	${SourceDir}/JNI.h
)

add_library(
	# Specifies the name of the library.
	ZooLib_Platform_JNI

	# STATIC or SHARED
	STATIC

	${SourceFiles}
	)

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable)

# Put source files in a single IDE group
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${SourceDir}" PREFIX "" FILES ${SourceFiles})
