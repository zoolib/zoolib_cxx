cmake_minimum_required(VERSION 3.12) # so target_link_libraries will work with OBJECT libraries

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Project/zoolib/Dataspace)

set (SourceFiles	
	${SourceDir}/Daton_Val.cpp
	${SourceDir}/Daton_Val.h
	${SourceDir}/Daton.cpp
	${SourceDir}/Daton.h
	${SourceDir}/Melange.h
	${SourceDir}/MelangeRemoter.cpp
	${SourceDir}/MelangeRemoter.h
	${SourceDir}/Relater_Asyncify.cpp
	${SourceDir}/Relater_Asyncify.h
	${SourceDir}/Relater_Searcher.cpp
	${SourceDir}/Relater_Searcher.h
# 	${SourceDir}/Relater_SQLite.cpp
# 	${SourceDir}/Relater_SQLite.h
	${SourceDir}/Relater_Union.cpp
	${SourceDir}/Relater_Union.h
	${SourceDir}/Relater.cpp
	${SourceDir}/Relater.h
	${SourceDir}/RelsWatcher_Relater.cpp
	${SourceDir}/RelsWatcher_Relater.h
	${SourceDir}/RelsWatcher.h
	${SourceDir}/ResultHandler.cpp
	${SourceDir}/ResultHandler.h
	${SourceDir}/RowBoat.cpp
	${SourceDir}/RowBoat.h
	${SourceDir}/Searcher_Datons.cpp
	${SourceDir}/Searcher_Datons.h
	${SourceDir}/Searcher.cpp
	${SourceDir}/Searcher.h
	${SourceDir}/Sieve_Singleton.cpp
	${SourceDir}/Sieve_Singleton.h
	${SourceDir}/Tests.cpp
	${SourceDir}/Tests.h
	${SourceDir}/Types.cpp
	${SourceDir}/Types.h
	${SourceDir}/Util_Strim_Daton.cpp
	${SourceDir}/Util_Strim_Daton.h
	${SourceDir}/Util_Strim.cpp
	${SourceDir}/Util_Strim.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Project)

add_library(ZooLib_Project_Dataspace OBJECT

	${SourceFiles}
	)
