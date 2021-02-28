cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Core/zoolib)

set (SourceFiles
	${SourceDir}/Any_T.h
	${SourceDir}/Any.h
	${SourceDir}/AnyBase.cpp
	${SourceDir}/AnyBase.h
	${SourceDir}/Atomic.h
	${SourceDir}/ByteSwap.h
	${SourceDir}/Callable_Macros.h
	${SourceDir}/Callable.h
	${SourceDir}/Chan.h
	${SourceDir}/ChanAspect.h
	${SourceDir}/ChanR_Bin.h
	${SourceDir}/ChanR_UTF.cpp
	${SourceDir}/ChanR_UTF.h
	${SourceDir}/ChanR.h
	${SourceDir}/ChanRPos_Bin.h
	${SourceDir}/ChanRU_Bin.h
	${SourceDir}/ChanRU_UTF.h
	${SourceDir}/ChanU_Bin.h
	${SourceDir}/ChanU_UTF.h
	${SourceDir}/ChanU.h
	${SourceDir}/ChanW_Bin.h
	${SourceDir}/ChanW_UTF.cpp
	${SourceDir}/ChanW_UTF.h
	${SourceDir}/ChanW.h
	${SourceDir}/Compare_T.h
	${SourceDir}/Compare.cpp
	${SourceDir}/Compare.h
	${SourceDir}/Compat_algorithm.h
	${SourceDir}/Compat_cmath.h
	${SourceDir}/Compat_MSVCStaticLib.h
	${SourceDir}/Compat_NonCopyable.h
	${SourceDir}/Compat_operator_bool.h
	${SourceDir}/Compat_string.h
	${SourceDir}/Compat_tuple.h
	${SourceDir}/Compat_unordered_map.h
	${SourceDir}/Counted.cpp
	${SourceDir}/Counted.h
	${SourceDir}/CountedVal.h
	${SourceDir}/CountedWithoutFinalize.cpp
	${SourceDir}/CountedWithoutFinalize.h
	${SourceDir}/CtorDtor.h
	${SourceDir}/Default.h
	${SourceDir}/Deleter.h
	${SourceDir}/DeriveFrom.h
	${SourceDir}/DList.h
	${SourceDir}/FunctionChain.h
	${SourceDir}/Memory.cpp
	${SourceDir}/Memory.h
	${SourceDir}/Multi.h
	${SourceDir}/Not.h
	${SourceDir}/Promise.h
	${SourceDir}/Safe.h
	${SourceDir}/Singleton.h
	${SourceDir}/size_t.h
	${SourceDir}/Startable.h
	${SourceDir}/Starter.h
	${SourceDir}/StdInt.h
	${SourceDir}/Stringf.cpp
	${SourceDir}/Stringf.h
	${SourceDir}/TagVal.h
	${SourceDir}/ThreadVal.h
	${SourceDir}/Time.cpp
	${SourceDir}/Time.h
	${SourceDir}/TypeList.h
	${SourceDir}/Unicode.cpp
	${SourceDir}/Unicode.h
	${SourceDir}/UnicodeCU.h
	${SourceDir}/UnicodeCU8.h
	${SourceDir}/UnicodePriv.h
	${SourceDir}/UnicodePrivB.h
	${SourceDir}/UnicodeString.h
	${SourceDir}/UnicodeString8.h
	${SourceDir}/Util_Chan.h
	${SourceDir}/Util_Relops.h
	${SourceDir}/Util_STL_Base_Map.h
	${SourceDir}/Util_STL_Base_MapOrSet.h
	${SourceDir}/Util_STL_Base_Seq.h
	${SourceDir}/Util_STL_Base_Set.h
	${SourceDir}/Util_STL_deque.h
	${SourceDir}/Util_STL_map.h
	${SourceDir}/Util_STL_set.h
	${SourceDir}/Util_STL_unordered_map.h
	${SourceDir}/Util_STL_unordered_set.h
	${SourceDir}/Util_STL_vector.h
	${SourceDir}/Util_STL.h
	${SourceDir}/ValueOnce.h
	${SourceDir}/ZCONFIG_API_Definitions.h
	${SourceDir}/ZCONFIG_API.h
	${SourceDir}/ZCONFIG_SPI_Definitions.h
	${SourceDir}/ZCONFIG_SPI.h
	${SourceDir}/zconfigd.h
	${SourceDir}/zconfigl.h
	${SourceDir}/ZDebug.cpp
	${SourceDir}/ZDebug.h
	${SourceDir}/ZMACRO_auto.h
	${SourceDir}/ZMACRO_decltype.h
	${SourceDir}/ZMACRO_FileFunctionLine.h
	${SourceDir}/ZMACRO_foreach.h
	${SourceDir}/ZP.h
	${SourceDir}/ZQ.h
	${SourceDir}/ZThread_pthread.cpp
	${SourceDir}/ZThread_pthread.h
	${SourceDir}/ZThread_T.h
	${SourceDir}/ZThread_Win.cpp
	${SourceDir}/ZThread_Win.h
	${SourceDir}/ZThread.cpp
	${SourceDir}/ZThread.h
	${SourceDir}/ZTypes.h
	)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${SourceDir}" PREFIX "" FILES ${SourceFiles})

add_library(
	ZooLib_Core # Name of library
	STATIC # vs shared

	${SourceFiles}
	)

include_directories(${SourceDir}/..)
