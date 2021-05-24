cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Project/zoolib/Expr)

set (SourceFiles	
	${SourceDir}/Util_Expr_Bool_CNF.cpp
	${SourceDir}/Expr_Bool.cpp
	${SourceDir}/Expr_Bool.h
	${SourceDir}/Expr_Op_T.h
	${SourceDir}/Expr.cpp
	${SourceDir}/Expr.h
	${SourceDir}/Util_Expr_Bool_CNF.h
	${SourceDir}/Util_Strim_Expr_Bool.cpp
	${SourceDir}/Util_Strim_Expr_Bool.h
	${SourceDir}/Visitor_Expr_Bool_Do_Eval.cpp
	${SourceDir}/Visitor_Expr_Bool_Do_Eval.h
	${SourceDir}/Visitor_Expr_Bool_ToStrim.cpp
	${SourceDir}/Visitor_Expr_Bool_ToStrim.h
	${SourceDir}/Visitor_Expr_Op_Do_Transform_T.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Project)

add_library(ZooLib_Project_Expr STATIC

	${SourceFiles}
	)
