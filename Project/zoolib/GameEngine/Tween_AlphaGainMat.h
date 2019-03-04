#ifndef __GameEngine_Tween_AlphaGainMat_h__
#define __GameEngine_Tween_AlphaGainMat_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Tween_Rat.h" // For Tween_Rat

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - Alpha

typedef ZTween<Alpha> Tween_Alpha;

ZRef<Tween_Alpha> sTween_Alpha(const ZRef<Tween_Rat>& iTween_Rat);

// =================================================================================================
// MARK: - Tween_Gain

typedef ZTween<Gain> Tween_Gain;

ZRef<Tween_Gain> sTween_Gain(const ZRef<Tween_Rat>& iTween_Rat);

// =================================================================================================
// MARK: - Tween_Mat

typedef ZTween<Mat> Tween_Mat;

ZRef<Tween_Mat> sTween_Mat_Identity();

// =================================================================================================
// MARK: - AlphaMat

typedef ZTween<AlphaGainMat> Tween_AlphaGainMat;

ZRef<ZTween<AlphaGainMat> > sTween_AlphaGainMat(const Map& iMap);

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(const AlphaGainMat& iAlphaGainMat);

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(Alpha iAlpha);

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(const Gain& iGain);

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(const Mat& iMat);

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(
	const ZRef<Tween_Alpha>& iTween_Alpha,
	const ZRef<Tween_Gain>& iTween_Gain,
	const ZRef<Tween_Mat>& iTween_Mat);

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(const ZRef<Tween_Alpha>& iTween_Alpha);

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(const ZRef<Tween_Gain>& iTween_Gain);

ZRef<Tween_AlphaGainMat> sTween_AlphaGainMat(const ZRef<Tween_Mat>& iTween_Mat);

// =================================================================================================
// MARK: - AlphaGainMatRegistration

class AlphaGainMatRegistration
	{
public:
	typedef ZRef<Tween_AlphaGainMat>(*Fun)(const Map& iMap);

	AlphaGainMatRegistration(const string8& iCtorName, Fun iFun);

	static ZRef<Tween_AlphaGainMat> sCtor(const string8& iCtorName, const Map& iMap);
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Tween_AlphaGainMat_h__
