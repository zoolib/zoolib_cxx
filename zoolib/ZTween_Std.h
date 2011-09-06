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
Val sFun_OneMinus(Val iVal)
	{ return 1 - iVal; }

template <class Val>
ZRef<ZTween<Val> > sTween_OneMinus(const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Filter_Fun<Val,Val,sFun_OneMinus>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_SawTooth

template <class Val>
Val sFun_SawTooth(double iTime)
	{ return sMinMax(0.0, iTime, 1.0); }	

template <class Val>
ZRef<ZTween<Val> > sTween_SawTooth()
	{ return new ZTween_Fun<Val,sFun_SawTooth>; }

template <class Val>
ZRef<ZTween<Val> > sTween_SawTooth(double iDuration)
	{ return sTween_Rate(iDuration, sTween_SawTooth<Val>()); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Triangle

template <class Val>
Val sFun_Triangle(double iTime)
	{ return 1 - fabs(sMinMax(0.0, iTime, 1.0) * 2 - 1); }	

template <class Val>
ZRef<ZTween<Val> > sTween_Triangle()
	{ return new ZTween_Fun<Val,sFun_Triangle>; }

template <class Val>
ZRef<ZTween<Val> > sTween_Triangle(double iDuration)
	{ return sTween_Rate(iDuration, sTween_Triangle<Val>()); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Square

template <class Val>
Val sFun_Square(double iTime)
	{ return iTime < 0.5 ? 0.0 : 1.0; }

template <class Val>
ZRef<ZTween<Val> > sTween_Square()
	{ return new ZTween_Fun<Val,sFun_Square>; }

template <class Val>
ZRef<ZTween<Val> > sTween_Square(double iDuration)
	{ return sTween_Rate(iDuration, sTween_Square<Val>()); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Sin

template <class Val>
Val sFun_Sin(double iTime)
	{ return sin(sMinMax(0.0, iTime, 1.0) * M_PI); }	

template <class Val>
ZRef<ZTween<Val> > sTween_Sin()
	{ return new ZTween_Fun<Val,sFun_Sin>; }

template <class Val>
ZRef<ZTween<Val> > sTween_Sin(double iDuration)
	{ return sTween_Rate(iDuration, sTween_Sin<Val>()); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_Cos

template <class Val>
Val sFun_Cos(double iTime)
	{ return cos(sMinMax(0.0, iTime, 1.0) * M_PI); }	

template <class Val>
ZRef<ZTween<Val> > sTween_Cos()
	{ return new ZTween_Fun<Val,sFun_Cos>; }

template <class Val>
ZRef<ZTween<Val> > sTween_Cos(double iDuration)
	{ return sTween_Rate(iDuration, sTween_Cos<Val>()); }

} // namespace ZooLib

#endif // __ZTween_Std__
