// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_Blit_h__
#define __ZooLib_Pixels_Blit_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/Geom.h"
#include "zoolib/Pixels/PixelDesc.h"
#include "zoolib/Pixels/Raster.h"

namespace ZooLib {
namespace Pixels {

/**
There are a lot of parameters to many of the methods in ZDCPixmapBlit. In
order to reduce the visual load we use the following abbreviations:
RD = RasterDesc
PD = PixelDesc
B = Bounds
*/

typedef RasterDesc RD;
typedef PixelDesc PD;

// =================================================================================================
#pragma mark -

void sMunge(void* iBaseAddress,
	const RasterDesc& iRasterDesc, const PixelDesc& iPixelDesc, const RectPOD& iBounds,
	MungeProc iMungeProc, void* iRefcon);

// =================================================================================================

enum EOp
	{
	eOp_Copy,
	eOp_Over,
	eOp_In,
	eOp_Plus
	};

/** Replicate iSourceB over iDestB, aligning iSourceOrigin with iDestB.TopLeft(). */
void sBlit(
	const RD& iSourceRD, const void* iSource, const RectPOD& iSourceB, const PD& iSourcePD,
	PointPOD iSourceOrigin,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	EOp iOperation);

/** Copy source to iDestB without replication. The actual rectangle drawn
will be smaller than iDestB if iSourceB is smaller. */
void sBlit(
	const RD& iSourceRD, const void* iSource, const RectPOD& iSourceB, const PD& iSourcePD,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	EOp iOperation);

/** Replicate iSourceB, with replicated matte. */
void sBlit(
	const RD& iSourceRD, const void* iSource, const RectPOD& iSourceB, const PD& iSourcePD,
	PointPOD iSourceOrigin,
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	PointPOD iMatteOrigin,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOperation);

/** Replicate iSourceB, matted. */
void sBlit(
	const RD& iSourceRD, const void* iSource, const RectPOD& iSourceB, const PD& iSourcePD,
	PointPOD iSourceOrigin,
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOperation);

/** Draw iSourceB into iDestB, with replicated matte. */
void sBlit(
	const RD& iSourceRD, const void* iSource, const RectPOD& iSourceB, const PD& iSourcePD,
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	PointPOD iMatteOrigin,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOperation);

/** Draw iSourceB masked by iMatteB into iDestB. */
void sBlit(
	const RD& iSourceRD, const void* iSource, const RectPOD& iSourceB, const PD& iSourcePD,
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOperation);

/** Fill iDestB with iColor. */
void sColor(
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	const RGBA& iColor,
	EOp iOperation);

/** Fill iDestB with iColor, matted. */
void sColor(
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	const RGBA& iColor,
	EOp iOperation);

/** Fill iDestB with iColor, with replicated matte. */
void sColor(
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	PointPOD iMatteOrigin,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	const RGBA& iColor,
	EOp iOperation);

/** Invert, replacing each pixel with white minus that pixel. */
void sInvert(const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD);

/** Multiply r,g, b & alpha by iAmount. */
void sOpaque(
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD, Comp iAmount);

/** Multiply r, g, b by iAmount, leaving alpha alone. */
void sDarken(
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD, Comp iAmount);

/** Multiply alpha by iAmount, leaving r,g,b alone. */
void sFade(
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD, Comp iAmount);

/** Take the alpha channel of matte, store it in alpha channel of dest,
pre-multiplying r,g,b of dest as it does so. */
void sApplyMatte(
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_Blit_h__
