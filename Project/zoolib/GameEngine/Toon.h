// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Toon_h__
#define __ZooLib_GameEngine_Toon_h__ 1
#include "zconfig.h"

#include "zoolib/CountedVal.h"

#include "zoolib/GameEngine/Tween.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Toon

typedef ZP<CountedVal<std::vector<Cel> > > CelStack;

typedef Tween<CelStack> Toon;

// =================================================================================================
#pragma mark - sToon aka sTween<CelStack>

ZP<Toon> sToon(const ZQ<Val>& iValQ);

// =================================================================================================
#pragma mark - ToonRegistration

class ToonRegistration
	{
public:
	typedef ZP<Toon>(*Fun)(const Map& iMap);

	ToonRegistration(const string8& iCtorName, Fun iFun);

	static ZP<Toon> sCtor(const string8& iCtorName, const Map& iMap);
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Toon_h__
