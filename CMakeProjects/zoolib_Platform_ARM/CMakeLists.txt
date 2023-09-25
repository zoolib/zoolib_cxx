cmake_minimum_required(VERSION 3.12) # so target_link_libraries will work with OBJECT libraries

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/ARM)

set (SourceFiles	
	${SourceDir}/Compat_arm.h
	${SourceDir}/Matrix_armv6.cpp
	${SourceDir}/Matrix_armv6.h
	${SourceDir}/Matrix_armv7.cpp
	${SourceDir}/Matrix_armv7.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${ZOOLIB_CXX} ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Platform)

add_library(ZooLib_Platform_ARM OBJECT

	${SourceFiles}
	)
