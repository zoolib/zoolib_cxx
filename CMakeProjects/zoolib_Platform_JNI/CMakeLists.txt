cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/JNI)

set (SourceFiles	
	${SourceDir}/Chan_JavaStream.cpp
	${SourceDir}/Chan_JavaStream.h
	${SourceDir}/JavaStream_Channer_Bin.cpp
	${SourceDir}/JavaStream_Channer_Bin.h
	${SourceDir}/JNI.cpp
	${SourceDir}/JNI.h
	${SourceDir}/PullPush_JNI.cpp
	${SourceDir}/PullPush_JNI.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${ZOOLIB_CXX} ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Platform)

add_library(ZooLib_Platform_JNI STATIC

	${SourceFiles}
	)
