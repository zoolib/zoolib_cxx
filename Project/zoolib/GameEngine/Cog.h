#ifndef __GameEngine_Cog_h__
#define __GameEngine_Cog_h__ 1

#include "zoolib/ZCog.h"

#include "zoolib/GameEngine/AssetCatalog.h"
#include "zoolib/GameEngine/Nook.h"
#include "zoolib/GameEngine/Rendered.h"
#include "zoolib/GameEngine/Touch.h"
#include "zoolib/GameEngine/Types.h"
#include "zoolib/GameEngine/Util_Allocator.h"

#include "zoolib/ZMACRO_foreach.h"

#include <map>
#include <vector>

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - InChannel

struct InChannel
	{
	InChannel(const ZRef<NookScope>& iNookScope);

	InChannel(const InChannel& iParent, const ZRef<NookScope>& iNookScope);

	InChannel(const InChannel& iParent, const Mat& iMat);

	const ZRef<NookScope>& GetActiveNookScope() const;

	const ZRef<NookScope>& GetOwnNookScope() const;

	const Mat& GetMat() const;

	const InChannel* GetPrior() const;

private:
	const InChannel* fPrior;
	ZRef<NookScope> fNookScope;
	Mat fMat;
	};

// =================================================================================================
// MARK: - OutChannel

class OutChannel
:	ZooLib::NonCopyable
	{
public:
	OutChannel(
		std::vector<ZRef<TouchListener> >* ioTLs,
		const ZRef<AssetCatalog>& iAssetCatalog,
		ZRef<Rendered_Group>& ioGroup);

	~OutChannel();

	std::vector<ZRef<TouchListener> >* GetTLs() const;
	ZRef<Rendered_Group>& GetGroup() const;

	void RegisterTouchListener(ZRef<TouchListener> iListener) const;

	size_t GetPriorTLCount() const;
	void MungeTLs(size_t iStartIndex, const Mat& iMat) const;

	void Load(const NameFrame& iNameFrame, int iPriority) const;

	const ZRef<AssetCatalog> fAssetCatalog;

private:
	std::vector<ZRef<TouchListener> >* fTLs;
	ZRef<Rendered_Group>& fGroup;
	};

// =================================================================================================
// MARK: - Param and Cog

struct Param
	{
	Param(const InChannel& iInChannel, const OutChannel& iOutChannel,
		uint64 iEra,
		double iElapsed, double iInterval, double iSubsequentTime)
	:	fEra(iEra)
	,	fInChannel(iInChannel)
	,	fOutChannel(iOutChannel)
	,	fElapsed(iElapsed)
	,	fInterval(iInterval)
	,	fSubsequentTime(iSubsequentTime)
		{}

	Param(const Param& iParam, const InChannel& iInChannel)
	:	fEra(iParam.fEra)
	,	fInChannel(iInChannel)
	,	fOutChannel(iParam.fOutChannel)
	,	fElapsed(iParam.fElapsed)
	,	fInterval(iParam.fInterval)
	,	fSubsequentTime(iParam.fSubsequentTime)
		{}

	uint64 GetEra() const
		{ return fEra; }

	const uint64 fEra;
	const InChannel& fInChannel;
	const OutChannel& fOutChannel;
	const double fElapsed;
	const double fInterval;
	const double fSubsequentTime;
	};

typedef ZCog<const Param&> Cog;

CVec3 sGetWorldPos(const Param& iParam, Rat iOffset);

ZRef<Nook> sGetOneNook(const Param& iParam, const ZName& iName);

std::vector<ZRef<Nook> > sGetAllNooks(const Param& iParam, const ZName& iName);

template <class Nook_p>
ZRef<Nook_p> sGetOneNook(const Param& iParam)
	{ return sGetOneNook(iParam, typeid(Nook_p).name()).DynamicCast<Nook_p>(); }

template <class Nook_p>
std::vector<ZRef<Nook_p> > sGetAllNooks(const Param& iParam)
	{
	std::vector<ZRef<Nook_p> > result;
	foreacha (entry, sGetAllNooks(iParam, ZName(typeid(Nook_p).name())))
		{
		if (ZRef<Nook_p> theNook = entry.template DynamicCast<Nook_p>())
			result.push_back(theNook);
		}
	return result;
	}

template <class Nook_p>
ZRef<Nook_p> sUpdateOneNook(const Param& iParam, ZWeakRef<Nook_p>& ioWR)
	{
	ZRef<Nook_p> theNook = ioWR;
	if (not theNook)
		{
		theNook = sGetOneNook<Nook_p>(iParam);
		ioWR = theNook;
		}
	return theNook;
	}

template <class Nook_p>
void sUpdateNookSet(const Param& iParam, NookSet<Nook_p>& ioNookSet)
	{
	ioNookSet.fWRNooks.clear();
	foreacha (entry, sGetAllNooks(iParam, ZName(typeid(Nook_p).name())))
		{
		if (ZRef<Nook_p> theNook = entry.template DynamicCast<Nook_p>())
			ioNookSet.fWRNooks.insert(sWeakRef(theNook));
		}
	}

// =================================================================================================
// MARK: - CogRegistration

class CogRegistration
	{
public:
	typedef Cog(*Fun)(const Map& iMap);

	CogRegistration(const string8& iCtorName, Fun iFun);

	static Cog sCtor(const string8& iCtorName, const Map& iMap);
	};

// =================================================================================================
// MARK: - sCog

Cog sCog(const ZQ<Val>& iValQ);

// =================================================================================================
// MARK: -

Cog sCog_NewEpoch(const Cog& iChild);

Cog sCog_Noop();

// =================================================================================================
// MARK: - Touches

Cog sCog_UpdateTouchListener(const ZRef<TouchListener>& iTouchListener);
Cog sCog_TouchNothing(const ZRef<TouchListener>& iTouchListener);
Cog sCog_TouchDown(const ZRef<TouchListener>& iTouchListener);

Cog sCog_DisableTouches(const Cog& iCog);

// =================================================================================================
// MARK: - Logging cogs

Cog sCog_Log1(int iLevel, const string8& iMessage);
Cog sCog_Log1(const string8&);
Cog sCog_Log(const string8&);

// =================================================================================================
// MARK: - Larger collections of cogs

Cog sCogs_Plus(std::vector<Cog>* ioCogs);
Cog sCogs_Each(std::vector<Cog>* ioCogs);

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Cog_h__
