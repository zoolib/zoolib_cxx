// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Tween_Rat_h__
#define __ZooLib_GameEngine_Tween_Rat_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Tween.h"
#include "zoolib/GameEngine/Types.h"

namespace ZooLib {
namespace GameEngine {

typedef Tween<Rat> Tween_Rat;

// =================================================================================================
#pragma mark - RatRegistration

class RatRegistration
	{
public:
	typedef ZP<Tween_Rat>(*Fun)(const Map& iMap);

	RatRegistration(const string8& iCtorName, Fun iFun);

	static ZP<Tween_Rat> sCtor(const string8& iCtorName, const Map& iMap);
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Tween_Rat_h__
