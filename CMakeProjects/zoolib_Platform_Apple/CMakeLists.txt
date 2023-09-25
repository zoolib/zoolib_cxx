cmake_minimum_required(VERSION 3.12) # so target_link_libraries will work with OBJECT libraries

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Platform/zoolib/Apple)

set (SourceFiles	
	${SourceDir}/AutoreleasePool.h
	${SourceDir}/Callable_Block.h
	${SourceDir}/Callable_ObjC.h
	${SourceDir}/Cartesian_CG.h
	${SourceDir}/Cartesian_NS.h
	${SourceDir}/CFNotification.cpp
	${SourceDir}/CFNotification.h
	${SourceDir}/CGData_Channer.cpp
	${SourceDir}/CGData_Channer.h
	${SourceDir}/Chan_Bin_CFStream.cpp
	${SourceDir}/Chan_Bin_CFStream.h
	${SourceDir}/Chan_UTF_CFString.cpp
	${SourceDir}/Chan_UTF_CFString.h
	${SourceDir}/Chan_UTF_NSString.h
	${SourceDir}/Chan_UTF_NSString.mm
	${SourceDir}/Coerce_CF.cpp
	${SourceDir}/Coerce_CF.h
	${SourceDir}/Compat_NSObject.h
	${SourceDir}/Data_CF.cpp
	${SourceDir}/Data_CF.h
	${SourceDir}/Data_NS.h
	${SourceDir}/Data_NS.mm
	${SourceDir}/Delegate.h
	${SourceDir}/Delegate.mm
	${SourceDir}/Map_CFPreferences.cpp
	${SourceDir}/Map_CFPreferences.h
	${SourceDir}/ObjC.h
	${SourceDir}/Pixmap_CGImage.cpp
	${SourceDir}/Pixmap_CGImage.h
	${SourceDir}/PullPush_CF.cpp
	${SourceDir}/PullPush_CF.h
	${SourceDir}/PullPush_NS.mm
	${SourceDir}/PullPush_NS.h
	${SourceDir}/Starter_CFRunLoop.cpp
	${SourceDir}/Starter_CFRunLoop.h
	${SourceDir}/Util_CF_Any.cpp
	${SourceDir}/Util_CF_Any.h
	${SourceDir}/Util_CF_Context.h
	${SourceDir}/Util_CF_ZZ.cpp
	${SourceDir}/Util_CF_ZZ.h
	${SourceDir}/Util_CF.cpp
	${SourceDir}/Util_CF.h
	${SourceDir}/Util_NS_ZZ.h
	${SourceDir}/Util_NS_ZZ.mm
	${SourceDir}/Util_NS.h
	${SourceDir}/Util_NS.mm
	${SourceDir}/Val_CF.cpp
	${SourceDir}/Val_CF.h
	${SourceDir}/Val_NS.h
	${SourceDir}/Val_NS.mm
	${SourceDir}/ZP_CF.cpp
	${SourceDir}/ZP_CF.h
	${SourceDir}/ZP_NS.h
	${SourceDir}/ZP_NS.mm
	${SourceDir}/ZP_xpc.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${ZOOLIB_CXX} ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Platform ${ZOOLIB_CXX}/Project)

add_library(ZooLib_Platform_Apple OBJECT

	${SourceFiles}
	)
