/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZooLib_GameEngine_Tween_Geometry_h__
#define __ZooLib_GameEngine_Tween_Geometry_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_algorithm.h"
#include "zoolib/Tween_Std.h" // For ZTween_Filter_Fun

#include "zoolib/GameEngine/Geometry3D.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - sTween_ScaleX

template <class Val>
ZRef<ZTween<Matrix<Val,4,4> > > sTween_ScaleX(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Filter_Fun<Matrix<Val,4,4>,Val,sScale3X>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_ScaleY

template <class Val>
ZRef<ZTween<Matrix<Val,4,4> > > sTween_ScaleY(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Filter_Fun<Matrix<Val,4,4>,Val,sScale3Y>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_ScaleZ

template <class Val>
ZRef<ZTween<Matrix<Val,4,4> > > sTween_ScaleZ(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Filter_Fun<Matrix<Val,4,4>,Val,sScale3Z>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_TranslateX

template <class Val>
ZRef<ZTween<Matrix<Val,4,4> > > sTween_TranslateX(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Filter_Fun<Matrix<Val,4,4>,Val,sTranslate3X>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_TranslateY

template <class Val>
ZRef<ZTween<Matrix<Val,4,4> > > sTween_TranslateY(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Filter_Fun<Matrix<Val,4,4>,Val,sTranslate3Y>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_TranslateZ

template <class Val>
ZRef<ZTween<Matrix<Val,4,4> > > sTween_TranslateZ(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Filter_Fun<Matrix<Val,4,4>,Val,sTranslate3Z>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_RotateX

template <class Val>
Matrix<Val,4,4> sTweenFun_Rotate2PiX(Val iVal)
	{ return sRotate3X<Val>(2 * M_PI * iVal); }

template <class Val>
ZRef<ZTween<Matrix<Val,4,4> > > sTween_RotateX(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Filter_Fun<Matrix<Val,4,4>,Val,sTweenFun_Rotate2PiX>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_RotateY

template <class Val>
Matrix<Val,4,4> sTweenFun_Rotate2PiY(Val iVal)
	{ return sRotate3Y<Val>(2 * M_PI * iVal); }

template <class Val>
ZRef<ZTween<Matrix<Val,4,4> > > sTween_RotateY(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Filter_Fun<Matrix<Val,4,4>,Val,sTweenFun_Rotate2PiY>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_RotateZ

template <class Val>
Matrix<Val,4,4> sTweenFun_Rotate2PiZ(Val iVal)
	{ return sRotate3Z<Val>(2 * M_PI * iVal); }

template <class Val>
ZRef<ZTween<Matrix<Val,4,4> > > sTween_RotateZ(const ZRef<ZTween<Val> >& iTween)
	{
	if (iTween)
		return new ZTween_Filter_Fun<Matrix<Val,4,4>,Val,sTweenFun_Rotate2PiZ>(iTween);
	return null;
	}

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Tween_Geometry_h__
