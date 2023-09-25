cmake_minimum_required(VERSION 3.12) # so target_link_libraries will work with OBJECT libraries

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/JNI)

set (SourceFiles	
	${SourceDir}/Chan_JavaStream.cpp
	${SourceDir}/Chan_JavaStream.h
	${SourceDir}/JavaStream_Channer_Bin.cpp
	${SourceDir}/JavaStream_Channer_Bin.h
	${SourceDir}/JNI.cpp
	${SourceDir}/JNI.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${ZOOLIB_CXX} ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Platform)

add_library(ZooLib_Platform_JNI OBJECT

	${SourceFiles}
	)
