cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/Zip)

set (SourceFiles	
	${SourceDir}/Archive_Zip.cpp
	${SourceDir}/Archive_Zip.h
	${SourceDir}/Archive_Zip_More.cpp
	${SourceDir}/Archive_Zip_More.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Platform)

add_library(ZooLib_Platform_Zip STATIC

	${SourceFiles}
	)
