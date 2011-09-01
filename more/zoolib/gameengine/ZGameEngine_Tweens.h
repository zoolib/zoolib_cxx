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

#ifndef __ZGameEngine_Tweens__
#define __ZGameEngine_Tweens__ 1
#include "zconfig.h"

#include "zoolib/gameengine/ZGameEngine_Geometry.h"
#include "zoolib/gameengine/ZGameEngine_Tween.h"

namespace ZooLib {
namespace ZGameEngine {

inline double spClamp(double iDouble)
	{ return std::min(std::max(0.0, iDouble), 1.0); }	

// =================================================================================================
#pragma mark -
#pragma mark * Tween_Fun

template <class Return, Return(*Fun)(double)>
class Tween_Fun
:	public Tween<Return>
	{
public:
	virtual Return ValAt(double iTime)
		{ return Fun(spClamp(iTime)); }

	virtual double Duration()
		{ return 1; }
	};

// =================================================================================================
#pragma mark -
#pragma mark * Tween_Filter_Fun

template <class Return, class Param, Return(*Fun)(Param)>
class Tween_Filter_Fun
:	public Tween<Return>
	{
public:
	Tween_Filter_Fun(const ZRef<ZGameEngine::Tween<Param> >& iTween)
	:	fTween(iTween)
		{}

	virtual Return ValAt(double iTime)
		{ return Fun(fTween->ValAt(iTime)); }

	virtual double Duration()
		{ return fTween->Duration(); }

	const ZRef<ZGameEngine::Tween<Param> > fTween;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sTween_OneMinus

template <class Val>
Val sFun_OneMinus(Val iVal)
	{ return 1 - iVal; }

template <class Val>
ZRef<Tween<Val> > sTween_OneMinus(const ZRef<Tween<Val> >& iTween)
	{ return new Tween_Filter_Fun<Val,Val,sFun_OneMinus>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_SawTooth

template <class Val>
Val sFun_SawTooth(double iTime)
	{ return spClamp(iTime); }	

template <class Val>
ZRef<Tween<Val> > sTween_SawTooth(double iDuration)
	{ return sDurationScale<Val>(iDuration, new Tween_Fun<Val,sFun_SawTooth>); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Triangle

template <class Val>
Val sFun_Triangle(double iTime)
	{ return 1 - fabs(spClamp(iTime) * 2 - 1); }	

template <class Val>
ZRef<Tween<Val> > sTween_Triangle(double iDuration)
	{ return sDurationScale<Val>(iDuration, new Tween_Fun<Val,sFun_Triangle>); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Square

template <class Val>
Val sFun_Square(double iTime)
	{ return iTime < 0.5 ? 0.0 : 1.0; }

template <class Val>
ZRef<Tween<Val> > sTween_Square(double iDuration)
	{ return sDurationScale<Val>(iDuration, new Tween_Fun<Val,sFun_Square>); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Sin

template <class Val>
Val sFun_Sin(double iTime)
	{ return sin(spClamp(iTime) * M_PI); }	

template <class Val>
ZRef<Tween<Val> > sTween_Sin(double iDuration)
	{ return sDurationScale<Val>(iDuration, new Tween_Fun<Val,sFun_Sin>); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Cos

template <class Val>
Val sFun_Cos(double iTime)
	{ return cos(spClamp(iTime) * M_PI); }	

template <class Val>
ZRef<Tween<Val> > sTween_Cos(double iDuration)
	{ return sDurationScale<Val>(iDuration, new Tween_Fun<Val,sFun_Cos>); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_RotateX

template <class Val>
Matrix<Val,4,4> sFun_Rotate2PiX(Val iVal)
	{ return sRotateX<Val>(iVal * 2 * M_PI); }

template <class Val>
ZRef<Tween<Matrix<Val,4,4> > > sTween_RotateX(const ZRef<Tween<Val> >& iTween)
	{ return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sFun_Rotate2PiX>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_RotateY

template <class Val>
Matrix<Val,4,4> sFun_Rotate2PiY(Val iVal)
	{ return sRotateY<Val>(iVal * 2 * M_PI); }

template <class Val>
ZRef<Tween<Matrix<Val,4,4> > > sTween_RotateY(const ZRef<Tween<Val> >& iTween)
	{ return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sFun_Rotate2PiY>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_RotateZ

template <class Val>
Matrix<Val,4,4> sFun_Rotate2PiZ(Val iVal)
	{ return sRotateZ<Val>(iVal * 2 * M_PI); }

template <class Val>
ZRef<Tween<Matrix<Val,4,4> > > sTween_RotateZ(const ZRef<Tween<Val> >& iTween)
	{ return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sFun_Rotate2PiZ>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_TranslateX

template <class Val>
ZRef<Tween<Matrix<Val,4,4> > > sTween_TranslateX(const ZRef<Tween<Val> >& iTween)
	{ return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sTranslateX>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_TranslateY

template <class Val>
ZRef<Tween<Matrix<Val,4,4> > > sTween_TranslateY(const ZRef<Tween<Val> >& iTween)
	{ return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sTranslateY>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_TranslateZ

template <class Val>
ZRef<Tween<Matrix<Val,4,4> > > sTween_TranslateZ(const ZRef<Tween<Val> >& iTween)
	{ return new Tween_Filter_Fun<Matrix<Val,4,4>,Val,sTranslateZ>(iTween); }

} // namespace ZGameEngine
} // namespace ZooLib

#endif // __ZGameEngine_Tweens__
