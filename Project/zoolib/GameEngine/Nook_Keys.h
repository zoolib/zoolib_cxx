#ifndef __ZooLib_GameEngine_Nook_Keys_h__
#define __ZooLib_GameEngine_Nook_Keys_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Nook.h"
#include "zoolib/GameEngine/Types.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark -

class Nook_Keys
:	public Nook
	{
public:
	Nook_Keys(const ZRef<NookScope>& iNookScope);
	
	std::vector<int> fKeys;
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Nook_Keys_h__
