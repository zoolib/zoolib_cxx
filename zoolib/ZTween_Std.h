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

#ifndef __ZTween_Std__
#define __ZTween_Std__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_algorithm.h" // for sMinMax
#include "zoolib/ZTween.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTween_Fun

template <class Return, Return(*Fun)(double)>
class ZTween_Fun
:	public ZTween<Return>
	{
public:
	virtual Return ValAt(double iTime)
		{ return Fun(sMinMax(0.0, iTime, 1.0)); }

	virtual double Duration()
		{ return 1; }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTween_Filter_Fun

template <class Return, class Param, Return(*Fun)(Param)>
class ZTween_Filter_Fun
:	public ZTween<Return>
	{
public:
	ZTween_Filter_Fun(const ZRef<ZTween<Param> >& iTween)
	:	fTween(iTween)
		{}

	virtual Return ValAt(double iTime)
		{ return Fun(fTween->ValAt(iTime)); }

	virtual double Duration()
		{ return fTween->Duration(); }

private:
	const ZRef<ZTween<Param> > fTween;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sTween_OneMinus

template <class Val>
Val sTweenFun_OneMinus(Val iVal)
	{ return 1 - iVal; }

template <class Val>
ZRef<ZTween<Val> > sTween_OneMinus(const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Filter_Fun<Val,Val,sTweenFun_OneMinus>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Squared

template <class Val>
Val sTweenFun_Squared(Val iVal)
	{ return iVal * iVal; }

template <class Val>
ZRef<ZTween<Val> > sTween_Squared(const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Filter_Fun<Val,Val,sTweenFun_Squared>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Linear

template <class Val>
Val sTweenFun_Linear(double iTime)
	{ return iTime; }	

template <class Val>
ZRef<ZTween<Val> > sTween_Linear()
	{ return new ZTween_Fun<Val,sTweenFun_Linear>; }

template <class Val>
ZRef<ZTween<Val> > sTween_Linear(double iDuration)
	{ return sTween_Rate(iDuration, sTween_Linear<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_Linear(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Linear<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_Linear(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_Linear<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Triangle

template <class Val>
Val sTweenFun_Triangle(double iTime)
	{ return 1 - fabs(iTime * 2 - 1); }	

template <class Val>
ZRef<ZTween<Val> > sTween_Triangle()
	{ return new ZTween_Fun<Val,sTweenFun_Triangle>; }

template <class Val>
ZRef<ZTween<Val> > sTween_Triangle(double iDuration)
	{ return sTween_Rate(iDuration, sTween_Triangle<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_Triangle(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Triangle<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_Triangle(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_Triangle<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Square

template <class Val>
Val sTweenFun_Square(double iTime)
	{ return iTime < 0.5 ? 0.0 : 1.0; }

template <class Val>
ZRef<ZTween<Val> > sTween_Square()
	{ return new ZTween_Fun<Val,sTweenFun_Square>; }

template <class Val>
ZRef<ZTween<Val> > sTween_Square(double iDuration)
	{ return sTween_Rate(iDuration, sTween_Square<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_Square(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Square<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_Square(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_Square<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Sin

template <class Val>
Val sTweenFun_Sin(double iTime)
	{ return sin(iTime * M_PI); }	

template <class Val>
ZRef<ZTween<Val> > sTween_Sin()
	{ return new ZTween_Fun<Val,sTweenFun_Sin>; }

template <class Val>
ZRef<ZTween<Val> > sTween_Sin(double iDuration)
	{ return sTween_Rate(iDuration, sTween_Sin<Val>()); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Cos

template <class Val>
Val sTweenFun_Cos(double iTime)
	{ return cos(iTime * M_PI); }	

template <class Val>
ZRef<ZTween<Val> > sTween_Cos()
	{ return new ZTween_Fun<Val,sTweenFun_Cos>; }

template <class Val>
ZRef<ZTween<Val> > sTween_Cos(double iDuration)
	{ return sTween_Rate(iDuration, sTween_Cos<Val>()); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_QuadIn

template <class Val>
Val sTweenFun_QuadIn(double iTime)
	{ return iTime * iTime; }	

template <class Val>
ZRef<ZTween<Val> > sTween_QuadIn()
	{ return new ZTween_Fun<Val,sTweenFun_QuadIn>; }

template <class Val>
ZRef<ZTween<Val> > sTween_QuadIn(double iDuration)
	{ return sTween_Rate(iDuration, sTween_QuadIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_QuadIn(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_QuadIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_QuadIn(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_QuadIn<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_QuadOut

template <class Val>
Val sTweenFun_QuadOut(double iTime)
	{ return -iTime * (iTime - 2); }	

template <class Val>
ZRef<ZTween<Val> > sTween_QuadOut()
	{ return new ZTween_Fun<Val,sTweenFun_QuadOut>; }

template <class Val>
ZRef<ZTween<Val> > sTween_QuadOut(double iDuration)
	{ return sTween_Rate(iDuration, sTween_QuadOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_QuadOut(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_QuadOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_QuadOut(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_QuadOut<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_CubicIn

template <class Val>
Val sTweenFun_CubicIn(double iTime)
	{ return iTime * iTime * iTime; }	

template <class Val>
ZRef<ZTween<Val> > sTween_CubicIn()
	{ return new ZTween_Fun<Val,sTweenFun_CubicIn>; }

template <class Val>
ZRef<ZTween<Val> > sTween_CubicIn(double iDuration)
	{ return sTween_Rate(iDuration, sTween_CubicIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_CubicIn(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_CubicIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_CubicIn(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_CubicIn<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_CubicOut

template <class Val>
Val sTweenFun_CubicOut(double iTime)
	{
	iTime -= 1;
	return iTime * iTime * iTime;
	}

template <class Val>
ZRef<ZTween<Val> > sTween_CubicOut()
	{ return new ZTween_Fun<Val,sTweenFun_CubicOut>; }

template <class Val>
ZRef<ZTween<Val> > sTween_CubicOut(double iDuration)
	{ return sTween_Rate(iDuration, sTween_CubicOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_CubicOut(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_CubicOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_CubicOut(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_CubicOut<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_QuartIn

template <class Val>
Val sTweenFun_QuartIn(double iTime)
	{ return iTime * iTime * iTime * iTime; }	

template <class Val>
ZRef<ZTween<Val> > sTween_QuartIn()
	{ return new ZTween_Fun<Val,sTweenFun_QuartIn>; }

template <class Val>
ZRef<ZTween<Val> > sTween_QuartIn(double iDuration)
	{ return sTween_Rate(iDuration, sTween_QuartIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_QuartIn(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_QuartIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_QuartIn(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_QuartIn<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_QuartOut

template <class Val>
Val sTweenFun_QuartOut(double iTime)
	{
	iTime -= 1;
	return -1 * (iTime * iTime * iTime * iTime - 1);
	}

template <class Val>
ZRef<ZTween<Val> > sTween_QuartOut()
	{ return new ZTween_Fun<Val,sTweenFun_QuartOut>; }

template <class Val>
ZRef<ZTween<Val> > sTween_QuartOut(double iDuration)
	{ return sTween_Rate(iDuration, sTween_QuartOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_QuartOut(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_QuartOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_QuartOut(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_QuartOut<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_QuintIn

template <class Val>
Val sTweenFun_QuintIn(double iTime)
	{ return iTime * iTime * iTime * iTime * iTime; }	

template <class Val>
ZRef<ZTween<Val> > sTween_QuintIn()
	{ return new ZTween_Fun<Val,sTweenFun_QuintIn>; }

template <class Val>
ZRef<ZTween<Val> > sTween_QuintIn(double iDuration)
	{ return sTween_Rate(iDuration, sTween_QuintIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_QuintIn(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_QuintIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_QuintIn(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_QuintIn<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_QuintOut

template <class Val>
Val sTweenFun_QuintOut(double iTime)
	{
	iTime -= 1;
	return iTime * iTime * iTime * iTime * iTime + 1;
	}	

template <class Val>
ZRef<ZTween<Val> > sTween_QuintOut()
	{ return new ZTween_Fun<Val,sTweenFun_QuintOut>; }

template <class Val>
ZRef<ZTween<Val> > sTween_QuintOut(double iDuration)
	{ return sTween_Rate(iDuration, sTween_QuintOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_QuintOut(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_QuintOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_QuintOut(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_QuintOut<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_SinIn

template <class Val>
Val sTweenFun_SinIn(double iTime)
	{ return -1 * cos(iTime * M_PI / 2) + 1; }	

template <class Val>
ZRef<ZTween<Val> > sTween_SinIn()
	{ return new ZTween_Fun<Val,sTweenFun_SinIn>; }

template <class Val>
ZRef<ZTween<Val> > sTween_SinIn(double iDuration)
	{ return sTween_Rate(iDuration, sTween_SinIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_SinIn(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_SinIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_SinIn(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_SinIn<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_SinOut

template <class Val>
Val sTweenFun_SinOut(double iTime)
	{ return sin(iTime * M_PI / 2); }

template <class Val>
ZRef<ZTween<Val> > sTween_SinOut()
	{ return new ZTween_Fun<Val,sTweenFun_SinOut>; }

template <class Val>
ZRef<ZTween<Val> > sTween_SinOut(double iDuration)
	{ return sTween_Rate(iDuration, sTween_SinOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_SinOut(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_SinOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_SinOut(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_SinOut<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_ExpoIn

template <class Val>
Val sTweenFun_ExpoIn(double iTime)
	{ return iTime <= 0 ? 0 : pow(2, 10 * (iTime - 1)); }	

template <class Val>
ZRef<ZTween<Val> > sTween_ExpoIn()
	{ return new ZTween_Fun<Val,sTweenFun_ExpoIn>; }

template <class Val>
ZRef<ZTween<Val> > sTween_ExpoIn(double iDuration)
	{ return sTween_Rate(iDuration, sTween_ExpoIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_ExpoIn(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_ExpoIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_ExpoIn(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_ExpoIn<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_ExpoOut

template <class Val>
Val sTweenFun_ExpoOut(double iTime)
	{ return iTime >= 1 ? 1 : (-pow(2, -10 * iTime) + 1); }

template <class Val>
ZRef<ZTween<Val> > sTween_ExpoOut()
	{ return new ZTween_Fun<Val,sTweenFun_ExpoOut>; }

template <class Val>
ZRef<ZTween<Val> > sTween_ExpoOut(double iDuration)
	{ return sTween_Rate(iDuration, sTween_ExpoOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_ExpoOut(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_ExpoOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_ExpoOut(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_ExpoOut<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_CircIn

template <class Val>
Val sTweenFun_CircIn(double iTime)
	{ return -1 * (sqrt(1 - iTime * iTime) - 1); }

template <class Val>
ZRef<ZTween<Val> > sTween_CircIn()
	{ return new ZTween_Fun<Val,sTweenFun_CircIn>; }

template <class Val>
ZRef<ZTween<Val> > sTween_CircIn(double iDuration)
	{ return sTween_Rate(iDuration, sTween_CircIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_CircIn(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_CircIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_CircIn(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_CircIn<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_CircOut

template <class Val>
Val sTweenFun_CircOut(double iTime)
	{
	iTime -= 1;
	return sqrt(1 - iTime * iTime);
	}

template <class Val>
ZRef<ZTween<Val> > sTween_CircOut()
	{ return new ZTween_Fun<Val,sTweenFun_CircOut>; }

template <class Val>
ZRef<ZTween<Val> > sTween_CircOut(double iDuration)
	{ return sTween_Rate(iDuration, sTween_CircOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_CircOut(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_CircOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_CircOut(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_CircOut<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_ElasticIn

template <class Val>
Val sTweenFun_ElasticIn(double iTime)
	{
	if (iTime <= 0)
		return 0;
	else if (iTime >= 1)
		return 1;

	iTime -= 1;
	return -1 * (pow(2, 10*iTime) * sin((iTime-0.3)*2*M_PI/0.3));
	}

template <class Val>
ZRef<ZTween<Val> > sTween_ElasticIn()
	{ return new ZTween_Fun<Val,sTweenFun_ElasticIn>; }

template <class Val>
ZRef<ZTween<Val> > sTween_ElasticIn(double iDuration)
	{ return sTween_Rate(iDuration, sTween_ElasticIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_ElasticIn(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_ElasticIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_ElasticIn(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_ElasticIn<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_ElasticOut

template <class Val>
Val sTweenFun_ElasticOut(double iTime)
	{
	if (iTime <= 0)
		return 0;
	else if (iTime >= 1)
		return 1;

	return pow(2, -10*iTime) * sin((iTime-0.3)*2*M_PI/0.3) + 1;
	}

template <class Val>
ZRef<ZTween<Val> > sTween_ElasticOut()
	{ return new ZTween_Fun<Val,sTweenFun_ElasticOut>; }

template <class Val>
ZRef<ZTween<Val> > sTween_ElasticOut(double iDuration)
	{ return sTween_Rate(iDuration, sTween_ElasticOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_ElasticOut(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_ElasticOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_ElasticOut(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_ElasticOut<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_BackIn

template <class Val>
class ZTween_Fun_BackIn
:	public ZTween<Val>
	{
public:
	ZTween_Fun_BackIn(Val iFactor) : fFactor(iFactor) {}

	virtual Val ValAt(double iTime)
		{ return iTime * iTime * ((fFactor + 1 ) * iTime - fFactor); }

	virtual double Duration()
		{ return 1; }

private:
	const Val fFactor;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_BackIn()
	{ return new ZTween_Fun_BackIn<Val>(1.70158); }

template <class Val>
ZRef<ZTween<Val> > sTween_BackIn(Val iFactor)
	{ return new ZTween_Fun_BackIn<Val>(iFactor); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_BackOut

template <class Val>
class ZTween_Fun_BackOut
:	public ZTween<Val>
	{
public:
	ZTween_Fun_BackOut(Val iFactor) : fFactor(iFactor) {}

	virtual Val ValAt(double iTime)
		{
		iTime -= 1;
		return iTime * iTime * ((fFactor + 1) * iTime + fFactor) + 1;
		}

	virtual double Duration()
		{ return 1; }

private:
	const Val fFactor;
	};

template <class Val>
ZRef<ZTween<Val> > sTween_BackOut()
	{ return new ZTween_Fun_BackOut<Val>(1.70158); }

template <class Val>
ZRef<ZTween<Val> > sTween_BackOut(Val iFactor)
	{ return new ZTween_Fun_BackOut<Val>(iFactor); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_BounceIn

template <class Val>
Val sTweenFun_BounceIn(double iTime)
	{
	if (iTime < 1 / 2.75)
		return 7.5625 * iTime * iTime;

	else if (iTime < 2 / 2.75)
		{
		iTime -= 1.5/2.75;
		return 7.5625 * iTime * iTime + .75;
		}

	else if (iTime < 2.5 / 2.75)
		{
		iTime -= 2.25/2.75;
		return 7.5625 * iTime * iTime + .9375;
		}
	else
		{
		iTime -= 2.625/2.75;
		return 7.5625 * iTime * iTime + .984375;
		}
	}

template <class Val>
ZRef<ZTween<Val> > sTween_BounceIn()
	{ return new ZTween_Fun<Val,sTweenFun_BounceIn>; }

template <class Val>
ZRef<ZTween<Val> > sTween_BounceIn(double iDuration)
	{ return sTween_Rate(iDuration, sTween_BounceIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_BounceIn(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_BounceIn<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_BounceIn(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_BounceIn<Val>())); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_BounceOut

template <class Val>
Val sTweenFun_BounceOut(double iTime)
	{ return sTweenFun_BounceIn<Val>(1 - iTime); }

template <class Val>
ZRef<ZTween<Val> > sTween_BounceOut()
	{ return new ZTween_Fun<Val,sTweenFun_BounceOut>; }

template <class Val>
ZRef<ZTween<Val> > sTween_BounceOut(double iDuration)
	{ return sTween_Rate(iDuration, sTween_BounceOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_BounceOut(Val iZeroVal, Val iOneVal)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_BounceOut<Val>()); }

template <class Val>
ZRef<ZTween<Val> > sTween_BounceOut(Val iZeroVal, Val iOneVal, double iDuration)
	{ return sTween_Range(iZeroVal, iOneVal, sTween_Rate(iDuration, sTween_BounceOut<Val>())); }

} // namespace ZooLib

#endif // __ZTween_Std__
