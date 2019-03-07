#include "zoolib/GameEngine/Cog_NookScope.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - Toon cogs

class Callable_Cog_NookScope
:	public Cog::Callable
	{
public:
	const ZRef<NookScope> fNookScope;
	const Cog fChild;

	Callable_Cog_NookScope(const ZRef<NookScope>& iNookScope, const Cog& iChild)
	:	fNookScope(iNookScope)
	,	fChild(iChild)
		{}

	virtual ZQ<Cog> QCall(const Cog& iSelf, const Param& iParam)
		{
		ZAssert(sIsPending(fChild));

		fNookScope->NewEra();

		Param theParam(iParam, InChannel(iParam.fInChannel, fNookScope));

		Cog newChild = fChild;
		if (sCallUpdate_PendingCog_Unchanged(newChild, theParam))
			return iSelf;

		if (sIsPending(newChild))
			return new Callable_Cog_NookScope(fNookScope, newChild);

		return newChild;
		}
	};

Cog sCog_NookScope(const Cog& iChild)
	{
	if (sIsPending(iChild))
		return new Callable_Cog_NookScope(new NookScope, iChild);
	return iChild;
	}

static
Cog spCogCtor_NookScope(const Map& iMap)
	{ return sCog_NookScope(sCog(iMap.QGet("Cog"))); }

static
CogRegistration spCogRegistration_NookScope("CogCtor_NookScope", spCogCtor_NookScope);

} // namespace GameEngine
} // namespace ZooLib
