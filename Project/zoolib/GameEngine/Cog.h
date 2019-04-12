#ifndef __ZooLib_GameEngine_Cog_h__
#define __ZooLib_GameEngine_Cog_h__ 1

#include "zoolib/Cog.h"

#include "zoolib/GameEngine/AssetCatalog.h"
#include "zoolib/GameEngine/FontCatalog.h"
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
#pragma mark - InChannel

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
#pragma mark - OutChannel

class OutChannel
:	ZooLib::NonCopyable
	{
public:
	OutChannel(
		const ZRef<AssetCatalog>& iAssetCatalog,
		const ZRef<FontCatalog>& iFontCatalog,
		const ZRef<SoundMeister>& iSoundMeister);

	~OutChannel();

	std::vector<ZRef<TouchListener> >& GetTLs() const;
	ZRef<Rendered_Group>& GetGroup() const;

	void RegisterTouchListener(ZRef<TouchListener> iListener) const;

	const ZRef<AssetCatalog> fAssetCatalog;
	const ZRef<FontCatalog> fFontCatalog;
	const ZRef<SoundMeister> fSoundMeister;

private:
	mutable std::vector<ZRef<TouchListener> > fTLs;
	mutable ZRef<Rendered_Group> fGroup;
	};

// =================================================================================================
#pragma mark - Param and Cog

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

typedef ZooLib::Cog<const Param&> Cog;

// =================================================================================================
#pragma mark - Nooks

ZRef<Nook> sGetOneNook(const Param& iParam, const Name& iName);

std::vector<ZRef<Nook> > sGetAllNooks(const Param& iParam, const Name& iName);

template <class Nook_p>
ZRef<Nook_p> sGetOneNook(const Param& iParam)
	{ return sGetOneNook(iParam, typeid(Nook_p).name()).DynamicCast<Nook_p>(); }

template <class Nook_p>
std::vector<ZRef<Nook_p> > sGetAllNooks(const Param& iParam)
	{
	std::vector<ZRef<Nook_p> > result;
	foreacha (entry, sGetAllNooks(iParam, Name(typeid(Nook_p).name())))
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
	foreacha (entry, sGetAllNooks(iParam, Name(typeid(Nook_p).name())))
		{
		if (ZRef<Nook_p> theNook = entry.template DynamicCast<Nook_p>())
			ioNookSet.fWRNooks.insert(sWeakRef(theNook));
		}
	}

// =================================================================================================
#pragma mark - CogRegistration

class CogRegistration
	{
public:
	typedef Cog(*Fun)(const Map& iMap);

	CogRegistration(const string8& iCtorName, Fun iFun);

	static Cog sCtor(const string8& iCtorName, const Map& iMap);
	};

// =================================================================================================
#pragma mark - sCog

Cog sCog(const ZQ<Val>& iValQ);

// =================================================================================================
#pragma mark -

Cog sCog_NewEpoch(const Cog& iChild);

Cog sCog_Noop();

// =================================================================================================
#pragma mark - Touches

Cog sCog_UpdateTouchListener(const ZRef<TouchListener>& iTouchListener);
Cog sCog_TouchNothing(const ZRef<TouchListener>& iTouchListener);
Cog sCog_TouchDown(const ZRef<TouchListener>& iTouchListener);

Cog sCog_DisableTouches(const Cog& iCog);

// =================================================================================================
#pragma mark - Logging cogs

Cog sCog_Log1(int iLevel, const string8& iMessage);
Cog sCog_Log1(const string8&);
Cog sCog_Log(const string8&);

// =================================================================================================
#pragma mark - Larger collections of cogs

Cog sCogs_Plus(std::vector<Cog>* ioCogs);
Cog sCogs_Each(std::vector<Cog>* ioCogs);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Cog_h__
