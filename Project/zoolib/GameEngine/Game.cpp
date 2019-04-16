#include "zoolib/GameEngine/Game.h"

#include "zoolib/Log.h"
#include "zoolib/ThreadVal.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/Util_string.h"

#include "zoolib/GameEngine/Cog.h"
#include "zoolib/GameEngine/DebugFlags.h"
#include "zoolib/GameEngine/DrawPreprocess.h"
#include "zoolib/GameEngine/Game_Render.h"
#include "zoolib/GameEngine/Util.h"
#include "zoolib/GameEngine/Util_Allocator.h"
#include "zoolib/GameEngine/Util_AssetCatalog.h"
#include "zoolib/GameEngine/Util_FinderHider.h"
#include "zoolib/GameEngine/Val.h"

#include <map>
#include <vector>

namespace ZooLib {
namespace GameEngine {

using namespace Util_STL;
using namespace Util_string;

using std::set;
using std::vector;

// =================================================================================================
#pragma mark - GameRate

static Rat spGameRate = 1.0;

void sRotateGameRate()
	{
	if (spGameRate < 0.1)
		spGameRate = 0.1;
	else if (spGameRate < 1.0)
		spGameRate = 1.0;
	else 
		spGameRate = 0.0;
	}

void sSetGameRate(Rat iRate)
	{ spGameRate = iRate; }

Rat sGetGameRate()
	{ return spGameRate; }

// =================================================================================================
#pragma mark - spSortTLs

namespace { // anonymous 

vector<ZRef<TouchListener> > spSortTLs(const vector<ZRef<TouchListener> >& iTLs)
	{
	typedef std::multimap<Rat,ZRef<TouchListener>, std::less<Rat>,
		TTAllocator<std::pair<const Rat, ZRef<TouchListener> > > > MapRatTL;

	RealAllocator theRealAllocator;
	MapRatTL theMap(std::less<Rat>(), theRealAllocator);

	foreachv (const ZRef<TouchListener>& theTL, iTLs)
		{
		const Rat theZ = (theTL->GetInverseMat() * CVec3())[2];
		theMap.insert(std::make_pair(theZ, theTL));
		}

	vector<ZRef<TouchListener> > result;
	foreacha (entry, theMap)
		result.push_back(entry.second);

	return result;	
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - Game

static Map spLoadData(const FileSpec& iFS, bool iPreferBinaryData)
	{
	ZQ<Map_Any> theMapQ;
	if (iPreferBinaryData)
		{
		if (ZRef<ChannerR_Bin,false> theChannerR = iFS.Child("data.bin").OpenR())
			{
			if (ZLOGF(w, eNotice))
				w << "Binary data preferred, missing 'data.bin'";
			}
		else
			{
			theMapQ = sReadBin(*theChannerR).QGet<Map_Any>();
			}
		}

	if (not theMapQ)
		theMapQ = sReadTextData(iFS.Child("data"));

	return sYadTree(*theMapQ, "_");
	}

Game::Game(const FileSpec& iFS,
	const ZRef<Callable_TextureFromPixmap>& iCallable_TextureFromPixmap,
	bool iPreferProcessedArt, bool iPreferSmallArt,
	const ZRef<SoundMeister>& iSoundMeister)
:	fTimestamp_LatestDrawn(0)
,	fTimestamp_ToDraw(0)
,	fNookScope(new NookScope)
,	fSoundMeister(iSoundMeister)
,	fNextEra(1)
,	fAccumulated(0)
	{
	fAssetCatalog = new AssetCatalog;

	sPopulate(fAssetCatalog,
		iFS, iCallable_TextureFromPixmap, iPreferProcessedArt, iPreferSmallArt);

	Map theRootMap = spLoadData(iFS, iPreferProcessedArt);

	fFontCatalog = sMakeFontCatalog(iFS.Child("fonts"));

	ThreadVal<ZRef<AssetCatalog>> theTV_AssetCatalog(fAssetCatalog);

	if (ZCONFIG_Debug)
		fCog = sCog(theRootMap["Start"]);
	else
		fCog = sCog(theRootMap["StartNoDebug"]);
	}

Game::~Game()
	{}

void Game::Initialize()
	{ ZCounted::Initialize(); }

void Game::Purge()
	{ fAssetCatalog->Purge(); }

ZRef<NookScope> Game::GetNookScope()
	{ return fNookScope; }

void Game::UpdateTouches(const TouchSet* iDown, const TouchSet* iMove, const TouchSet* iUp)
	{
	ZAcqMtx acq(fMtx_Game);
	if (iDown)
		fPendingTouchesDown.insert(iDown->begin(), iDown->end());
	if (iMove)
		fPendingTouchesMove.insert(iMove->begin(), iMove->end());
	if (iUp)
		fPendingTouchesUp.insert(iUp->begin(), iUp->end());
	}

void Game::Draw(
	double iNewTimestamp,
	GPoint iBackingSize,
	bool iUseShader,
	const ZRef<Callable_Void>& iCallable_FlipBuffers)
	{
	ThreadVal<ZRef<AssetCatalog>> theTV_AssetCatalog(fAssetCatalog);

	GPoint theGameSize = this->GetGameSize();

	ZRef<Rendered> theRendered;
	{
	ZAcqMtx acq(fMtx_Game);

	fTimestamp_ToDraw = iNewTimestamp;
	fCnd_Game.Broadcast();

	if (fRendered_Prior != fRendered)
		{
		fRendered_Prior = fRendered;
		theRendered = fRendered;
		}
	}

	if (theRendered)
		{
		double start = Time::sSystem();
		const Rat listenerL = X(theGameSize) / 3;
		const Rat listenerR = 2 * X(theGameSize) / 3;
		const Rat listenerDistance = 5.0 * X(theGameSize) / 12;

		theRendered = Util::sFinderHider(theRendered, iBackingSize, theGameSize);

		sGame_Render(
			theRendered,
			iBackingSize,
			theGameSize,
			iUseShader,
			DebugFlags::sTextureBounds, DebugFlags::sTextureBounds,
			fSoundMeister, listenerL, listenerR, listenerDistance);

		const double elapsed1 = Time::sSystem() - start;
		if (elapsed1 > 0.01)
			{
			if (ZLOGF(w, eDebug + 2))
				w << (elapsed1 * 1000) << "#########################################";
			}
		sCall(iCallable_FlipBuffers);
		const double elapsed2 = Time::sSystem() - start;
		if (elapsed2 > 0.014)
			{
			if (ZLOGF(w, eDebug + 2))
				w << (elapsed2 * 1000) << " @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
			}
		}
	}

void Game::RunOnce()
	{
	double interval;

	GPoint theGameSize = this->GetGameSize();

	{
	ZAcqMtx acq(fMtx_Game);
	while (fTimestamp_ToDraw <= fTimestamp_LatestDrawn)
		fCnd_Game.Wait(fMtx_Game);

	interval = fTimestamp_ToDraw - fTimestamp_LatestDrawn;

	fTimestamp_LatestDrawn = fTimestamp_ToDraw;

	this->pUpdateTouches();
	}

	ThreadVal<ZRef<AssetCatalog>> theTV_AssetCatalog(fAssetCatalog);

	ZRef<Rendered> theRendered = this->pCrank(interval);

	theRendered = sDrawPreprocess(
		theRendered,
		fAssetCatalog, GameEngine::DebugFlags::sTextureNameFrame,
		fFontCatalog,
		theGameSize);

	ZAcqMtx acq(fMtx_Game);
	fRendered = theRendered;
	}

void Game::pUpdateTouches()
	{
	// Go through touch downs and assign touches to listeners.
	foreachv (const ZRef<Touch>& theTouch, fPendingTouchesDown)
		{
		sInsertMust(fTouchesActive, theTouch);

		// Find any listeners that care about this touch, ordered near to far,
		// so we walk the list backwards.
		// We'll want to take account of Z-axis at some point.
		set<ZRef<TouchListener> > interested;
		ZRef<TouchListener> exclusive;

		for (auto riter = fTLs.rbegin(), end = fTLs.rend();
			riter != end; ++riter)
			{
			const ZRef<TouchListener> theListener = *riter;
			const CVec3 localPos = theListener->GetMat() * theTouch->fPos;
			if (sContains(theListener->fBounds, localPos))
				{
				if (theListener->fExclusive)
					{
					if (interested.empty())
						{
						exclusive = theListener;
						break;
						}
					}
				else
					{
					interested.insert(theListener);
					}
				}
			}

		if (exclusive)
			{
			sInsertMust(fExclusiveTouches, theTouch, exclusive);
			if (exclusive->fActive.size() < exclusive->fMaxTouches)
				{
				sInsertMust(exclusive->fActive, theTouch);
				sInsertMust(exclusive->fDowns, theTouch);
				}
			}
		else foreachv (const ZRef<TouchListener>& theListener, interested)
			{
			if (theListener->fActive.size() < theListener->fMaxTouches)
				{
				sInsertMust(theListener->fActive, theTouch);
				sInsertMust(theListener->fDowns, theTouch);
				}
			}				
		}
	fPendingTouchesDown.clear();

	foreachv (const ZRef<Touch>& theTouch, fPendingTouchesMove)
		{
		ZRef<TouchListener> exclusive;
		if (ZQ<ZRef<TouchListener> > theQ = sQGet(fExclusiveTouches, theTouch))
			exclusive = *theQ;

		foreachv (ZRef<TouchListener> theListener, fTLs)
			{
			if (not exclusive || theListener == exclusive)
				{
				if (sContains(theListener->fActive, theTouch))
					theListener->fMoves.insert(theTouch);
				}
			}
		}
	fPendingTouchesMove.clear();

	foreachv (const ZRef<Touch>& theTouch, fPendingTouchesUp)
		{
		sEraseMust(fTouchesActive, theTouch);

		sInsertMust(fTouchesUp, theTouch);

		ZRef<TouchListener> exclusive;
		if (ZQ<ZRef<TouchListener> > theQ = sQGet(fExclusiveTouches, theTouch))
			exclusive = *theQ;

		foreachv (ZRef<TouchListener> theListener, fTLs)
			{
			if (not exclusive || theListener == exclusive)
				{
				if (sContains(theListener->fActive, theTouch))
					sInsertMust(theListener->fUps, theTouch);
				}
			}
		}
	fPendingTouchesUp.clear();
	}

ZRef<Rendered> Game::pCrank(double iInterval)
	{
	InChannel theInChannel(fNookScope);
	fNookScope->NewEra();

	OutChannel theOutChannel(fAssetCatalog, fFontCatalog, fSoundMeister);

	iInterval *= spGameRate;
	fAccumulated += iInterval;

	const Param theParam(theInChannel, theOutChannel,
		fNextEra++,
		fAccumulated, iInterval, fAccumulated + 2*(iInterval));

	fCog = sCallCog(fCog, theParam);

	ZRef<Rendered_Group> theGroup = theOutChannel.GetGroup();

	if (DebugFlags::sTouches)
		{
		const RGBA theRGBA = sRGBA(0.0, 1.0, 0.0, 0.5);
		const GRect theBounds = sGRect(-30,-30,30,30);
		foreachv (ZRef<Touch> theTouch, fTouchesActive)
			{
			const GRect theRect = theBounds + sCVec2(theTouch->fPos[0], theTouch->fPos[1]);
			theGroup->Append(sFrontmost(sRendered_Rect(theRGBA, theRect)));
			}
		}

	std::vector<ZRef<TouchListener> > priorTLs = fTLs;
	fTLs = spSortTLs(theOutChannel.GetTLs());

	foreachv (const ZRef<Touch>& theTouch, fTouchesUp)
		{
		ZRef<TouchListener> exclusive;
		if (ZQ<ZRef<TouchListener> > theQ = sQGetErase(fExclusiveTouches, theTouch))
			exclusive = *theQ;

		foreachv (ZRef<TouchListener> theListener, priorTLs)
			{
			if (not exclusive || theListener == exclusive)
				sErase(theListener->fActive, theTouch);
			}
		}

	fTouchesUp.clear();

	foreachv (ZRef<TouchListener> theListener, priorTLs)
		{
		theListener->fDowns.clear();
		theListener->fMoves.clear();
		theListener->fUps.clear();
		}

	return theGroup;
	}

} // namespace GameEngine
} // namespace ZooLib
