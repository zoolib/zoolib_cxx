#ifndef __ZooLib_GameEngine_Game_h__
#define __ZooLib_GameEngine_Game_h__ 1
#include "zconfig.h"

#include "zoolib/File.h"

#include "zoolib/GameEngine/AssetCatalog.h"
#include "zoolib/GameEngine/FontCatalog.h"
#include "zoolib/GameEngine/Cog.h"
#include "zoolib/GameEngine/Sound.h"
#include "zoolib/GameEngine/Touch.h"
#include "zoolib/GameEngine/Types.h"

namespace ZooLib {
namespace GameEngine {

void sRotateGameRate();

void sSetGameRate(Rat iRate);
Rat sGetGameRate();

// =================================================================================================
#pragma mark -

class Game
:	public Counted
	{
public:
	Game(const FileSpec& iFS,
		const ZP<Callable_TextureFromPixmap>& iCallable_TextureFromPixmap,
		bool iPreferProcessedArt, bool iPreferSmallArt,
		const ZP<SoundMeister>& iSoundMeister);

	virtual ~Game();

// From Counted
	virtual void Initialize();

// Our protocol
	virtual void Resume() = 0;
	virtual void Pause() = 0;
	virtual void TearDown() = 0;

	virtual GPoint GetGameSize() = 0;

	void WakeToBeKilled();

	void ExternalPurgeHasOccurred();

	void Purge();

	ZP<NookScope> GetNookScope();

	void UpdateKeyDowns(int iKey);

	void UpdateTouches(const TouchSet* iDown, const TouchSet* iMove, const TouchSet* iUp);

	void Draw(
		double iNewTimestamp,
		GPoint iBackingSize,
		const ZP<Callable_Void>& iCallable_FlipBuffers);

	void RunOnce();

private:
	void pUpdateTouches();

	ZP<Rendered> pCrank(double iInterval, const std::vector<int>* iKeyDowns);

	ZP<Rendered> fRendered;
	ZP<Rendered> fRendered_Prior;

	ZMtx fMtx_Game;
	ZCnd fCnd_Game;

	double fTimestamp_LatestDrawn;
	double fTimestamp_ToDraw;

	ZP<NookScope> fNookScope;

	Map fRootMap;

	ZP<AssetCatalog> fAssetCatalog;

	ZP<FontCatalog> fFontCatalog;

	ZP<SoundMeister> fSoundMeister;

	std::vector<int> fKeyDowns;

	std::vector<ZP<TouchListener> > fTLs;
	std::map<ZP<Touch>,ZP<TouchListener> > fExclusiveTouches;

	TouchSet fTouchesActive;
	TouchSet fTouchesUp;

	TouchSet fPendingTouchesDown;
	TouchSet fPendingTouchesMove;
	TouchSet fPendingTouchesUp;

	uint64 fNextEra;
	double fAccumulated;
	Cog fCog;
	};

ZP<Game> sMakeGame(const FileSpec& iResourceFS, bool iPreferSmallArt);

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Game_h__
