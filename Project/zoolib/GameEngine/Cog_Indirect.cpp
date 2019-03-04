#include "zoolib/GameEngine/Cog_Indirect.h"

#include "zoolib/Callable_Indirect.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - sCog_Indirect

typedef Callable_Indirect<Cog::Signature> Callable_Indirect_Cog;

void sSetIndirect(const Cog& iLHS, const Cog& iRHS)
	{
	if (ZRef<Callable_Indirect_Cog> theIndirect = iLHS.DynamicCast<Callable_Indirect_Cog>())
		theIndirect->Set(iRHS);
	else
		ZAssert(not iLHS);
	}

Cog sCog_Indirect()
	{ return sCallable_Indirect<Cog::Signature>().StaticCast<Cog::Callable>(); }

Cog sCog_Indirect(Seq& ioSeq)
	{
	Cog theCog = sCog_Indirect();
	ioSeq.Append(theCog);
	return theCog;
	}

static
Cog spCogFun_KillIndirects(const Cog& iSelf, const Param& iParam,
	SharedState_Mutable& ioState, const string8& iName)
	{
	Seq theSeq = ioState.Get<Seq>(iName);
	for (size_t x = 0, count = theSeq.Count(); x < count; ++x)
		{
		if (ZRef<Callable_Indirect_Cog> theCallable =
			theSeq.Get<Cog>(x).DynamicCast<Callable_Indirect<Cog::Signature> >())
			{ theCallable->Set(null); }
		}
	ioState.Erase(iName);

	return true;
	}

Cog sCog_KillIndirects(SharedState_Mutable& ioState, const string8& iName)
	{
	GEMACRO_Callable(spCallable, spCogFun_KillIndirects);

	return sBindR(spCallable, ioState, iName);
	}

} // namespace GameEngine
} // namespace ZooLib
