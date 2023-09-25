cmake_minimum_required(VERSION 3.12) # so target_link_libraries will work with OBJECT libraries

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Project/zoolib/RelationalAlgebra)

set (SourceFiles	
# 	${SourceDir}/AsSQL.cpp
# 	${SourceDir}/AsSQL.h
	${SourceDir}/ColName.h
	${SourceDir}/Expr_Rel_Calc.cpp
	${SourceDir}/Expr_Rel_Calc.h
	${SourceDir}/Expr_Rel_Comment.cpp
	${SourceDir}/Expr_Rel_Comment.h
	${SourceDir}/Expr_Rel_Concrete.cpp
	${SourceDir}/Expr_Rel_Concrete.h
	${SourceDir}/Expr_Rel_Const.cpp
	${SourceDir}/Expr_Rel_Const.h
	${SourceDir}/Expr_Rel_Dee.cpp
	${SourceDir}/Expr_Rel_Dee.h
	${SourceDir}/Expr_Rel_Difference.cpp
	${SourceDir}/Expr_Rel_Difference.h
	${SourceDir}/Expr_Rel_Dum.cpp
	${SourceDir}/Expr_Rel_Dum.h
	${SourceDir}/Expr_Rel_Embed.cpp
	${SourceDir}/Expr_Rel_Embed.h
	${SourceDir}/Expr_Rel_Intersect.cpp
	${SourceDir}/Expr_Rel_Intersect.h
	${SourceDir}/Expr_Rel_Product.cpp
	${SourceDir}/Expr_Rel_Product.h
	${SourceDir}/Expr_Rel_Project.cpp
	${SourceDir}/Expr_Rel_Project.h
	${SourceDir}/Expr_Rel_Rename.cpp
	${SourceDir}/Expr_Rel_Rename.h
	${SourceDir}/Expr_Rel_Restrict.cpp
	${SourceDir}/Expr_Rel_Restrict.h
	${SourceDir}/Expr_Rel_Union.cpp
	${SourceDir}/Expr_Rel_Union.h
	${SourceDir}/Expr_Rel.cpp
	${SourceDir}/Expr_Rel.h
	${SourceDir}/GetRelHead.cpp
	${SourceDir}/GetRelHead.h
	${SourceDir}/PseudoMap.cpp
	${SourceDir}/PseudoMap.h
	${SourceDir}/RelHead.cpp
	${SourceDir}/RelHead.h
	${SourceDir}/Transform_ConsolidateRenames.cpp
	${SourceDir}/Transform_ConsolidateRenames.h
	${SourceDir}/Transform_DecomposeRestricts.cpp
	${SourceDir}/Transform_DecomposeRestricts.h
	${SourceDir}/Transform_PushDownRestricts.cpp
	${SourceDir}/Transform_PushDownRestricts.h
	${SourceDir}/Util_Rel_Operators.cpp
	${SourceDir}/Util_Rel_Operators.h
	${SourceDir}/Util_Strim_Rel.cpp
	${SourceDir}/Util_Strim_Rel.h
	${SourceDir}/Util_Strim_RelHead.cpp
	${SourceDir}/Util_Strim_RelHead.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Project)

add_library(ZooLib_Project_RelationalAlgebra OBJECT

	${SourceFiles}
	)
