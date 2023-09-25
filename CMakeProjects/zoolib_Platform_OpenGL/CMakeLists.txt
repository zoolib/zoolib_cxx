cmake_minimum_required(VERSION 3.12) # so target_link_libraries will work with OBJECT libraries

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/OpenGL)

set (SourceFiles	
	${SourceDir}/Compat_OpenGL.h
	${SourceDir}/Util.cpp
	${SourceDir}/Util.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${ZOOLIB_CXX} ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Platform)

add_library(ZooLib_Platform_OpenGL OBJECT

	${SourceFiles}
	)
