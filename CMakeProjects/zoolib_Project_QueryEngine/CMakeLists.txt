cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Project/zoolib/QueryEngine)

set (SourceFiles	
	${SourceDir}/Expr_Rel_Search.cpp
	${SourceDir}/Expr_Rel_Search.h
	${SourceDir}/Result.cpp
	${SourceDir}/Result.h
	${SourceDir}/ResultFromWalker.cpp
	${SourceDir}/ResultFromWalker.h
	${SourceDir}/Transform_Search.cpp
	${SourceDir}/Transform_Search.h
	${SourceDir}/Util_Strim_Result.cpp
	${SourceDir}/Util_Strim_Result.h
	${SourceDir}/Util_Strim_Walker.cpp
	${SourceDir}/Util_Strim_Walker.h
	${SourceDir}/Visitor_DoMakeWalker.cpp
	${SourceDir}/Visitor_DoMakeWalker.h
	${SourceDir}/Walker_Calc.cpp
	${SourceDir}/Walker_Calc.h
	${SourceDir}/Walker_Comment.cpp
	${SourceDir}/Walker_Comment.h
	${SourceDir}/Walker_Const.cpp
	${SourceDir}/Walker_Const.h
	${SourceDir}/Walker_Dee.cpp
	${SourceDir}/Walker_Dee.h
	${SourceDir}/Walker_Dum.cpp
	${SourceDir}/Walker_Dum.h
	${SourceDir}/Walker_Embed.cpp
	${SourceDir}/Walker_Embed.h
	${SourceDir}/Walker_Product.cpp
	${SourceDir}/Walker_Product.h
	${SourceDir}/Walker_Project.cpp
	${SourceDir}/Walker_Project.h
	${SourceDir}/Walker_Rename.cpp
	${SourceDir}/Walker_Rename.h
	${SourceDir}/Walker_Restrict.cpp
	${SourceDir}/Walker_Restrict.h
	${SourceDir}/Walker_Result.cpp
	${SourceDir}/Walker_Result.h
	${SourceDir}/Walker_Union.cpp
	${SourceDir}/Walker_Union.h
	${SourceDir}/Walker.cpp
	${SourceDir}/Walker.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Project)

add_library(ZooLib_Project_QueryEngine STATIC

	${SourceFiles}
	)
