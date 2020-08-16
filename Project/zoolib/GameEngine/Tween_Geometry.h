// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Tween_Geometry_h__
#define __ZooLib_GameEngine_Tween_Geometry_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_algorithm.h"
#include "zoolib/Tween_Std.h" // For Tween_Filter_Fun

#include "zoolib/GameEngine/Geometry3D.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - sTween_ScaleX

template <class Val>
ZP<Tween<Matrix<Val,4,4> > > sTween_ScaleX(const ZP<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sScale3X>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_ScaleY

template <class Val>
ZP<Tween<Matrix<Val,4,4> > > sTween_ScaleY(const ZP<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sScale3Y>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_ScaleZ

template <class Val>
ZP<Tween<Matrix<Val,4,4> > > sTween_ScaleZ(const ZP<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sScale3Z>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_TranslateX

template <class Val>
ZP<Tween<Matrix<Val,4,4> > > sTween_TranslateX(const ZP<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sTranslate3X>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_TranslateY

template <class Val>
ZP<Tween<Matrix<Val,4,4> > > sTween_TranslateY(const ZP<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sTranslate3Y>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_TranslateZ

template <class Val>
ZP<Tween<Matrix<Val,4,4> > > sTween_TranslateZ(const ZP<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sTranslate3Z>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_RotateX

template <class Val>
Matrix<Val,4,4> sTweenFun_Rotate2PiX(Val iVal)
	{ return sRotate3X<Val>(2 * M_PI * iVal); }

template <class Val>
ZP<Tween<Matrix<Val,4,4> > > sTween_RotateX(const ZP<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sTweenFun_Rotate2PiX>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_RotateY

template <class Val>
Matrix<Val,4,4> sTweenFun_Rotate2PiY(Val iVal)
	{ return sRotate3Y<Val>(2 * M_PI * iVal); }

template <class Val>
ZP<Tween<Matrix<Val,4,4> > > sTween_RotateY(const ZP<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sTweenFun_Rotate2PiY>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_RotateZ

template <class Val>
Matrix<Val,4,4> sTweenFun_Rotate2PiZ(Val iVal)
	{ return sRotate3Z<Val>(2 * M_PI * iVal); }

template <class Val>
ZP<Tween<Matrix<Val,4,4> > > sTween_RotateZ(const ZP<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sTweenFun_Rotate2PiZ>(iTween);
	return null;
	}

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Tween_Geometry_h__
