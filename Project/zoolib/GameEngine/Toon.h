#ifndef __ZooLib_GameEngine_Toon_h__
#define __ZooLib_GameEngine_Toon_h__ 1
#include "zconfig.h"

#include "zoolib/CountedVal.h"

#include "zoolib/GameEngine/Tween.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - Toon

typedef ZRef<CountedVal<std::vector<Cel> > > CelStack;

typedef ZTween<CelStack> Toon;

// =================================================================================================
// MARK: - sToon aka sTween<CelStack>

ZRef<Toon> sToon(const ZQ<Val>& iValQ);

// =================================================================================================
// MARK: - ToonRegistration

class ToonRegistration
	{
public:
	typedef ZRef<Toon>(*Fun)(const Map& iMap);

	ToonRegistration(const string8& iCtorName, Fun iFun);

	static ZRef<Toon> sCtor(const string8& iCtorName, const Map& iMap);
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Toon_h__
