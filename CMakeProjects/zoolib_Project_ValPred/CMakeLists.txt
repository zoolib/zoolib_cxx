cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Project/zoolib/ValPred)

set (SourceFiles	
	${SourceDir}/Expr_Bool_ValPred.cpp
	${SourceDir}/Expr_Bool_ValPred.h
	${SourceDir}/Util_Expr_Bool_ValPred_Rename.cpp
	${SourceDir}/Util_Expr_Bool_ValPred_Rename.h
	${SourceDir}/Util_Strim_Expr_Bool_ValPred.cpp
	${SourceDir}/Util_Strim_Expr_Bool_ValPred.h
	${SourceDir}/Util_Strim_ValPred_DB.cpp
	${SourceDir}/Util_Strim_ValPred_DB.h
	${SourceDir}/ValPred_DB.cpp
	${SourceDir}/ValPred_DB.h
	${SourceDir}/ValPred_GetNames.cpp
	${SourceDir}/ValPred_GetNames.h
	${SourceDir}/ValPred_Renamed.cpp
	${SourceDir}/ValPred_Renamed.h
	${SourceDir}/ValPred.cpp
	${SourceDir}/ValPred.h
	${SourceDir}/Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches.cpp
	${SourceDir}/Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches.h
	${SourceDir}/Visitor_Expr_Bool_ValPred_DB_ToStrim.cpp
	${SourceDir}/Visitor_Expr_Bool_ValPred_DB_ToStrim.h
	${SourceDir}/Visitor_Expr_Bool_ValPred_Do_GetNames.cpp
	${SourceDir}/Visitor_Expr_Bool_ValPred_Do_GetNames.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Project)

add_library(
	ZooLib_Project_ValPred # Name of library
	STATIC # vs shared

	${SourceFiles}
	)
