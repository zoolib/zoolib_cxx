cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/Zip)

set (SourceFiles	
	${SourceDir}/File_Zip.cpp
	${SourceDir}/File_Zip.h
	${SourceDir}/File_Zip_More.cpp
	${SourceDir}/File_Zip_More.h
	)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${SourceDir}" PREFIX "" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Platform)

add_library(
	ZooLib_Platform_Zip # Name of library
	STATIC # vs shared

	${SourceFiles}
	)
