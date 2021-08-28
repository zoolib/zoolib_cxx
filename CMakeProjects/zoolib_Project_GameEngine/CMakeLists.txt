cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Project/zoolib/GameEngine)

set (SourceFiles
	${SourceDir}/AssetCatalog.cpp
	${SourceDir}/AssetCatalog.h
	${SourceDir}/Cog_Buffer.cpp
	${SourceDir}/Cog_Buffer.h
	${SourceDir}/Cog_Button.cpp
	${SourceDir}/Cog_Button.h
	${SourceDir}/Cog_Delay.cpp
	${SourceDir}/Cog_Delay.h
	${SourceDir}/Cog_Distribute.cpp
	${SourceDir}/Cog_Distribute.h
	${SourceDir}/Cog_Group.cpp
	${SourceDir}/Cog_Group.h
	${SourceDir}/Cog_Indirect.cpp
	${SourceDir}/Cog_Indirect.h
	${SourceDir}/Cog_NookScope.cpp
	${SourceDir}/Cog_NookScope.h
	${SourceDir}/Cog_Random.cpp
	${SourceDir}/Cog_Random.h
	${SourceDir}/Cog_Replicate.cpp
	${SourceDir}/Cog_Replicate.h
	${SourceDir}/Cog_Sound.cpp
	${SourceDir}/Cog_Sound.h
	${SourceDir}/Cog_Toon.cpp
	${SourceDir}/Cog_Toon.h
	${SourceDir}/Cog.cpp
	${SourceDir}/Cog.h
	${SourceDir}/ColorMatrix.h
	${SourceDir}/CoT.cpp
	${SourceDir}/CoT.h
	${SourceDir}/DebugFlags.cpp
	${SourceDir}/DebugFlags.h
	${SourceDir}/Draw_GL_Shader.cpp
	${SourceDir}/Draw_GL_Shader.h
	${SourceDir}/Draw_GL.cpp
	${SourceDir}/Draw_GL.h
	${SourceDir}/DrawPreprocess.cpp
	${SourceDir}/DrawPreprocess.h
	${SourceDir}/FontCatalog.cpp
	${SourceDir}/FontCatalog.h
	${SourceDir}/Game.cpp
	${SourceDir}/Game.h
	${SourceDir}/Geometry.h
	${SourceDir}/Geometry2D.h
	${SourceDir}/Geometry3D.h
	${SourceDir}/Main_Android.cpp
	${SourceDir}/Main_Mac.mm
	${SourceDir}/Main_iOS.mm
	${SourceDir}/Nook.cpp
	${SourceDir}/Nook.h
	${SourceDir}/PitchAndCatch.cpp
	${SourceDir}/PitchAndCatch.h
	${SourceDir}/Rendered.cpp
	${SourceDir}/Rendered.h
	${SourceDir}/RenderGame.cpp
	${SourceDir}/RenderGame.h
	${SourceDir}/Sound_CoreAudio.cpp
	${SourceDir}/Sound_CoreAudio.h
	${SourceDir}/Sound.cpp
	${SourceDir}/Sound.h
	${SourceDir}/Texture_GL.cpp
	${SourceDir}/Texture_GL.h
	${SourceDir}/Texture.cpp
	${SourceDir}/Texture.h
	${SourceDir}/Toon.cpp
	${SourceDir}/Toon.h
	${SourceDir}/Touch.cpp
	${SourceDir}/Touch.h
	${SourceDir}/Tween_BlushGainMat.cpp
	${SourceDir}/Tween_BlushGainMat.h
	${SourceDir}/Tween_Geometry.h
	${SourceDir}/Tween_Rat.cpp
	${SourceDir}/Tween_Rat.h
	${SourceDir}/Tween.h
	${SourceDir}/Types.cpp
	${SourceDir}/Types.h
	${SourceDir}/UIView_Game.h
	${SourceDir}/UIView_Game.mm
	${SourceDir}/Util_Allocator.cpp
	${SourceDir}/Util_Allocator.h
	${SourceDir}/Util_AssetCatalog.cpp
	${SourceDir}/Util_AssetCatalog.h
	${SourceDir}/Util_Assets.cpp
	${SourceDir}/Util_Assets.h
	${SourceDir}/Util_FinderHider.cpp
	${SourceDir}/Util_FinderHider.h
#	${SourceDir}/Util_GrindAssets.cpp
#	${SourceDir}/Util_GrindAssets.h
	${SourceDir}/Util_TextData.cpp
	${SourceDir}/Util_TextData.h
	${SourceDir}/Util.cpp
	${SourceDir}/Util.h
	${SourceDir}/Val.cpp
	${SourceDir}/Val.h
	${SourceDir}/Visitor_Rendered_Std.cpp
	${SourceDir}/Visitor_Rendered_Std.h
	)

source_group("" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Project)

add_library(ZooLib_Project_GameEngine OBJECT
	${SourceFiles}
	)
