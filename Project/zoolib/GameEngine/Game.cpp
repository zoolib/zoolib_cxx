#include "zoolib/GameEngine/Game.h"

#include "zoolib/Log.h"
#include "zoolib/ThreadVal.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/Util_string.h"

#include "zoolib/GameEngine/Cog.h"
#include "zoolib/GameEngine/DebugFlags.h"
#include "zoolib/GameEngine/Draw_GL_Shader.h"
#include "zoolib/GameEngine/DrawPreprocess.h"
#include "zoolib/GameEngine/RenderGame.h"
#include "zoolib/GameEngine/Texture_GL.h"
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
#pragma mark - Game

static Map spLoadData(const FileSpec& iFS, bool iPreferBinaryData)
	{
	ZQ<Map_Any> theMapQ;
	if (iPreferBinaryData)
		{
		if (NotZP<ChannerR_Bin> theChannerR = iFS.Child("data.bin").OpenR())
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
	const ZP<Callable_TextureFromPixmap>& iCallable_TextureFromPixmap,
	bool iPreferProcessedArt, bool iPreferSmallArt,
	const ZP<SoundMeister>& iSoundMeister)
:	fTimestamp_LatestDrawn(0)
,	fTimestamp_ToDraw(0)
,	fNookScope(new NookScope)
,	fSoundMeister(iSoundMeister)
,	fNextEra(1)
,	fAccumulated(0)
	{
	fRootMap = spLoadData(iFS, iPreferProcessedArt);

	fAssetCatalog = new AssetCatalog;

	sPopulate(fAssetCatalog,
		iFS, iCallable_TextureFromPixmap, iPreferProcessedArt, iPreferSmallArt);

	fFontCatalog = sMakeFontCatalog(iFS.Child("fonts"));

	ThreadVal<ZP<AssetCatalog>> theTV_AssetCatalog(fAssetCatalog);

	if (ZCONFIG_Debug)
		fCog = sCog(fRootMap["StartDebug"]);
	else
		fCog = sCog(fRootMap["Start"]);
	}

Game::~Game()
	{}

void Game::Initialize()
	{ ZCounted::Initialize(); }

void Game::ExternalPurgeHasOccurred()
	{
	fAssetCatalog->ExternalPurgeHasOccurred();
	Texture_GL::sOrphanAll();
	sKillContext();
	}

void Game::Purge()
	{ fAssetCatalog->Purge(); }

ZP<NookScope> Game::GetNookScope()
	{ return fNookScope; }

void Game::UpdateKeyDowns(int iKey)
	{
	ZAcqMtx acq(fMtx_Game);
	fKeyDowns.push_back(iKey);
	}

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
	const ZP<Callable_Void>& iCallable_FlipBuffers)
	{
	ThreadVal<ZP<AssetCatalog>> theTV_AssetCatalog(fAssetCatalog);

	ZP<Rendered> theRendered;
	{
	ZAcqMtx acq(fMtx_Game);

	fTimestamp_ToDraw = iNewTimestamp;
	fCnd_Game.Broadcast();

    if (not iCallable_FlipBuffers || fRendered_Prior != fRendered)
		{
		fRendered_Prior = fRendered;
		theRendered = fRendered;
		}
	}

	if (theRendered)
		{
    	GPoint theGameSize = this->GetGameSize();

		double start = Time::sSystem();
		const Rat listenerL = X(theGameSize) / 3;
		const Rat listenerR = 2 * X(theGameSize) / 3;
		const Rat listenerDistance = 5.0 * X(theGameSize) / 12;

		theRendered = Util::sFinderHider(theRendered, iBackingSize, theGameSize);

		sRenderGame(
			theRendered,
			iBackingSize,
			theGameSize,
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

void Game::WakeToBeKilled()
	{
	ZAcqMtx acq(fMtx_Game);
	fTimestamp_LatestDrawn = -1;
	fCnd_Game.Broadcast();
	fAssetCatalog->Kill();
	}

void Game::RunOnce()
	{
	double interval;
	std::vector<int> theKeyDowns;

	{
	ZAcqMtx acq(fMtx_Game);
	while (fTimestamp_ToDraw <= fTimestamp_LatestDrawn)
		fCnd_Game.Wait(fMtx_Game);

	if (fTimestamp_LatestDrawn < 0)
		return;

	interval = fTimestamp_ToDraw - fTimestamp_LatestDrawn;

	fTimestamp_LatestDrawn = fTimestamp_ToDraw;

	this->pUpdateTouches();
	theKeyDowns.swap(fKeyDowns);
	}

	ThreadVal<ZP<AssetCatalog>> theTV_AssetCatalog(fAssetCatalog);

	ZP<Rendered> theRendered = this->pCrank(interval, &theKeyDowns);

	const GPoint theGameSize = this->GetGameSize();

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
	foreachv (const ZP<Touch>& theTouch, fPendingTouchesDown)
		{
		sInsertMust(fTouchesActive, theTouch);

		set<ZP<TouchListener> > interested;
		Rat nearestInterested;

		ZP<TouchListener> exclusive;
		Rat nearestExclusive;

		foreacha (theTL, fTLs)
			{
			if (theTL->Contains(theTouch->fPos))
				{
				const Rat theZ = (theTL->GetInverseMat() * CVec3())[2];
				if (sIsEmpty(interested))
					{
					nearestInterested = theZ;
					}
				else if (nearestInterested < theZ)
					{
					nearestInterested = theZ;
					}
				interested.insert(theTL);

				if (theTL->fExclusive)
					{
					if (not exclusive)
						{
						nearestExclusive = theZ;
						exclusive = theTL;
						}
					else if (nearestExclusive < theZ)
						{
						nearestExclusive = theZ;
						exclusive = theTL;
						}
					}
				}
			}

		if (exclusive and (sIsEmpty(interested) || nearestInterested > nearestExclusive))
			{
			sInsertMust(fExclusiveTouches, theTouch, exclusive);
			if (exclusive->fActive.size() < exclusive->fMaxTouches)
				{
				sInsertMust(exclusive->fActive, theTouch);
				sInsertMust(exclusive->fDowns, theTouch);
				}
			}
		else foreachv (const ZP<TouchListener>& theTL, interested)
			{
			if (theTL->fActive.size() < theTL->fMaxTouches)
				{
				sInsertMust(theTL->fActive, theTouch);
				sInsertMust(theTL->fDowns, theTouch);
				}
			}				
		}
	fPendingTouchesDown.clear();

	foreachv (const ZP<Touch>& theTouch, fPendingTouchesMove)
		{
		ZP<TouchListener> exclusive;
		if (ZQ<ZP<TouchListener> > theQ = sQGet(fExclusiveTouches, theTouch))
			exclusive = *theQ;

		foreachv (ZP<TouchListener> theTL, fTLs)
			{
			if (not exclusive || theTL == exclusive)
				{
				if (sContains(theTL->fActive, theTouch))
					theTL->fMoves.insert(theTouch);
				}
			}
		}
	fPendingTouchesMove.clear();

	foreachv (const ZP<Touch>& theTouch, fPendingTouchesUp)
		{
		sEraseMust(fTouchesActive, theTouch);

		sInsertMust(fTouchesUp, theTouch);

		ZP<TouchListener> exclusive;
		if (ZQ<ZP<TouchListener> > theQ = sQGet(fExclusiveTouches, theTouch))
			exclusive = *theQ;

		foreachv (ZP<TouchListener> theTL, fTLs)
			{
			if (not exclusive || theTL == exclusive)
				{
				if (sContains(theTL->fActive, theTouch))
					sInsertMust(theTL->fUps, theTouch);
				}
			}
		}
	fPendingTouchesUp.clear();
	}

ZP<Rendered> Game::pCrank(double iInterval, const vector<int>* iKeyDowns)
	{
	InChannel theInChannel(this->GetGameSize(), iKeyDowns, fNookScope);
	fNookScope->NewEra();

	OutChannel theOutChannel(fRootMap, fAssetCatalog, fFontCatalog, fSoundMeister);

	iInterval *= spGameRate;
	fAccumulated += iInterval;

	const Param theParam(theInChannel, theOutChannel,
		fNextEra++,
		fAccumulated, iInterval, fAccumulated + 2*(iInterval));

	fCog = sCallCog(fCog, theParam);

	ZP<Rendered_Group> theGroup = theOutChannel.GetGroup();

	if (DebugFlags::sTouches)
		{
		const RGBA theRGBA = sRGBA(0.0, 1.0, 0.0, 0.5);
		const GRect theBounds = sGRect(-30,-30,30,30);
		foreachv (ZP<Touch> theTouch, fTouchesActive)
			{
			const GRect theRect = theBounds + sCVec2(theTouch->fPos[0], theTouch->fPos[1]);
			theGroup->Append(sFrontmost(sRendered_Rect(theRGBA, theRect)));
			}
		}

	foreachv (const ZP<Touch>& theTouch, fTouchesUp)
		{
		ZP<TouchListener> exclusive;
		if (ZQ<ZP<TouchListener> > theQ = sQGetErase(fExclusiveTouches, theTouch))
			exclusive = *theQ;

		foreachv (ZP<TouchListener> theTL, fTLs)
			{
			if (not exclusive || theTL == exclusive)
				sErase(theTL->fActive, theTouch);
			}
		}

	fTouchesUp.clear();

	foreachv (ZP<TouchListener> theTL, fTLs)
		{
		theTL->fDowns.clear();
		theTL->fMoves.clear();
		theTL->fUps.clear();
		}

	fTLs = theOutChannel.GetTLs();

	return theGroup;
	}

} // namespace GameEngine
} // namespace ZooLib
