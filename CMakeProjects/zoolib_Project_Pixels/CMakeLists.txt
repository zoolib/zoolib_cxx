cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(SourceDir ${ZOOLIB_CXX}/Project/zoolib/Pixels)

set (SourceFiles	
	${SourceDir}/BigRegion.cpp
	${SourceDir}/BigRegion.h
	${SourceDir}/Blit.cpp
	${SourceDir}/Blit.h
	${SourceDir}/BlitPriv.h
	${SourceDir}/Cartesian_Geom.h
	${SourceDir}/Formats.cpp
	${SourceDir}/Formats.h
	${SourceDir}/Geom.h
	${SourceDir}/PixelDesc.cpp
	${SourceDir}/PixelDesc.h
	${SourceDir}/PixelIters.h
	${SourceDir}/Pixmap.cpp
	${SourceDir}/Pixmap.h
	${SourceDir}/PixmapCoder_PNG.cpp
	${SourceDir}/PixmapCoder_PNG.h
	${SourceDir}/Pixval.h
	${SourceDir}/Pixval2RGBA.cpp
	${SourceDir}/Pixval2RGBA.h
	${SourceDir}/PixvalAccessor.cpp
	${SourceDir}/PixvalAccessor.h
	${SourceDir}/PixvalDesc.h
	${SourceDir}/PixvalIters.cpp
	${SourceDir}/PixvalIters.h
	${SourceDir}/Raster.cpp
	${SourceDir}/Raster.h
	${SourceDir}/RasterDesc.cpp
	${SourceDir}/RasterDesc.h
	${SourceDir}/RasterOperations.cpp
	${SourceDir}/RasterOperations.h
	${SourceDir}/RGBA.h
	${SourceDir}/RGBA2Pixval.cpp
	${SourceDir}/RGBA2Pixval.h
	)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${SourceDir}" PREFIX "" FILES ${SourceFiles})

include_directories(${SourceDir}/../.. ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Portable ${ZOOLIB_CXX}/Project)

add_library(
	ZooLib_Project_Pixels # Name of library
	STATIC # vs shared

	${SourceFiles}
	)
