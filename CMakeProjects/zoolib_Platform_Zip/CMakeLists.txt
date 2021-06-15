if(NOT DEFINED ZooLib_IncludeDir_libzip)
	message(FATAL_ERROR "You must define ZooLib_IncludeDir_libzip")
endif()

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/Zip)

set (SourceFiles	
	${SourceDir}/Archive_Zip.cpp
	${SourceDir}/Archive_Zip.h
	${SourceDir}/Archive_Zip_More.cpp
	${SourceDir}/Archive_Zip_More.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${ZOOLIB_CXX} ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Platform)

include_directories(${ZooLib_IncludeDir_libzip})

add_library(ZooLib_Platform_Zip STATIC

	${SourceFiles}
	)
