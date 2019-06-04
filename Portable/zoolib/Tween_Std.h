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

#ifndef __ZooLib_Tween_Std_h__
#define __ZooLib_Tween_Std_h__ 1
#include "zconfig.h"

#include "zoolib/Tween.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Tween_Fun

template <class Val, Val(*Fun)(double)>
class Tween_Fun
:	public Tween<Val>
	{
public:
	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace >= 0 && iPlace < 1.0)
			return Fun(iPlace);
		return null;
		}

//	virtual double Weight()
//		{ return 1; }
	};

// =================================================================================================
#pragma mark - Tween_Filter_Fun

template <class Val, class Param, Val(*Fun)(Param)>
class Tween_Filter_Fun
:	public Tween<Val>
	{
public:
	Tween_Filter_Fun(const ZRef<Tween<Param> >& iTween)
	:	fTween(iTween)
		{}

	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (ZQ<Param> theQ = fTween->QValAt(iPlace))
			return Fun(*theQ);
		return null;
		}

	virtual double Weight()
		{ return fTween->Weight(); }

private:
	const ZRef<Tween<Param> > fTween;
	};

// =================================================================================================
#pragma mark - Tween_Filter_Coerce

template <class Val, class Param>
class Tween_Filter_Coerce
:	public Tween<Val>
	{
public:
	Tween_Filter_Coerce(const ZRef<Tween<Param> >& iTween)
	:	fTween(iTween)
		{}

	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (ZQ<Param> theQ = fTween->QValAt(iPlace))
			return Val(*theQ);
		return null;
		}

	virtual double Weight()
		{ return fTween->Weight(); }

private:
	const ZRef<Tween<Param> > fTween;
	};

template <class Val, class Param>
ZRef<Tween<Val> > sTween_Filter_Coerce(const ZRef<Tween<Param> >& iTween)
	{
	if (iTween)
		return new Tween_Filter_Coerce<Val,Param>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_OneMinus

template <class Val>
Val spTweenFun_OneMinus(Val iVal)
	{ return 1 - iVal; }

template <class Val>
ZRef<Tween<Val> > sTween_OneMinus(const ZRef<Tween<Val> >& iTween)
	{
	if (iTween)
		return new Tween_Filter_Fun<Val,Val,spTweenFun_OneMinus>(iTween);
	return null;
	}

// =================================================================================================
#pragma mark - sTween_Linear

template <class Val>
Val spTweenFun_Linear(double iPlace)
	{ return iPlace; }

template <class Val>
ZRef<Tween<Val> > sTween_Linear()
	{ return new Tween_Fun<Val,spTweenFun_Linear>; }

template <class Val>
ZRef<Tween<Val> > sTween_Linear(Val iZeroVal, Val iOneVal)
	{ return sTween_ValRange(iZeroVal, iOneVal, sTween_Linear<Val>()); }

template <class Val>
ZRef<Tween<Val> > sTween_Linear(Val iZeroVal, Val iOneVal, double iWeight)
	{ return sTween_ValRange(iZeroVal, iOneVal, sTween_WeightScale(iWeight, sTween_Linear<Val>())); }

// =================================================================================================
#pragma mark - sTween_Triangle

template <class Val>
Val spTweenFun_Triangle(double iPlace)
	{ return 1 - fabs(iPlace * 2 - 1); }

template <class Val>
ZRef<Tween<Val> > sTween_Triangle()
	{ return new Tween_Fun<Val,spTweenFun_Triangle>; }

// =================================================================================================
#pragma mark - sTween_Square

template <class Val>
Val spTweenFun_Square(double iPlace)
	{ return iPlace < 0.5 ? 0.0 : 1.0; }

template <class Val>
ZRef<Tween<Val> > sTween_Square()
	{ return new Tween_Fun<Val,spTweenFun_Square>; }

// =================================================================================================
#pragma mark - sTween_Sin

template <class Val>
Val spTweenFun_Sin(double iPlace)
	{ return sin(2 * M_PI * iPlace); }

template <class Val>
ZRef<Tween<Val> > sTween_Sin()
	{ return new Tween_Fun<Val,spTweenFun_Sin>; }

// =================================================================================================
#pragma mark - sTween_Cos

template <class Val>
Val spTweenFun_Cos(double iPlace)
	{ return cos(2 * M_PI * iPlace); }

template <class Val>
ZRef<Tween<Val> > sTween_Cos()
	{ return new Tween_Fun<Val,spTweenFun_Cos>; }

// =================================================================================================
#pragma mark - sTween_Erf

// <https://en.wikipedia.org/wiki/Error_function>

template <class Val>
Val spTweenFun_Erf(double iPlace)
	{ return erf(2 * iPlace); }

template <class Val>
ZRef<Tween<Val> > sTween_Erf()
	{ return new Tween_Fun<Val,spTweenFun_Erf>; }

// =================================================================================================
#pragma mark - sTween_Sqrt

template <class Val>
Val spTweenFun_Sqrt(double iPlace)
	{ return iPlace < 0.0 ? 0.0 : iPlace > 1.0 ? 1.0 : sqrt(iPlace); }

template <class Val>
ZRef<Tween<Val> > sTween_Sqrt()
	{ return new Tween_Fun<Val,spTweenFun_Sqrt>; }

// =================================================================================================
#pragma mark - sTween_QuadIn

template <class Val>
Val spTweenFun_QuadIn(double iPlace)
	{ return iPlace * iPlace; }

template <class Val>
ZRef<Tween<Val> > sTween_QuadIn()
	{ return new Tween_Fun<Val,spTweenFun_QuadIn>; }

// =================================================================================================
#pragma mark - sTween_QuadOut

template <class Val>
Val spTweenFun_QuadOut(double iPlace)
	{ return -iPlace * (iPlace - 2); }

template <class Val>
ZRef<Tween<Val> > sTween_QuadOut()
	{ return new Tween_Fun<Val,spTweenFun_QuadOut>; }

// =================================================================================================
#pragma mark - sTween_CubicIn

template <class Val>
Val spTweenFun_CubicIn(double iPlace)
	{ return iPlace * iPlace * iPlace; }

template <class Val>
ZRef<Tween<Val> > sTween_CubicIn()
	{ return new Tween_Fun<Val,spTweenFun_CubicIn>; }

// =================================================================================================
#pragma mark - sTween_CubicOut

template <class Val>
Val spTweenFun_CubicOut(double iPlace)
	{
	iPlace -= 1;
	return iPlace * iPlace * iPlace;
	}

template <class Val>
ZRef<Tween<Val> > sTween_CubicOut()
	{ return new Tween_Fun<Val,spTweenFun_CubicOut>; }

// =================================================================================================
#pragma mark - sTween_QuartIn

template <class Val>
Val spTweenFun_QuartIn(double iPlace)
	{ return iPlace * iPlace * iPlace * iPlace; }

template <class Val>
ZRef<Tween<Val> > sTween_QuartIn()
	{ return new Tween_Fun<Val,spTweenFun_QuartIn>; }

// =================================================================================================
#pragma mark - sTween_QuartOut

template <class Val>
Val spTweenFun_QuartOut(double iPlace)
	{
	iPlace -= 1;
	return -1 * (iPlace * iPlace * iPlace * iPlace - 1);
	}

template <class Val>
ZRef<Tween<Val> > sTween_QuartOut()
	{ return new Tween_Fun<Val,spTweenFun_QuartOut>; }

// =================================================================================================
#pragma mark - sTween_QuintIn

template <class Val>
Val spTweenFun_QuintIn(double iPlace)
	{ return iPlace * iPlace * iPlace * iPlace * iPlace; }

template <class Val>
ZRef<Tween<Val> > sTween_QuintIn()
	{ return new Tween_Fun<Val,spTweenFun_QuintIn>; }

// =================================================================================================
#pragma mark - sTween_QuintOut

template <class Val>
Val spTweenFun_QuintOut(double iPlace)
	{
	iPlace -= 1;
	return iPlace * iPlace * iPlace * iPlace * iPlace + 1;
	}

template <class Val>
ZRef<Tween<Val> > sTween_QuintOut()
	{ return new Tween_Fun<Val,spTweenFun_QuintOut>; }

// =================================================================================================
#pragma mark - sTween_SinIn

template <class Val>
Val spTweenFun_SinIn(double iPlace)
	{ return -1 * cos(iPlace * M_PI / 2) + 1; }

template <class Val>
ZRef<Tween<Val> > sTween_SinIn()
	{ return new Tween_Fun<Val,spTweenFun_SinIn>; }

// =================================================================================================
#pragma mark - sTween_SinOut

template <class Val>
Val spTweenFun_SinOut(double iPlace)
	{ return sin(iPlace * M_PI / 2); }

template <class Val>
ZRef<Tween<Val> > sTween_SinOut()
	{ return new Tween_Fun<Val,spTweenFun_SinOut>; }

// =================================================================================================
#pragma mark - sTween_ExpoIn

template <class Val>
Val spTweenFun_ExpoIn(double iPlace)
	{ return iPlace <= 0 ? 0 : pow(2, 10 * (iPlace - 1)); }

template <class Val>
ZRef<Tween<Val> > sTween_ExpoIn()
	{ return new Tween_Fun<Val,spTweenFun_ExpoIn>; }

// =================================================================================================
#pragma mark - sTween_ExpoOut

template <class Val>
Val spTweenFun_ExpoOut(double iPlace)
	{ return iPlace >= 1 ? 1 : (-pow(2, -10 * iPlace) + 1); }

template <class Val>
ZRef<Tween<Val> > sTween_ExpoOut()
	{ return new Tween_Fun<Val,spTweenFun_ExpoOut>; }

// =================================================================================================
#pragma mark - sTween_CircIn

template <class Val>
Val spTweenFun_CircIn(double iPlace)
	{ return -1 * (sqrt(1 - iPlace * iPlace) - 1); }

template <class Val>
ZRef<Tween<Val> > sTween_CircIn()
	{ return new Tween_Fun<Val,spTweenFun_CircIn>; }

// =================================================================================================
#pragma mark - sTween_CircOut

template <class Val>
Val spTweenFun_CircOut(double iPlace)
	{
	iPlace -= 1;
	return sqrt(1 - iPlace * iPlace);
	}

template <class Val>
ZRef<Tween<Val> > sTween_CircOut()
	{ return new Tween_Fun<Val,spTweenFun_CircOut>; }

// =================================================================================================
#pragma mark - sTween_ElasticIn

template <class Val>
Val spTweenFun_ElasticIn(double iPlace)
	{
	iPlace -= 1;
	return -1 * (pow(2, 10*iPlace) * sin((iPlace-0.3)*2*M_PI/0.3));
	}

template <class Val>
ZRef<Tween<Val> > sTween_ElasticIn()
	{ return new Tween_Fun<Val,spTweenFun_ElasticIn>; }

// =================================================================================================
#pragma mark - sTween_ElasticOut

template <class Val>
Val spTweenFun_ElasticOut(double iPlace)
	{ return pow(2, -10*iPlace) * sin((iPlace-0.3)*2*M_PI/0.3) + 1; }

template <class Val>
ZRef<Tween<Val> > sTween_ElasticOut()
	{ return new Tween_Fun<Val,spTweenFun_ElasticOut>; }

// =================================================================================================
#pragma mark - sTween_BackIn

template <class Val>
class Tween_Fun_BackIn
:	public Tween<Val>
	{
public:
	Tween_Fun_BackIn(Val iFactor) : fFactor(iFactor) {}

	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace < 0 || iPlace >= 1.0)
			return null;
		return iPlace * iPlace * ((fFactor + 1 ) * iPlace - fFactor);
		}

//	virtual double Weight()
//		{ return 1; }

private:
	const Val fFactor;
	};

template <class Val>
ZRef<Tween<Val> > sTween_BackIn()
	{ return new Tween_Fun_BackIn<Val>(1.70158); }

template <class Val>
ZRef<Tween<Val> > sTween_BackIn(Val iFactor)
	{ return new Tween_Fun_BackIn<Val>(iFactor); }

// =================================================================================================
#pragma mark - sTween_BackOut

template <class Val>
class Tween_Fun_BackOut
:	public Tween<Val>
	{
public:
	Tween_Fun_BackOut(Val iFactor) : fFactor(iFactor) {}

	virtual ZQ<Val> QValAt(double iPlace)
		{
		if (iPlace < 0 || iPlace >= 1.0)
			return null;
		iPlace -= 1;
		return iPlace * iPlace * ((fFactor + 1) * iPlace + fFactor) + 1;
		}

//	virtual double Weight()
//		{ return 1; }

private:
	const Val fFactor;
	};

template <class Val>
ZRef<Tween<Val> > sTween_BackOut()
	{ return new Tween_Fun_BackOut<Val>(1.70158); }

template <class Val>
ZRef<Tween<Val> > sTween_BackOut(Val iFactor)
	{ return new Tween_Fun_BackOut<Val>(iFactor); }

// =================================================================================================
#pragma mark - sTween_BounceIn

template <class Val>
Val spTweenFun_BounceIn(double iPlace)
	{
	if (iPlace < 1 / 2.75)
		{
		return 7.5625 * iPlace * iPlace;
		}
	else if (iPlace < 2 / 2.75)
		{
		iPlace -= 1.5/2.75;
		return 7.5625 * iPlace * iPlace + .75;
		}
	else if (iPlace < 2.5 / 2.75)
		{
		iPlace -= 2.25/2.75;
		return 7.5625 * iPlace * iPlace + .9375;
		}
	else
		{
		iPlace -= 2.625/2.75;
		return 7.5625 * iPlace * iPlace + .984375;
		}
	}

template <class Val>
ZRef<Tween<Val> > sTween_BounceIn()
	{ return new Tween_Fun<Val,spTweenFun_BounceIn>; }

// =================================================================================================
#pragma mark - sTween_BounceOut

template <class Val>
Val spTweenFun_BounceOut(double iPlace)
	{ return spTweenFun_BounceIn<Val>(1 - iPlace); }

template <class Val>
ZRef<Tween<Val> > sTween_BounceOut()
	{ return new Tween_Fun<Val,spTweenFun_BounceOut>; }

} // namespace ZooLib

#endif // __ZooLib_Tween_Std_h__
