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

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZTween_Std.h" // For ZTween_Filter_Fun
#include "zoolib/gameengine/ZGameEngine_Geometry.h"

namespace ZooLib {
namespace ZGameEngine {

// =================================================================================================
#pragma mark -
#pragma mark * sTween_ScaleX

template <class Val>
ZRef<ZTween<ZMatrix<Val,4,4> > > sTween_ScaleX(const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Filter_Fun<ZMatrix<Val,4,4>,Val,sScaleX>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_ScaleY

template <class Val>
ZRef<ZTween<ZMatrix<Val,4,4> > > sTween_ScaleY(const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Filter_Fun<ZMatrix<Val,4,4>,Val,sScaleY>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_ScaleZ

template <class Val>
ZRef<ZTween<ZMatrix<Val,4,4> > > sTween_ScaleZ(const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Filter_Fun<ZMatrix<Val,4,4>,Val,sScaleZ>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_TranslateX

template <class Val>
ZRef<ZTween<ZMatrix<Val,4,4> > > sTween_TranslateX(const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Filter_Fun<ZMatrix<Val,4,4>,Val,sTranslateX>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_TranslateY

template <class Val>
ZRef<ZTween<ZMatrix<Val,4,4> > > sTween_TranslateY(const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Filter_Fun<ZMatrix<Val,4,4>,Val,sTranslateY>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_TranslateZ

template <class Val>
ZRef<ZTween<ZMatrix<Val,4,4> > > sTween_TranslateZ(const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Filter_Fun<ZMatrix<Val,4,4>,Val,sTranslateZ>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_RotateX

template <class Val>
ZMatrix<Val,4,4> sFun_Rotate2PiX(Val iVal)
	{ return sRotateX<Val>(iVal * 2 * M_PI); }

template <class Val>
ZRef<ZTween<ZMatrix<Val,4,4> > > sTween_RotateX(const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Filter_Fun<ZMatrix<Val,4,4>,Val,sFun_Rotate2PiX>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_RotateY

template <class Val>
ZMatrix<Val,4,4> sFun_Rotate2PiY(Val iVal)
	{ return sRotateY<Val>(iVal * 2 * M_PI); }

template <class Val>
ZRef<ZTween<ZMatrix<Val,4,4> > > sTween_RotateY(const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Filter_Fun<ZMatrix<Val,4,4>,Val,sFun_Rotate2PiY>(iTween); }

// =================================================================================================
#pragma mark -
#pragma mark * sTween_RotateZ

template <class Val>
ZMatrix<Val,4,4> sFun_Rotate2PiZ(Val iVal)
	{ return sRotateZ<Val>(iVal * 2 * M_PI); }

template <class Val>
ZRef<ZTween<ZMatrix<Val,4,4> > > sTween_RotateZ(const ZRef<ZTween<Val> >& iTween)
	{ return new ZTween_Filter_Fun<ZMatrix<Val,4,4>,Val,sFun_Rotate2PiZ>(iTween); }

} // namespace ZGameEngine
} // namespace ZooLib

#endif // __ZGameEngine_Tweens__
