cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(ZDIR ${ZOOLIB_CXX}/Platform)

set (CppFiles	
	${ZDIR}/zoolib/JNI/JNI.cpp
)

add_library(
	# Specifies the name of the library.
	ZooLib_Platform_JNI

	# Sets the library as a shared library.
	STATIC

	${CppFiles}
	)

# Specifies a path to native header files.
include_directories(${ZDIR} ${ZOOLIB_CXX}/core ${ZOOLIB_CXX}/default_config)
