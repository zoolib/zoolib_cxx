// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __GameEngine_Tween_BlushGainMat_h__
#define __GameEngine_Tween_BlushGainMat_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Tween_Rat.h" // For Tween_Rat

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Blush

typedef Tween<Blush> Tween_Blush;

ZP<Tween_Blush> sTween_Blush(const ZP<Tween_Rat>& iTween_Rat);

// =================================================================================================
#pragma mark - Tween_Gain

typedef Tween<Gain> Tween_Gain;

ZP<Tween_Gain> sTween_Gain(const ZP<Tween_Rat>& iTween_Rat);

// =================================================================================================
#pragma mark - Tween_Mat

typedef Tween<Mat> Tween_Mat;

ZP<Tween_Mat> sTween_Mat_Identity();

// =================================================================================================
#pragma mark - BlushMat

typedef Tween<BlushGainMat> Tween_BlushGainMat;

ZP<Tween<BlushGainMat> > sTween_BlushGainMat(const Map& iMap);

ZP<Tween_BlushGainMat> sTween_BlushGainMat(const BlushGainMat& iBlushGainMat);

ZP<Tween_BlushGainMat> sTween_BlushGainMat(Blush iBlush);

ZP<Tween_BlushGainMat> sTween_BlushGainMat(const Gain& iGain);

ZP<Tween_BlushGainMat> sTween_BlushGainMat(const Mat& iMat);

ZP<Tween_BlushGainMat> sTween_BlushGainMat(
	const ZP<Tween_Blush>& iTween_Blush,
	const ZP<Tween_Gain>& iTween_Gain,
	const ZP<Tween_Mat>& iTween_Mat);

ZP<Tween_BlushGainMat> sTween_BlushGainMat(const ZP<Tween_Blush>& iTween_Blush);

ZP<Tween_BlushGainMat> sTween_BlushGainMat(const ZP<Tween_Gain>& iTween_Gain);

ZP<Tween_BlushGainMat> sTween_BlushGainMat(const ZP<Tween_Mat>& iTween_Mat);

// =================================================================================================
#pragma mark - BlushGainMatRegistration

class BlushGainMatRegistration
	{
public:
	typedef ZP<Tween_BlushGainMat>(*Fun)(const Map& iMap);

	BlushGainMatRegistration(const string8& iCtorName, Fun iFun);

	static ZP<Tween_BlushGainMat> sCtor(const string8& iCtorName, const Map& iMap);
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Tween_BlushGainMat_h__
