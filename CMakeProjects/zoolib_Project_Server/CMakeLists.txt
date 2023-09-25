cmake_minimum_required(VERSION 3.12) # so target_link_libraries will work with OBJECT libraries

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Project/zoolib/Server)

set (SourceFiles	
	${SourceDir}/Roster.cpp
	${SourceDir}/Roster.h
	${SourceDir}/Server.cpp
	${SourceDir}/Server.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Project)

add_library(ZooLib_Project_Server OBJECT

	${SourceFiles}
	)
