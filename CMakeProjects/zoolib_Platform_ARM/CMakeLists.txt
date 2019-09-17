cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(ZDIR ${ZOOLIB_CXX}/Platform)

set (CppFiles	
	${ZDIR}/zoolib/ARM/Matrix_armv6.cpp
	${ZDIR}/zoolib/ARM/Matrix_armv7.cpp
)

add_library(
	# Specifies the name of the library.
	ZooLib_Platform_ARM

	# Sets the library as a shared library.
	STATIC

	${CppFiles}
	)

# Specifies a path to native header files.
include_directories(${ZDIR} ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/default_config)
