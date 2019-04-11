#ifndef __GameEngine_Tween_BlushGainMat_h__
#define __GameEngine_Tween_BlushGainMat_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Tween_Rat.h" // For Tween_Rat

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Blush

typedef Tween<Blush> Tween_Blush;

ZRef<Tween_Blush> sTween_Blush(const ZRef<Tween_Rat>& iTween_Rat);

// =================================================================================================
#pragma mark - Tween_Gain

typedef Tween<Gain> Tween_Gain;

ZRef<Tween_Gain> sTween_Gain(const ZRef<Tween_Rat>& iTween_Rat);

// =================================================================================================
#pragma mark - Tween_Mat

typedef Tween<Mat> Tween_Mat;

ZRef<Tween_Mat> sTween_Mat_Identity();

// =================================================================================================
#pragma mark - BlushMat

typedef Tween<BlushGainMat> Tween_BlushGainMat;

ZRef<Tween<BlushGainMat> > sTween_BlushGainMat(const Map& iMap);

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(const BlushGainMat& iBlushGainMat);

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(Blush iBlush);

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(const Gain& iGain);

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(const Mat& iMat);

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(
	const ZRef<Tween_Blush>& iTween_Blush,
	const ZRef<Tween_Gain>& iTween_Gain,
	const ZRef<Tween_Mat>& iTween_Mat);

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(const ZRef<Tween_Blush>& iTween_Blush);

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(const ZRef<Tween_Gain>& iTween_Gain);

ZRef<Tween_BlushGainMat> sTween_BlushGainMat(const ZRef<Tween_Mat>& iTween_Mat);

// =================================================================================================
#pragma mark - BlushGainMatRegistration

class BlushGainMatRegistration
	{
public:
	typedef ZRef<Tween_BlushGainMat>(*Fun)(const Map& iMap);

	BlushGainMatRegistration(const string8& iCtorName, Fun iFun);

	static ZRef<Tween_BlushGainMat> sCtor(const string8& iCtorName, const Map& iMap);
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Tween_BlushGainMat_h__
