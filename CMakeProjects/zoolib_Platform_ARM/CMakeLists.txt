cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/ARM)

set (SourceFiles	
	${SourceDir}/Compat_arm.h
	${SourceDir}/Matrix_armv6.cpp
	${SourceDir}/Matrix_armv6.h
	${SourceDir}/Matrix_armv7.cpp
	${SourceDir}/Matrix_armv7.h
	)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${SourceDir}" PREFIX "" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Platform)

add_library(ZooLib_Platform_ARM STATIC

	${SourceFiles}
	)
